/*

  $Id$

  Copyright 1989-2009 MINES ParisTech

  This file is part of NewGen.

  NewGen is free software: you can redistribute it and/or modify it under the
  terms of the GNU General Public License as published by the Free Software
  Foundation, either version 3 of the License, or any later version.

  NewGen is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
  License for more details.

  You should have received a copy of the GNU General Public License along with
  NewGen.  If not, see <http://www.gnu.org/licenses/>.

*/
/*
  These are the functions defined in the Newgen list library.
*/

#ifndef newgen_list_included
#define newgen_list_included

typedef struct cons {
  gen_chunk car;
  struct cons *cdr ;
} cons, * list ;

#define NIL ((cons *)NULL)
#define POP(l) ((l)=(l)->cdr)
#define ENDP(l) ((l)==NIL)

#define list_undefined ((cons *)-3)
#define list_undefined_p(c) ((c)==list_undefined)

#define CAR(pcons) ((pcons)->car)
#define CDR(pcons) ((pcons)->cdr)
#define REFCAR(pc) (&(CAR(pc).p))

#define CONS(_t_,_i_,_l_) gen_##_t_##_cons((_i_),(_l_))
#define gen_BOOL_cons gen_bool_cons
#define gen_INT_cons gen_int_cons
#define gen_LIST_cons gen_list_cons
#define gen_CONSP_cons gen_list_cons
#define gen_STRING_cons gen_string_cons
/* #define CONS(type,x,l) gen_cons((void*) (x), (l)) */

#define MAPL(_map_list_cp,_code,_l)					\
  {									\
    list _map_list_cp = (_l) ;						\
    for(; !ENDP(_map_list_cp); POP(_map_list_cp))			\
      _code;								\
  }

/* MAP(TYPE, var, code, list)
 */
#define MAP(_map_CASTER, _map_item, _map_code, _map_list)		\
  {									\
    list _map_item##_list = (_map_list);				\
    _map_CASTER##_TYPE _map_item;					\
    for(; _map_item##_list; POP(_map_item##_list))			\
    {									\
      _map_item = _map_CASTER(CAR(_map_item##_list));			\
      _map_code;							\
    }									\
  }



/* Some CPP magics to get a line-number-dependent "unique" identifier: */
#define UNIQUE_NAME_1(prefix, x)   prefix##x
#define UNIQUE_NAME_2(prefix, x)   UNIQUE_NAME_1 (prefix, x)
/* Should work if 2 FOREACH are not on the same line... */
#define UNIQUE_NAME  UNIQUE_NAME_2 (iter_, __LINE__)


/* FOREACH, similar to MAP but more gdb/emacs/vim...
 * friendly since it remains line oriented.
 *
 * FOREACH(T, v, l) generates a "v_list" variable, so this name cannot
 * be used in the same scope for another variable, and the same index
 * name cannot be used in another FOREACH in the same scope either.
 *
 * We can use a declaration in the for()
 */
#define FOREACH(_fe_CASTER, _fe_item, _fe_list) \
  list UNIQUE_NAME = (_fe_list);		\
  for( _fe_CASTER##_TYPE _fe_item;					\
       !ENDP(UNIQUE_NAME) && (_fe_item= _fe_CASTER(CAR(UNIQUE_NAME) ));	\
       POP(UNIQUE_NAME))


/* Fonctions de list.c
 */
extern list gen_append GEN_PROTO(( list , list )) ;
extern list gen_concatenate GEN_PROTO(( list , list )) ;
extern void gen_copy GEN_PROTO(( void *, void *)) ;
extern list gen_copy_seq GEN_PROTO(( list )) ;
extern int gen_eq GEN_PROTO(( void *, void * )) ;
extern void *gen_car GEN_PROTO((list));
extern void *gen_identity GEN_PROTO((void*));
extern void *gen_find GEN_PROTO((void *, list ,
				  bool (*)(), void *(*)() )) ;
extern void *gen_find_from_end GEN_PROTO((void *, list ,
				  bool (*)(), void *(*)() )) ;
extern void *gen_find_eq GEN_PROTO(( void *, list )) ;
extern void *gen_find_if GEN_PROTO(( bool (*)(), list ,
					 void *(*)())) ;
extern void *gen_find_if_from_end GEN_PROTO((bool (*)(), list ,
					      void *(*)())) ;
extern void *gen_find_tabulated GEN_PROTO(( char *, int )) ;
extern list gen_filter_tabulated GEN_PROTO(( bool (*)(gen_chunk*), int )) ;
extern void gen_free_area GEN_PROTO((void**, int)) ;
extern void gen_free_list GEN_PROTO(( list )) ;
extern void gen_full_free_list GEN_PROTO(( list ));
extern list gen_last GEN_PROTO(( list )) ;
extern size_t gen_length GEN_PROTO(( list )) ;
extern size_t list_own_allocated_memory GEN_PROTO((list));
extern void gen_map GEN_PROTO(( void (*)(), list )) ;
extern void gen_mapl GEN_PROTO(( void (*)(), list )) ;
extern void gen_mapc_tabulated GEN_PROTO(( void (*)(), int )) ;
extern list gen_nconc GEN_PROTO(( list , list )) ;
extern list gen_full_copy_list GEN_PROTO((list)) ;
extern list gen_nreverse GEN_PROTO(( list )) ;
extern gen_chunk gen_nth GEN_PROTO(( int, list )) ;
extern list gen_nthcdr GEN_PROTO(( int, list )) ;
extern char *gen_reduce GEN_PROTO(( char *, char *(*)(), list )) ;
extern void gen_remove GEN_PROTO(( list *, void * )) ;
extern void gen_remove_once GEN_PROTO(( list *, void * )) ;
extern list gen_some  GEN_PROTO(( bool(*)(), list )) ;
extern void gen_insert_after GEN_PROTO((void *, void *, list )) ;
extern list gen_insert_before GEN_PROTO((void * no, void * o, list l)) ;
extern list gen_once GEN_PROTO((void *, list));
extern bool gen_in_list_p GEN_PROTO((void *, list));
extern int gen_occurences GEN_PROTO((void *, list));
extern bool gen_once_p GEN_PROTO((list));
extern void gen_sort_list GEN_PROTO((list, int (*)())) ;
extern void gen_closure GEN_PROTO((list (*)(), list));
extern list gen_make_list GEN_PROTO((int, ...));
extern list gen_copy_string_list GEN_PROTO((list));
extern void gen_free_string_list GEN_PROTO((list));
extern list gen_cons GEN_PROTO((void *, list));
extern list gen_bool_cons GEN_PROTO((bool, list));
extern list gen_int_cons GEN_PROTO((_int, list));
extern list gen_string_cons GEN_PROTO((string, list));
extern list gen_list_cons GEN_PROTO((list, list));
extern list gen_typed_cons GEN_PROTO((_int, void *, list));
extern list gen_CHUNK_cons GEN_PROTO((gen_chunk *, list));
extern void gen_list_and GEN_PROTO((list *, list));
extern void gen_list_and_not GEN_PROTO((list *, list));
extern void gen_list_patch GEN_PROTO((list, void *, void *));
extern int gen_position GEN_PROTO((void *, list));

#endif /* newgen_list_included */
