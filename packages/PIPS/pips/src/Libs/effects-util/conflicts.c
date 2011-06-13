/*

 $Id$

 Copyright 1989-2010 MINES ParisTech

 This file is part of PIPS.

 PIPS is free software: you can redistribute it and/or modify it
 under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.

 PIPS is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE.

 See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with PIPS.  If not, see <http://www.gnu.org/licenses/>.

*/
#ifdef HAVE_CONFIG_H
#include "pips_config.h"
#endif

#include <stdio.h>
 
#include "linear.h"

#include "genC.h"

#include "ri.h"
#include "effects.h"

#include "misc.h"

#include "ri-util.h"
#include "effects-util.h"


/**
 * @brief Check if two effects always conflict.
 * @descriptionTwo effects will always conflict if their two abstract locations
 * has a non-empty intersection and if at least one of them is a write.
 * The approximation must also be "MUST", if one of them is a "MAY" there is no
 * conflict
 *
 * This function is conservative: it is always correct not to declare a
 * conflict.
 *
 * @return true if there is always a conflict between eff1 and eff2
 */
bool effects_must_conflict_p( effect eff1, effect eff2 ) {
  action ac1 = effect_action(eff1);
  action ac2 = effect_action(eff2);
  approximation ap1 = effect_approximation(eff1);
  approximation ap2 = effect_approximation(eff2);
  bool conflict_p = FALSE;

  /* We enforce must approximation for the two effects */
  if ( approximation_exact_p(ap1) && approximation_exact_p(ap2) ) {
    /* We enforce that at least one effect is a write */
    if ( action_write_p(ac1) || action_write_p(ac2) ) {
      cell cell1 = effect_cell(eff1);
      cell cell2 = effect_cell(eff2);
      /* Check that the cells conflicts */
      if ( cells_must_conflict_p( cell1, cell2 ) ) {
        conflict_p = TRUE;
      }
    }
  }
  return conflict_p;
}


/**
 * @brief Check if two effect might conflict, even if they are read only
 * @description Two effects may conflict if their abstract two location sets has
 * a non-empty intersection
 *
 * This function is conservative: it is always correct to declare a conflict.
 */
bool effects_might_conflict_even_read_only_p(effect eff1, effect eff2) {
  action ac1 = effect_action(eff1);
  action ac2 = effect_action(eff2);
  bool conflict_p = TRUE;

  action_kind ak1 = action_to_action_kind(ac1);
  action_kind ak2 = action_to_action_kind(ac2);

  if(action_kind_tag(ak1) != action_kind_tag(ak2)) {
    // A store mutation cannot conflict with an environment or type
    // declaration mutation
    conflict_p = FALSE;
  } else {
    if(action_kind_store_p(ak1)) {
      cell cell1 = effect_cell(eff1);
      cell cell2 = effect_cell(eff2);
      if(!cells_may_conflict_p(cell1, cell2)) {
        conflict_p = FALSE;
      }
    } else {
      /* For environment and type declarations, the references are
       empty and the conflict is only based on the referenced
       entity */
      entity v1 = effect_variable(eff1);
      entity v2 = effect_variable(eff2);
      conflict_p = v1 == v2;
    }
  }
  return conflict_p;
}


/**
 * @brief Check if two effect may conflict
 * @description Two effects may conflict if their abstract two location sets has
 * a non-empty intersection and if at least one of them is a write.
 *
 * This function is conservative: it is always correct to declare a conflict.
 */
bool effects_may_conflict_p( effect eff1, effect eff2 ) {
  action ac1 = effect_action(eff1);
  action ac2 = effect_action(eff2);
  bool conflict_p = TRUE;

  if ( action_read_p(ac1) && action_read_p(ac2) ) {
    // Two read won't conflict
    conflict_p = FALSE;
  } else {
    conflict_p = effects_might_conflict_even_read_only_p(eff1,eff2);
  }
  return conflict_p;
}


/**
 *  @brief OBSOLETE, was never used !!
 */
static bool old_effects_conflict_p( effect eff1, effect eff2 ) {
  action ac1 = effect_action(eff1);
  action ac2 = effect_action(eff2);
  bool conflict_p = FALSE;

  if ( action_write_p(ac1) || action_write_p(ac2) ) {
    if ( anywhere_effect_p( eff1 ) || anywhere_effect_p( eff2 ) )
      conflict_p = TRUE;
    else {

      reference r1 = effect_any_reference(eff1);
      entity v1 = reference_variable(r1);
      reference r2 = effect_any_reference(eff2);
      entity v2 = reference_variable(r2);

      /* Do we point to the same area? */

      if ( entities_may_conflict_p( v1, v2 ) ) {
        list ind1 = reference_indices(r1);
        list ind2 = reference_indices(r2);
        list cind1 = list_undefined;
        list cind2 = list_undefined;

        if ( v1 != v2 ) {
          /* We do not bother with the offset and the array types */
          conflict_p = TRUE;
        } else {
          if ( !ENDP(ind1) && !ENDP(ind2) ) {
            for ( cind1 = ind1, cind2 = ind2; !ENDP(cind1) && !ENDP(cind2); POP(cind1), POP(cind2) ) {
              expression e1 = EXPRESSION(CAR(cind1));
              expression e2 = EXPRESSION(CAR(cind2));
              if ( unbounded_expression_p( e1 ) || unbounded_expression_p( e2 ) )
                conflict_p = TRUE;
              else {
                intptr_t i1 = -1;
                intptr_t i2 = -1;
                bool i1_p = FALSE;
                bool i2_p = FALSE;

                i1_p = expression_integer_value( e1, &i1 );
                i2_p = expression_integer_value( e2, &i2 );
                if ( i1_p && i2_p )
                  conflict_p = ( i1 == i2 );
                else
                  conflict_p = TRUE;
              }
            }
          } else
            conflict_p = TRUE;
        }
      }

      else
        conflict_p = TRUE;
    }
  }
  return conflict_p;
}

/**
 * @brief Synonym for effects_may_conflict_p().
 *
 * @description Name preserved to limit rewriting of source code using the old
 * version. Also checks results of new implementation wrt the old implementation
 * FIXME : to be removed: was never used until now !
 */
bool effects_conflict_p( effect eff1, effect eff2 ) {
  bool conflict_p = effects_may_conflict_p( eff1, eff2 );
  bool old_conflict_p = old_effects_conflict_p( eff1, eff2 );

  /* In general, there is no reason to have the same results... This
   is only a first debugging step. */
  if ( conflict_p != old_conflict_p )
    pips_internal_error("Inconsistent conflict detection.");

  return conflict_p;
}

/**
 * @brief Check if there may be a conflict between two array references
 *
 * @description May the two references to array a using subscript list sl1 and
 * sl2 access the same memory locations?
 *
 * Subscript list sl1 and sl2 can be evaluated in two different stores.
 *
 * It is assumed that dim(a)=length(sl1)=length(sl2);
 *
 * If the nth subscript expression can be statically evaluated in both sl1
 * and sl2 and if the subscript values are different, there is not
 * conflict. For instance a[i][0] does not conflict with a[j][1].
 * SG: this is only true if you assume no out-of-bound indices
 * e.g int a[2][2]; a[1][0] conflict with a[0][2] ..
 *
 * This is about the old references_conflict_p()
 */
bool array_references_may_conflict_p( list sl1, list sl2 ) {
  bool conflict_p = TRUE;

  list cind1 = list_undefined;
  list cind2 = list_undefined;
  for ( cind1 = sl1, cind2 = sl2; conflict_p && !ENDP(cind1) && !ENDP(cind2); POP(cind1), POP(cind2) ) {
    expression e1 = EXPRESSION(CAR(cind1));
    expression e2 = EXPRESSION(CAR(cind2));
    if ( unbounded_expression_p( e1 ) || unbounded_expression_p( e2 ) )
      conflict_p = TRUE;
    else {
      intptr_t i1 = -1;
      intptr_t i2 = -1;
      bool i1_p = FALSE;
      bool i2_p = FALSE;

      i1_p = expression_integer_value( e1, &i1 );
      i2_p = expression_integer_value( e2, &i2 );
      if ( i1_p && i2_p )
        conflict_p = ( i1 == i2 );
      else
        conflict_p = TRUE;
    }
  }
  return conflict_p;
}

/**
 * @brief FIXME ?
 *
 * @description May the two references to v using subscript list sl1 and sl2
 * access the same memory locations?
 *
 * Subscript list sl1 and sl2 can be evaluated in two different stores.
 *
 * FI: this code seems to assume that ALIASING_ACROSS_DATA_STRUCTURES
 * is set to FALSE.
 */
bool variable_references_may_conflict_p( entity v, list sl1, list sl2 )
{
  bool conflict_p = TRUE;
  type t = entity_type(v);
  int sl1n = gen_length( sl1 );
  int sl2n = gen_length( sl2 );
  int nd = gen_length( variable_dimensions(type_variable(t)) );

  if ( sl1n == sl2n && sl1n == nd ) {
    /* This is equivalent to a simple Fortran-like array reference */
    conflict_p = array_references_may_conflict_p( sl1, sl2 );
  } else {
    if ( !ENDP(sl1) && !ENDP(sl2) ) {
      list cind1 = list_undefined;
      list cind2 = list_undefined;
      /* FI: this is new not really designed (!) code */
      for ( cind1 = sl1, cind2 = sl2; !ENDP(cind1) && !ENDP(cind2)
	      && conflict_p; POP(cind1), POP(cind2) ) {
	expression e1 = EXPRESSION(CAR(cind1));
	expression e2 = EXPRESSION(CAR(cind2));
	if ( unbounded_expression_p( e1 ) || unbounded_expression_p( e2 ) )
	  conflict_p = TRUE;
	else if ( expression_reference_p( e1 ) && expression_reference_p( e2 ) ) {
	  /* Because of heap modelization functions can be used as
	     subscript. Because of struct and union modelization,
	     fields can be used as subscripts. */
	  entity s1 = expression_variable( e1 ); // first subscript
	  entity s2 = expression_variable( e2 ); // second subscript
	  type s1t = entity_type(s1);
	  type s2t = entity_type(s2);

	  if ( type_equal_p( s1t, s2t ) ) {
	    if ( type_functional_p(s1t) ) {
	      /* context sensitive heap modelization */
	      conflict_p = same_string_p(entity_name(s1), entity_name(s2));
	    }
	    else if( entity_field_p(s1) &&  entity_field_p(s2))
	      {
		if(type_struct_variable_p(t)) conflict_p=same_entity_p(s1,s2);
		else if(type_union_variable_p(t)) conflict_p=true;
	      }
	  } else {
	    /* assume the code is correct... Assume no floating
	       point index... a[i] vs a[x]... */
	    conflict_p = FALSE;
	  }
	} else {
	  intptr_t i1 = -1;
	  intptr_t i2 = -1;
	  bool i1_p = FALSE;
	  bool i2_p = FALSE;

	  i1_p = expression_integer_value( e1, &i1 );
	  i2_p = expression_integer_value( e2, &i2 );
	  if ( i1_p && i2_p )
	    conflict_p = ( i1 == i2 );
	  else
	    conflict_p = TRUE;
	}
      }
    } else
      conflict_p = TRUE;
  }
  return conflict_p;
}

/**
 * @brief Check if two references may conflict
 *
 * @description Can the two references r1 and r2 access the same
 * memory location when evaluated in two different stores?
 *
 * We have to deal with static aliasing for Fortran and with
 * dynamic aliasing for C and Fortran95.
 *
 * We have to deal with abstract locations used to represent sets of
 * memory locations.
 *
 * A PIPS reference is a memory access path rather than a reference as
 * understood in programming languages:
 *
 * - a field of a struct or a union d can be accessed by subscripting
 *   d with a field number or with a field entity
 *
 * - a dereferencing is expressed by a zero subscript: *p is p[0]
 *
 * - abstract locations such as foo:*HEAP* or foo:*HEAP**MEMORY* or
 *   foo:*HEAP*_v3 can be used
 *
 * - heap modelization uses the malloc statement number as a subscript
 *
 * - flow-sensitive heap modelization can use indices from the
 *   surrounding loops as subscripts
 *
 * - context-sensitive heap modelization can also use function
 *   reference to record the call path
 *
 * Three boolean properties are involved:
 *
 * - ALIASING_ACROSS_TYPES: if false objects of different types cannot
 *   be aliased
 *
 * - ALIASING_INSIDE_DATA_STRUCTURE: if false, access paths starting
 *   from the same data structure are assumed disjoint as soon as they
 *   differ. This property holds even after pointer
 *   dereferencement. It is extremely strong and wrong for PIPS source
 *   code, unless persistant is taken into account.
 *
 * - ALIASING_ACROSS_FORMAL_PARAMETERS: if false, access paths
     starting from different parameters cannot conflict.

 */
bool references_may_conflict_p( reference r1, reference r2 ) {
  bool conflict_p = TRUE; // In doubt, conflict is assumed
  entity v1 = reference_variable(r1);
  entity v2 = reference_variable(r2);

  if ( entities_may_conflict_p( v1, v2 ) ) {
    list ind1 = reference_indices(r1);
    list ind2 = reference_indices(r2);

    if ( v1 != v2 ) {
      /* We do not bother with the offset and the array types in case
	 of static aliasing */
      /* We do not bother with the abstract locations */
      conflict_p = TRUE;
    } else {
      /* v1 == v2 */
      /* Is ALIASING_ACROSS_DATA_STRUCTURES taken into account? */
      conflict_p = variable_references_may_conflict_p( v1, ind1, ind2 );
    }
  } else {
    /* Can we have some dynamic aliasing? */
    /* Do we have aliasing between types? */
    /* Do we have aliasing within a data structure? This should have
       been checked above with
       variable_references_may_conflict_p(v1,ind1,ind2) */

    /* Can we use types to conclude there is not indirect conflict? */
    bool get_bool_property( string );
    if ( !get_bool_property( "ALIASING_ACROSS_TYPES" ) ) {
      /* No type check for abstract location
       * FIXME : really ?
       */
      if ( !entity_abstract_location_p( reference_variable(r1) )
	   && !entity_abstract_location_p( reference_variable(r2) ) ) {

	/* Are we dealing with NULL POINTER ? */
	if ( entity_null_locations_p(v1) && entity_null_locations_p(v2) )
	  conflict_p = TRUE;
	else if(entity_null_locations_p(v1) || entity_null_locations_p(v2) )
	  conflict_p = FALSE;
	else{
	  type t1 = cell_reference_to_type( r1 );
	  type t2 = cell_reference_to_type( r2 );

	  conflict_p = type_equal_p( t1, t2 );
	}
      }
    }

    /* Do we use formal parameters? */
    if ( !get_bool_property( "ALIASING_ACROSS_FORMAL_PARAMETERS" ) ) {
      /* No type check for abstract location
       * FIXME : really ?
       */
      if ( entity_formal_p( reference_variable(r1) )
	   && entity_formal_p( reference_variable(r2) ) ) {
	conflict_p = FALSE;
      }
    }

    /* Are we dealing with NULL POINTER ? */
    if ( entity_null_locations_p(v1) && entity_null_locations_p(v2) )
      conflict_p = TRUE;
    else if(entity_null_locations_p(v1) || entity_null_locations_p(v2) )
      conflict_p = FALSE;

    /* There still could be a conflict in C because the two references
       might point to the same memory location. It might be more
       effective to drop this when Fortran 77 code is analyzed because
       the lack of pointers guarantees there is no conflict. */
    if ( conflict_p ) {
      /* Do we have some dereferencing in ind1 or ind2? Do we assume
       that p[0] conflicts with any reference? We might as well use
       reference_to_abstract_location()... */
      /* Could be improved with ALIASING_ACROSS_DATA_STRUCTURES? */
      bool exact;
      // Check dereferencing in r1
      conflict_p = effect_reference_dereferencing_p( r1, &exact );
      if(!conflict_p) {
        /* We need to evaluate dereferencing in r2 only when a dereferencing
         * have not been find in r1 */
        conflict_p = effect_reference_dereferencing_p( r2, &exact );
      }
      /* In other words, we assume not conflict as soon as not pointer is
       * dereferenced... even when aliasing across types is not ignored !
       *
       * If aliasing across types is ignored, we know here that the
       * two memory locations referenced are of the same type. If the
       * pointer in one reference (let's assume only one pointer to
       * start with) is not of type pointer to the common type, then
       * there is no conflict.
       *
       * Else, we have to assume a conflict no matter what, because
       * simple cases should have been simplified via the points-to
       * analysis.
       */
    }
  }
  return conflict_p;
}

/**
 * @brief Check if two references may conflict
 *
 * @description See references_may_conflict_p
 *
 */
bool references_must_conflict_p( reference r1, reference r2 ) {
  bool conflict_p = FALSE;
  entity e1 = reference_variable(r1);
  entity e2 = reference_variable(r2);

  // Do a simple check for scalar conflicts
  if ( reference_scalar_p( r1 ) && reference_scalar_p( r2 )
      && entities_must_conflict_p( e1, e2 ) ) {
    conflict_p = TRUE;
  } else {
    /* pips_user_warning("Not completely implemented yet. "
       "Conservative under approximation is made\n");*/
  }
  return conflict_p;
}

/**
 * @brief Check if two cell may or must conflict
 *
 * @param must_p if set to true, we enforce a must conflict
 */
bool cells_maymust_conflict_p( cell c1, cell c2, bool must_p ) {
  bool conflict_p = FALSE;
  reference r1 = reference_undefined;
  reference r2 = reference_undefined;

  if ( cell_reference_p(c1) )
    r1 = cell_reference(c1);
  else if ( cell_preference_p(c1) )
    r1 = preference_reference(cell_preference(c1));

  if ( cell_reference_p(c2) )
    r2 = cell_reference(c2);
  else if ( cell_preference_p(c2) )
    r2 = preference_reference(cell_preference(c2));

  if ( reference_undefined_p(r1) || reference_undefined_p(r2) ) {
    pips_internal_error("either undefined references or gap "
        "not implemented yet\n");
  }

  conflict_p = must_p ? references_must_conflict_p( r1, r2 )
                      : references_may_conflict_p( r1, r2 );

  return conflict_p;
}

/**
 * @brief Check if two cell may conflict
 */
bool cells_may_conflict_p( cell c1, cell c2 ) {
  bool conflict_p = cells_maymust_conflict_p( c1, c2, FALSE );
  return conflict_p;
}

/**
 * @brief Check if two cell must conflict
 */
bool cells_must_conflict_p( cell c1, cell c2 ) {
  bool conflict_p = cells_maymust_conflict_p( c1, c2, TRUE );
  return conflict_p;
}

/**
 * @brief Check if two entities may or must conflict
 *
 * FI->MA: we certainly said a lot more during the February 2010
 * meeting, when abstract locations were added. And now we have store
 * and type declaration dependencies...
 *
 * @param must_p define if we enforce must conflict or only may one
 *
 * Be careful because entity_variable_p(e) does not guarantee that e
 * is a variable defined by the programmer. Maybe another function is
 * needed to make sure that the conversion to an abstract location
 * generates a useful result... variable_entity_p() is not necessarily
 * good either because it uses the entity storage to make a
 * decision. Formal parameters and return values are not taken into
 * account.
 *
 * There no abstract locations for formal parameters and return
 * values, which may not be a good idea if C let you pick up the
 * address of a formal parameter. They have to be handled in a
 * specific way.
 */
bool entities_maymust_conflict_p( entity e1, entity e2, bool must_p )
{
  bool conflict_p = !must_p; // safe default value
  bool (*abstract_locations_conflict_p)(entity,entity);

  if( must_p ) {
    abstract_locations_conflict_p = abstract_locations_must_conflict_p;
  } else  {
    abstract_locations_conflict_p = abstract_locations_may_conflict_p;
  }

  if ( entity_abstract_location_p( e1 ) )
    if ( entity_abstract_location_p( e2 ) )
      conflict_p = abstract_locations_conflict_p( e1, e2 );
    else if ( entity_null_locations_p(e2) ) {
      conflict_p = entity_all_locations_p(e1);
    }
    else if ( entity_variable_p(e2) ) {
      if ( variable_return_p( e2 ) )
	conflict_p = FALSE;
      else if ( entity_formal_p( e2 ) ) {
	/* FI: Either we need an new abstract location for the formal
	   parameters or we need to deal explictly with this case
	   here and declare conflict with *anywhere*. */
	conflict_p = entity_all_locations_p(e1);
      }
      else {
	entity al2 = variable_to_abstract_location( e2 );
	conflict_p = abstract_locations_conflict_p( e1, al2 );
      }
    } else {
      pips_internal_error("Unexpected case.");
      ;
    }
  else {
    if ( entity_abstract_location_p( e2 ) ) {
      if ( entity_null_locations_p(e1)) {
	conflict_p = entity_all_locations_p(e2);
	}
      else if ( entity_variable_p(e1) ) {
	if ( variable_return_p( e1 ) )
	  conflict_p = FALSE;
	else if ( entity_formal_p( e1 ) ) {
	  /* FI: same comment as above*/
	  conflict_p = entity_all_locations_p(e1);
	}
	else {
	  entity al1 = variable_to_abstract_location( e1 );
	  conflict_p = abstract_locations_conflict_p( al1, e2 );
	}
      } else {
	pips_internal_error("Unexpected case.");
	;
      }
    } else { /* No abstract location is involved */
      if ( variable_return_p( e1 ) && variable_return_p( e2 ) ) {
	return e1 == e2;
      }
      else if ( entity_formal_p( e1 ) && entity_formal_p( e2 ) ) {
	return e1 == e2;
      }else if( entity_null_locations_p(e1) || entity_null_locations_p(e2) ){
	return e1 == e2;
      }
      else if ( entity_variable_p(e1) && entity_variable_p(e2) ) {
	/* FIXME : variable_entity_must_conflict_p does not exist yet */
	if( !must_p) {
	  conflict_p = variable_entity_may_conflict_p( e1, e2 );
	}
	else {
	  /* A must conflict is useful to guarantee a kill, but this
	     shows that it is not related to the definition of the
	     may case: two variables may share exactly the same set
	     of memory locations but a reference to one of them does
	     not necessarily imply that all locations are read or
	     written. More comments (thinking) are needed to
	     distinguish between entity and reference conflicts. */
	  /* We assume that e1 and e2 are program variables. Because
	     we do not have enough comments, we do not know if this
	     only hold for variables and arrays of one element. It is
	     easy to argue that an array cannot must conflict with
	     itself. The test below does not solve the case of
	     struct, and maybe union. */
	  if(entity_scalar_p(e1))
	    conflict_p = e1==e2;
	  else
	    conflict_p = FALSE;
	}
      } else {
	/* FI: we end up here if references linked to environment or
	   type declarations are tested for conflicts. Should we
	   perform such tests, basically e1==e2, or assume that they
	   should have been handled at a higher level? */
	if(!variable_entity_p(e1) || variable_entity_p(e2)) {
	  /* There are no conflicts between entities of different
	     kinds */
	  /* Since this implies e1!=e2, this case could be merged
	     with the next one, but the spec would be less clear */
	  conflict_p = FALSE;
	}
	else {
	  /* Environment and type declaration conflicts imply that
	     the very same entity is involved. */
	  conflict_p = e1==e2;
	}
      }
    }
  }
  return conflict_p;
}

/**
 * @brief Check if two entities may conflict
 *
 */
bool entities_may_conflict_p( entity e1, entity e2 ) {
  return entities_maymust_conflict_p( e1, e2, FALSE);
}

/**
 * @brief Check if two entities must conflict
 *
 */
bool entities_must_conflict_p( entity e1, entity e2 ) {
  return entities_maymust_conflict_p( e1, e2, TRUE);
}
