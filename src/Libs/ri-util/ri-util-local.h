/* RI-UTIL Library: Functions dealing with and constants related to
 * PIPS intermediate representation ri.newgen,
 * that are NOT generated by NewGen
 *
 * $Id$
 *
 * $Log: ri-util-local.h,v $
 * Revision 1.89  2003/08/14 09:32:49  irigoin
 * COMPILATION_UNIT_PREFIX removed
 *
 * Revision 1.88  2003/08/12 09:14:56  irigoin
 * COMPILATION_UNIT_PREFIX addded (for Fabien's sake, as a short term solution)
 *
 * Revision 1.87  2003/08/06 13:44:54  nguyen
 * Add intrinsics for C
 *
 * Revision 1.86  2003/07/28 15:07:01  coelho
 * INV -> _INV_
 *
 * Revision 1.85  2003/07/24 07:21:03  nguyen
 * Add prefixes and new basic type sizes for C language
 *
 * Revision 1.84  2003/06/19 07:25:04  nguyen
 * Update calls to make_statement and make_variable with new RI for C
 *
 * Revision 1.83  2002/10/07 10:00:54  irigoin
 * Macros for operators ABS, DABS and CABS added.
 *
 * Revision 1.82  2002/10/04 09:29:41  irigoin
 * Check on stack area added in macro SPECIAL_AREA_P
 *
 * Revision 1.81  2002/06/17 15:29:57  irigoin
 * IO_LIST_STRING_NAME replaces "IOLIST=", duplicate names used for implied
 * do, repeat value and static initialization. DATA_LIST_FUNCTION_NAME added
 *
 * Revision 1.80  2002/06/13 13:43:26  irigoin
 * Pseudo-intrinsic STATIC-INITIALIZATION() added to encode DATA statements
 * as CALLs to STATIC_INITIALIZATION()
 *
 * Revision 1.79  2002/06/13 12:59:29  irigoin
 * Pseudo-intrinsic REPEAT_VALUE() added to encode DATA statements as CALL to REPEAT_VALUE()
 *
 */

 /* Pvecteur is an external type for NewGen and the ri data structure
  * and is not included in ri.h
  *
  * It is included here to avoid changes in many PIPS modules which
  * use ri-util
  *
  * Idem for type Ppolynome (PB 25/07/90); FI: polynomials are not
  * used in ri.newgen; they should not be mentionned here
  *
  * Idem for type Psysteme (FI 3 November 1990)
  */

#include "linear.h"

#include "text.h"

/* mapping.h inclusion
 *
 * I do that because this file was previously included in genC.h,
 * but the macros defined here use ri types (statement, entity...).
 * three typedef are also included here. ri.h is a prerequisit for 
 * mapping.h.
 *
 * FC, Feb 21, 1994
 */

/* these macros are obsolete! newgen functions (->) should be used
 * instead
 */

#ifndef STATEMENT_MAPPING_INCLUDED
#define STATEMENT_MAPPING_INCLUDED
typedef hash_table statement_mapping;
#define MAKE_STATEMENT_MAPPING() \
     (statement_mapping) hash_table_make(hash_pointer, 0)
#define FREE_STATEMENT_MAPPING(map) \
    (hash_table_free((hash_table) (map)))
#define SET_STATEMENT_MAPPING(map, stat, val) \
    hash_put((hash_table) (map), (char *)(stat), (char *)(val))
#define GET_STATEMENT_MAPPING(map, stat) \
    hash_get((hash_table) (map), (char *) (stat))
#define STATEMENT_MAPPING_COUNT(map) \
    hash_table_entry_count((hash_table) map)
#define STATEMENT_MAPPING_MAP(s, v, code, h) \
        HASH_MAP(s, v, code, h)
/*
 * this warrant BA Macro Compatibility:
 */
#define DEFINE_CURRENT_MAPPING(name, type) \
        GENERIC_CURRENT_MAPPING(name, type, statement)
#endif

#ifndef ENTITY_MAPPING_INCLUDED
#define ENTITY_MAPPING_INCLUDED
typedef hash_table entity_mapping;
#define MAKE_ENTITY_MAPPING() \
    ((entity_mapping) hash_table_make(hash_pointer, 0))
#define FREE_ENTITY_MAPPING(map) \
    (hash_table_free((hash_table) (map)))
#define SET_ENTITY_MAPPING(map, ent, val) \
    hash_put((hash_table) (map), (char *)(ent), (char *)(val))
#define GET_ENTITY_MAPPING(map, ent) \
    hash_get((hash_table) (map), (char *)(ent))
#define ENTITY_MAPPING_COUNT(map) \
    hash_table_entry_count((hash_table) map)
#define ENTITY_MAPPING_MAP(s, v, code, h) \
        HASH_MAP(s, v, code, h)
#endif

#ifndef CONTROL_MAPPING_INCLUDED
#define CONTROL_MAPPING_INCLUDED
typedef hash_table control_mapping;
#define MAKE_CONTROL_MAPPING() \
    ((control_mapping) hash_table_make(hash_pointer, 0))
#define FREE_CONTROL_MAPPING(map) \
    (hash_table_free((hash_table) (map)))
#define SET_CONTROL_MAPPING(map, cont, val) \
    hash_put((hash_table) (map), (char *)(cont), (char *)(val))
#define GET_CONTROL_MAPPING(map, cont) \
    hash_get((hash_table) (map), (char *)(cont))
#define CONTROL_MAPPING_COUNT(map) \
    hash_table_entry_count((hash_table) map)
#define CONTROL_MAPPING_MAP(s, v, code, h) \
        HASH_MAP(s, v, code, h)
#endif

/*  special characters
 */
#define MODULE_SEP 			':'

#define MODULE_SEP_STRING 		":"
#define LABEL_PREFIX 			"@"
#define MAIN_PREFIX 			"%"
#define COMMON_PREFIX 			"~"
#define BLOCKDATA_PREFIX 		"&"

#define FILE_SEP_STRING 		"%"
#define BLOCK_SEP_STRING 		"~"
#define MEMBER_SEP_STRING 		"^"
#define STRUCT_PREFIX 			"#"
#define UNION_PREFIX 			"*"
#define ENUM_PREFIX 			"?"
#define TYPEDEF_PREFIX 			"$"

/*  constant names
 */
#define BLANK_COMMON_LOCAL_NAME 	"*BLANK*"
#define DYNAMIC_AREA_LOCAL_NAME 	"*DYNAMIC*"
#define STATIC_AREA_LOCAL_NAME 		"*STATIC*"
#define HEAP_AREA_LOCAL_NAME 		"*HEAP*"
#define STACK_AREA_LOCAL_NAME 		"*STACK*"

#define EMPTY_LABEL_NAME LABEL_PREFIX
#define LIST_DIRECTED_FORMAT_NAME 	"LIST-DIRECTED"

#define TOP_LEVEL_MODULE_NAME 		"TOP-LEVEL"
#define UNBOUNDED_DIMENSION_NAME 	"UNBOUNDED-DIMENSION"
#define IMPLIED_DO_NAME 		"IMPLIED-DO"
#define IMPLIED_DO_FUNCTION_NAME 	"IMPLIED-DO"
#define REPEAT_VALUE_NAME 		"REPEAT-VALUE"
#define REPEAT_VALUE_FUNCTION_NAME 	"REPEAT-VALUE"
#define STATIC_INITIALIZATION_NAME 	"STATIC-INITIALIZATION"
#define STATIC_INITIALIZATION_FUNCTION_NAME 	"STATIC-INITIALIZATION"
#define DATA_LIST_FUNCTION_NAME 	"DATA-LIST="
#define IO_LIST_STRING_NAME             "IOLIST="
#define RETURN_LABEL_NAME 		"00000"


#define BRACE_INTRINSIC                 "BRACE_INTRINSIC"
#define NULL_STATEMENT_INTRINSIC        "NULL_STATEMENT_INTRINSIC"

#define IMPLIED_COMPLEX_NAME		"CMPLX_"
#define IMPLIED_DCOMPLEX_NAME		"DCMPLX_"

#define ASSIGN_OPERATOR_NAME 		"="
#define PLUS_OPERATOR_NAME 		"+"
#define MINUS_OPERATOR_NAME 		"-"
#define UNARY_MINUS_OPERATOR_NAME 	"--"
#define MULTIPLY_OPERATOR_NAME 		"*"
#define DIVIDE_OPERATOR_NAME 		"/"
#define INVERSE_OPERATOR_NAME           "_INV_" /* internal stuff */
#define POWER_OPERATOR_NAME 		"**"
#define MODULO_OPERATOR_NAME 		"MOD"
#define MIN_OPERATOR_NAME 		"MIN"
#define MIN0_OPERATOR_NAME 		"MIN0"
#define AMIN1_OPERATOR_NAME 		"AMIN1"
#define DMIN1_OPERATOR_NAME 		"DMIN1"
#define MAX_OPERATOR_NAME 		"MAX"
#define MAX0_OPERATOR_NAME 		"MAX0"
#define AMAX1_OPERATOR_NAME 		"AMAX1"
#define DMAX1_OPERATOR_NAME 		"DMAX1"
#define ABS_OPERATOR_NAME 		"ABS"
#define IABS_OPERATOR_NAME 		"IABS"
#define DABS_OPERATOR_NAME 		"DABS"
#define CABS_OPERATOR_NAME 		"CABS"

/* generic conversion names.
 */
#define INT_GENERIC_CONVERSION_NAME	"INT"
#define REAL_GENERIC_CONVERSION_NAME	"REAL"
#define DBLE_GENERIC_CONVERSION_NAME	"DBLE"
#define CMPLX_GENERIC_CONVERSION_NAME	"CMPLX"
#define DCMPLX_GENERIC_CONVERSION_NAME	"DCMPLX"

#define INT_TO_CHAR_CONVERSION_NAME	"CHAR"
#define CHAR_TO_INT_CONVERSION_NAME	"ICHAR"

/* FI: intrinsics are defined at a third place after bootstrap and effects!
 *     I guess the name should be defined here and used in table(s) there
 */

#define AND_OPERATOR_NAME 		".AND."
#define OR_OPERATOR_NAME 		".OR."
#define NOT_OPERATOR_NAME 		".NOT."
#define NON_EQUAL_OPERATOR_NAME 	".NE."
#define EQUIV_OPERATOR_NAME 		".EQV."
#define NON_EQUIV_OPERATOR_NAME 	".NEQV."

#define TRUE_OPERATOR_NAME 		".TRUE."
#define FALSE_OPERATOR_NAME 		".FALSE."

#define GREATER_OR_EQUAL_OPERATOR_NAME 	".GE."
#define GREATER_THAN_OPERATOR_NAME 	".GT."
#define LESS_OR_EQUAL_OPERATOR_NAME 	".LE."
#define LESS_THAN_OPERATOR_NAME 	".LT."
#define EQUAL_OPERATOR_NAME 		".EQ."

#define CONTINUE_FUNCTION_NAME 		"CONTINUE"
#define RETURN_FUNCTION_NAME 		"RETURN"
#define STOP_FUNCTION_NAME 		"STOP"
#define PAUSE_FUNCTION_NAME 		"PAUSE"

#define SUBSTRING_FUNCTION_NAME 	"_SUBSTR"
#define ASSIGN_SUBSTRING_FUNCTION_NAME 	"_ASSIGN_SUBSTR"

#define WRITE_FUNCTION_NAME 		"WRITE"
#define REWIND_FUNCTION_NAME 		"REWIND"
#define OPEN_FUNCTION_NAME 		"OPEN"
#define CLOSE_FUNCTION_NAME 		"CLOSE"
#define READ_FUNCTION_NAME 		"READ"
#define BUFFERIN_FUNCTION_NAME 		"BUFFERIN"
#define BUFFEROUT_FUNCTION_NAME 	"BUFFEROUT"
#define ENDFILE_FUNCTION_NAME 		"ENDFILE"
#define FORMAT_FUNCTION_NAME 		"FORMAT"

#define MOD_INTRINSIC_NAME 		"MOD"

/* These operators are used within the optimize transformation in
   order to manipulate operators such as n-ary add and multiply or
   multiply-add operators ( JZ - sept 98)
 */
#define EOLE_FMA_OPERATOR_NAME          "EOLE-FMA-OP"
#define EOLE_FMS_OPERATOR_NAME          "EOLE-FMS-OP"
#define EOLE_PROD_OPERATOR_NAME         "EOLE-PROD-OP"
#define EOLE_SUM_OPERATOR_NAME          "EOLE-SUM-OP"

/* moved from ricedg-local.h */
#define LOOP_COUNTER_MODULE_NAME 	"LOOP-COUNTER"
#define DI_VAR_MODULE_NAME 		"DI-VAR"

/* macros */
#define entity_an_operator_p(e,name) \
  (strcmp(entity_local_name(e), name##_OPERATOR_NAME)==0)
#define entity_a_function_p(e,name) \
  (strcmp(entity_local_name(e), name##_FUNCTION_NAME)==0)

#define ENTITY_CONVERSION_P(e,name) \
  (strcmp(entity_local_name(e), name##_GENERIC_CONVERSION_NAME)==0)
#define ENTITY_CONVERSION_CMPLX_P(e) ENTITY_CONVERSION_P(e, CMPLX)
#define ENTITY_CONVERSION_DCMPLX_P(e) ENTITY_CONVERSION_P(e, DCMPLX)

#define ENTITY_CONTINUE_P(e) entity_a_function_p(e, CONTINUE)
#define ENTITY_STOP_P(e) entity_a_function_p(e, STOP)
#define ENTITY_RETURN_P(e) entity_a_function_p(e, RETURN)

#define ENTITY_ASSIGN_P(e) entity_an_operator_p(e, ASSIGN)
#define ENTITY_PLUS_P(e) entity_an_operator_p(e, PLUS)
#define ENTITY_MINUS_P(e) entity_an_operator_p(e, MINUS)
#define ENTITY_UNARY_MINUS_P(e) entity_an_operator_p(e, UNARY_MINUS)
#define ENTITY_MULTIPLY_P(e) entity_an_operator_p(e, MULTIPLY)
#define ENTITY_MODULO_P(e) entity_an_operator_p(e, MODULO)
#define ENTITY_POWER_P(e) entity_an_operator_p(e, POWER)
#define ENTITY_DIVIDE_P(e) entity_an_operator_p(e, DIVIDE)
#define ENTITY_MIN_P(e) entity_an_operator_p(e, MIN)
#define ENTITY_MAX_P(e) entity_an_operator_p(e, MAX)
#define ENTITY_MIN0_P(e) entity_an_operator_p(e, MIN0)
#define ENTITY_MAX0_P(e) entity_an_operator_p(e, MAX0)
#define ENTITY_AMIN1_P(e) entity_an_operator_p(e, AMIN1)
#define ENTITY_AMAX1_P(e) entity_an_operator_p(e, AMAX1)
#define ENTITY_DMIN1_P(e) entity_an_operator_p(e, DMIN1)
#define ENTITY_DMAX1_P(e) entity_an_operator_p(e, DMAX1)
#define ENTITY_MIN_OR_MAX_P(e) (ENTITY_MIN_P(e) || ENTITY_MAX_P(e) )
#define ENTITY_ABS_P(e) entity_an_operator_p(e, ABS)
#define ENTITY_IABS_P(e) entity_an_operator_p(e, IABS)
#define ENTITY_DABS_P(e) entity_an_operator_p(e, DABS)
#define ENTITY_CABS_P(e) entity_an_operator_p(e, CABS)

#define ENTITY_AND_P(e) entity_an_operator_p(e, AND)

#define ENTITY_OR_P(e) entity_an_operator_p(e, OR)
#define ENTITY_NOT_P(e) entity_an_operator_p(e, NOT)
#define ENTITY_NON_EQUAL_P(e) entity_an_operator_p(e, NON_EQUAL)
#define ENTITY_EQUIV_P(e) entity_an_operator_p(e, EQUIV)
#define ENTITY_NON_EQUIV_P(e) entity_an_operator_p(e, NON_EQUIV)

/* Attention : 
   This definition is different with the Fortran Standard where the logical 
   operators are the following only: AND, OR, NOT, EQUIV, NEQUIV (NN-Mars 2000)*/

#define ENTITY_LOGICAL_OPERATOR_P(e) ( ENTITY_RELATIONAL_OPERATOR_P(e) || \
                                       ENTITY_AND_P(e) || \
                                       ENTITY_OR_P(e) || \
                                       ENTITY_NOT_P(e) || \
                                       ENTITY_NON_EQUAL_P(e) || \
                                       ENTITY_EQUIV_P(e) || \
                                       ENTITY_NON_EQUIV_P(e) )

#define ENTITY_TRUE_P(e) entity_an_operator_p(e, TRUE)
#define ENTITY_FALSE_P(e) entity_an_operator_p(e, FALSE)

#define ENTITY_GREATER_OR_EQUAL_P(e) entity_an_operator_p(e, GREATER_OR_EQUAL)
#define ENTITY_GREATER_THAN_P(e)  entity_an_operator_p(e, GREATER_THAN)
#define ENTITY_LESS_OR_EQUAL_P(e) entity_an_operator_p(e, LESS_OR_EQUAL)
#define ENTITY_LESS_THAN_P(e) entity_an_operator_p(e, LESS_THAN)

#define ENTITY_EQUAL_P(e) entity_an_operator_p(e, EQUAL)

#define ENTITY_RELATIONAL_OPERATOR_P(e) ( \
					 ENTITY_GREATER_OR_EQUAL_P(e) || \
					 ENTITY_GREATER_THAN_P(e) || \
					 ENTITY_LESS_OR_EQUAL_P(e) || \
					 ENTITY_LESS_THAN_P(e) || \
					 ENTITY_NON_EQUAL_P(e) || \
					 ENTITY_EQUAL_P(e) )

/* Special areas are sometimes tested by top_level_entity_p() because they
   are not top level entities whereas user commons are. */
#define SPECIAL_AREA_P(e) \
    ((same_string_p(module_local_name(e), DYNAMIC_AREA_LOCAL_NAME)) || \
     (same_string_p(module_local_name(e), STATIC_AREA_LOCAL_NAME)) || \
     (same_string_p(module_local_name(e), STACK_AREA_LOCAL_NAME)) || \
     (same_string_p(module_local_name(e), HEAP_AREA_LOCAL_NAME)))

#define SPECIAL_COMMON_P(e) SPECIAL_AREA_P(e)

/* IO Management
 */
#define ENTITY_NAME_P(e, name)(same_string_p(entity_local_name(e),name))

#define ENTITY_WRITE_P(e) ENTITY_NAME_P(e, "WRITE")
#define ENTITY_REWIND_P(e) ENTITY_NAME_P(e, "REWIND")
#define ENTITY_OPEN_P(e) ENTITY_NAME_P(e, "OPEN")
#define ENTITY_CLOSE_P(e) ENTITY_NAME_P(e, "CLOSE")
#define ENTITY_READ_P(e) ENTITY_NAME_P(e, "READ")
#define ENTITY_BUFFERIN_P(e) ENTITY_NAME_P(e, "BUFFERIN")
#define ENTITY_BUFFEROUT_P(e) ENTITY_NAME_P(e, "BUFFEROUT")
#define ENTITY_ENDFILE_P(e) ENTITY_NAME_P(e, "ENDFILE")
#define ENTITY_IMPLIEDDO_P(e) ENTITY_NAME_P(e, IMPLIED_DO_NAME)
#define ENTITY_IO_LIST_P(e) ENTITY_NAME_P(e, IO_LIST_STRING_NAME)
#define ENTITY_FORMAT_P(e) ENTITY_NAME_P(e, "FORMAT")

/* DATA management
 */
#define ENTITY_STATIC_INITIALIZATION_P(e) ENTITY_NAME_P(e, STATIC_INITIALIZATION_FUNCTION_NAME)
#define ENTITY_REPEAT_VALUE_P(e) ENTITY_NAME_P(e, REPEAT_VALUE_FUNCTION_NAME)
#define ENTITY_DATA_LIST_P(e) ENTITY_NAME_P(e, DATA_LIST_FUNCTION_NAME)

/* C initialization expression */
#define ENTITY_BRACE_INTRINSIC_P(e) ENTITY_NAME_P(e, BRACE_INTRINSIC)

#define ENTITY_IMPLIED_CMPLX_P(e) ENTITY_NAME_P(e, IMPLIED_COMPLEX_NAME)
#define ENTITY_IMPLIED_DCMPLX_P(e) ENTITY_NAME_P(e, IMPLIED_DCOMPLEX_NAME)

#define ENTITY_FOUR_OPERATION_P(e) ( ENTITY_PLUS_P(e) || \
                                     ENTITY_MINUS_P(e) || \
				     ENTITY_UNARY_MINUS_P(e) || \
                                     ENTITY_MULTIPLY_P(e) || \
                                     ENTITY_DIVIDE_P(e) )

#define IO_CALL_P(call) io_intrinsic_p(call_function(call))

/* classification of basics
 */
#define basic_numeric_simple_p(b) (basic_int_p(b) || basic_float_p(b))
#define basic_numeric_p(b) (basic_numeric_simple_p(b) || basic_complex_p(b))
#define basic_compatible_simple_p(b1, b2) (\
                (basic_numeric_simple_p(b1) && basic_numeric_simple_p(b2)) ||\
                (basic_string_p(b1) && basic_string_p(b2)) ||\
                (basic_logical_p(b1) && basic_logical_p(b2)) ||\
                (basic_overloaded_p(b1) && basic_overloaded_p(b2)) ||\
                (basic_undefined_p(b1) && basic_undefined_p(b2)))
#define basic_compatible_p(b1, b2) (\
                (basic_numeric_p(b1) && basic_numeric_p(b2)) ||\
                (basic_string_p(b1) && basic_string_p(b2)) ||\
                (basic_logical_p(b1) && basic_logical_p(b2)) ||\
                (basic_overloaded_p(b1) && basic_overloaded_p(b2)) ||\
                (basic_undefined_p(b1) && basic_undefined_p(b2)))


/*  constant sizes
 */
#define LABEL_SIZE 5
#define INDENTATION (get_int_property("PRETTYPRINT_INDENTATION"))
#define MAXIMAL_MODULE_NAME_SIZE 36

/*   default values
 */
#define STATEMENT_NUMBER_UNDEFINED (-1)
#define STATEMENT_ORDERING_UNDEFINED (-1)
#define UNKNOWN_RAM_OFFSET (-1)

/* On devrait utiliser Newgen pour cela, mais comme on ne doit pas
   les utiliser directement (mais via statement_less_p), cela devrait
   decourager les temeraires */

#define MAKE_ORDERING(u,s) (u<<16|s)
#define ORDERING_NUMBER(o) (o>>16)
#define ORDERING_STATEMENT(o) (o & 0xffff)

#define NORMALIZE_EXPRESSION(e) \
    ((expression_normalized(e) == normalized_undefined) ? \
         (expression_normalized(e) = NormalizeExpression(e)) : \
         (expression_normalized(e)))

/*   MISC: newgen shorthands
 */
#define entity_declarations(e) (code_declarations(entity_code(e)))

#define effect_system(e) \
	(descriptor_convex_p(effect_descriptor(e))? \
	 descriptor_convex(effect_descriptor(e)) : SC_UNDEFINED)

#define effect_reference(e) \
	 preference_reference(cell_preference(effect_cell(e)))

#define entity_variable_p(e) (type_variable_p(entity_type(e)))

#define make_simple_effect(reference,action,approximation)\
    make_effect(make_cell(is_cell_preference, make_preference(reference)),\
    (action),(approximation), \
    make_descriptor(is_descriptor_none,UU))  

#define make_convex_effect(reference,action,approximation,system)\
    make_effect(make_cell(is_cell_preference, make_preference(reference)),\
    (action),(approximation), \
    make_descriptor(is_descriptor_convex,system))  


#define statement_block_p(stat) \
	(instruction_block_p(statement_instruction(stat)))

#define entity_constant_p(e) (type_functional_p(entity_type(e)) && \
  storage_rom_p(entity_storage(e)) && value_constant_p(entity_initial(e)))

/* building instruction and statements...
 */
#define instruction_to_statement(i) \
   make_statement(entity_empty_label(),\
		  STATEMENT_NUMBER_UNDEFINED, STATEMENT_ORDERING_UNDEFINED,\
		  empty_comments, i,NIL,NULL)

#define loop_to_instruction(l) make_instruction(is_instruction_loop, l)
#define test_to_instruction(t) make_instruction(is_instruction_test, t)
#define call_to_instruction(c) make_instruction(is_instruction_call, c)

#define loop_to_statement(l) instruction_to_statement(loop_to_instruction(l))
#define test_to_statement(t) instruction_to_statement(test_to_instruction(t))
#define call_to_statement(c) instruction_to_statement(call_to_instruction(c))


/***************************************************** BOOLEAN EXPRESSIONS */
/* Building quickly boolean expressions, FC.
 */

#define unary_intrinsic_expression(name, e)\
 call_to_expression(make_call(entity_intrinsic(name),CONS(EXPRESSION,e,NIL)))

#define binary_intrinsic_expression(name, e1, e2)\
 call_to_expression(make_call(entity_intrinsic(name),\
 CONS(EXPRESSION, e1, CONS(EXPRESSION, e2, NIL))))
  
#define not_expression(e) \
    unary_intrinsic_expression(NOT_OPERATOR_NAME, e)
#define or_expression(e1, e2) \
    binary_intrinsic_expression(OR_OPERATOR_NAME, e1, e2)
#define and_expression(e1, e2) \
    binary_intrinsic_expression(AND_OPERATOR_NAME, e1, e2)
#define ne_expression(e1, e2) \
    binary_intrinsic_expression(NON_EQUAL_OPERATOR_NAME, e1, e2)
#define eq_expression(e1, e2) \
    binary_intrinsic_expression(EQUAL_OPERATOR_NAME, e1, e2)
#define gt_expression(e1, e2) \
    binary_intrinsic_expression(GREATER_THAN_OPERATOR_NAME, e1, e2)
#define ge_expression(e1, e2) \
    binary_intrinsic_expression(GREATER_OR_EQUAL_OPERATOR_NAME, e1, e2)
#define lt_expression(e1, e2) \
    binary_intrinsic_expression(LESS_THAN_OPERATOR_NAME, e1, e2)
#define le_expression(e1, e2) \
    binary_intrinsic_expression(LESS_OR_EQUAL_OPERATOR_NAME, e1, e2)


/******************************************************** CONTROL GRAPH... */
/* For the control graph modifiers: */

/* To specify the way that remove_a_control_from_a_list_and_relink
   acts: */
enum remove_a_control_from_a_list_and_relink_direction 
{
   /* Put some strange number to avoid random clash as much as
      possible... */
   source_is_predecessor_and_dest_is_successor = 119,
      source_is_successor_and_dest_is_predecessor = -123
      };
typedef enum remove_a_control_from_a_list_and_relink_direction
remove_a_control_from_a_list_and_relink_direction;


/********************************************************* DUMMY VARIABLES */
#define PRIME_LETTER_FOR_VARIABLES	"p"

/* define to build the _dummy and _prime of a variable.
 */
#define GET_DUMMY_VARIABLE_ENTITY(MODULE, NAME, lname)\
entity get_ith_##lname##_dummy(int i)\
    {return(get_ith_dummy(MODULE, NAME, i));}\
entity get_ith_##lname##_prime(int i)\
    {return(get_ith_dummy(MODULE, NAME PRIME_LETTER_FOR_VARIABLES, i));}

/* Constants for some ex-atomizer variable generation */
#define TMP_ENT 1
#define AUX_ENT 2
#define DOUBLE_PRECISION_SIZE 8


/***************************************************** HPF/HPFC DIRECTIVES */

/* moved here because needed by syntax:-(
 */
/* Directive names encoding: HPF_PREFIX + one character.
 * This encoding is achieved thru a sed script that transforms directives 
 * into calls that can be parsed by the PIPS F77 parser. It's a hack but 
 * it greatly reduced the number of lines for directive analysis, and 
 * it allowed quite simply to figure out where the executable directives
 * are in the code.
 * However the syntax allowed in mapping directives is restricted to F77.
 */

/* prefix for spacial directive calls
 */
#define HPF_PREFIX		"HPFC"

/* suffixies for encoded hpf keywords
 */
#define BLOCK_SUFFIX		"K"
#define CYCLIC_SUFFIX		"C"
#define STAR_SUFFIX		"S"

/* suffixes for HPF directives managed by HPFC
 */
#define ALIGN_SUFFIX		"A"
#define REALIGN_SUFFIX		"B"
#define DISTRIBUTE_SUFFIX	"D"
#define REDISTRIBUTE_SUFFIX	"E"
#define INDEPENDENT_SUFFIX	"I"
#define NEW_SUFFIX		"N"
#define REDUCTION_SUFFIX	"R"
#define PROCESSORS_SUFFIX	"P"
#define TEMPLATE_SUFFIX		"T"
#define PURE_SUFFIX		"U"
#define DYNAMIC_SUFFIX		"Y"

/* suffixes for my own (FCD:-) directives.
 * these directives are used to instrument the code. 
 * must be used carefully. may be ignore with some properties.
 */
#define TELL_SUFFIX		"0"
#define SYNCHRO_SUFFIX		"1"
#define TIMEON_SUFFIX		"2"
#define TIMEOFF_SUFFIX		"3"
#define SETBOOL_SUFFIX		"4"
#define SETINT_SUFFIX		"5"
#define HPFCIO_SUFFIX		"6"
#define HOSTSECTION_SUFFIX	"7"
#define DEAD_SUFFIX		"8"
#define FAKE_SUFFIX		"9"

/* property prefix for ignoring FCD directives
 * TIME, SYNCHRO and SET exists.
 */
#define FCD_IGNORE_PREFIX	"HPFC_IGNORE_FCD_"

/* soft block->sequence transition
 */
#ifdef is_instruction_sequence
#define is_instruction_block is_instruction_sequence
#define instruction_block_p(i) instruction_sequence_p(i)
#define instruction_block(i) sequence_statements(instruction_sequence(i))
#endif

/* After the modification in Newgen:  unstructured = entry:control x exit:control
   we have create a macro to transform automatically unstructured_control to unstructured_entry */
#define unstructured_control unstructured_entry

/* Default type sizes */

#define DEFAULT_CHARACTER_TYPE_SIZE (1)
#define DEFAULT_SHORT_INTEGER_TYPE_SIZE (2)
#define DEFAULT_INTEGER_TYPE_SIZE (4)
#define DEFAULT_LONG_INTEGER_TYPE_SIZE (6)
#define DEFAULT_LONG_LONG_INTEGER_TYPE_SIZE (8)

#define DEFAULT_UNSIGNED_TYPE_SIZE (1)
#define DEFAULT_SIGNED_TYPE_SIZE (2)

/* The standard integer types are represented as follow
char                   = 1
short_int              = 2
int                    = 4
long_int               = 6
long_long_int          = 8

unsigned_char          = 11
unsigned_short_int     = 12
unsigned_int           = 14
unsigned_long_int      = 16
unsigned_long_long_int = 18

signed_char            = 21
signed_short_int       = 22
signed_int             = 24
signed_long_int        = 26
signed_long_long_int   = 28

mod(i,10) refers to the basic size and div(i,10) refers that 
the variable is unsigned, signed or not */

#define DEFAULT_REAL_TYPE_SIZE (4)
#define DEFAULT_DOUBLEPRECISION_TYPE_SIZE (8)
#define DEFAULT_COMPLEX_TYPE_SIZE (8)
#define DEFAULT_DOUBLECOMPLEX_TYPE_SIZE (16)
#define DEFAULT_LOGICAL_TYPE_SIZE (4)

#define DEFAULT_POINTER_TYPE_SIZE (4)


/* Implicit variables to handle IO effetcs */

/* package name for io routines */
#define IO_EFFECTS_PACKAGE_NAME "__IO_EFFECTS"
/* array of Logical UNits; it is more or less handled as the current file pointer */
#define IO_EFFECTS_ARRAY_NAME "LUNS"
/* array of end of file codes */
#define IO_EOF_ARRAY_NAME "END_LUNS"
/* array of error codes for LUNs */
#define IO_ERROR_ARRAY_NAME "ERR_LUNS"
/* size of the unit specifier */
#define IO_EFFECTS_UNIT_SPECIFIER_LENGTH 4
/* Standard unit numbers depend on the operating system. Here are UNIX definitions. */
#define STDERR_LUN (0)
#define STDIN_LUN (5)
#define STDOUT_LUN (6)

/* Empty comments (i.e. default comments) */

#define empty_comments string_undefined

/* Macro to walk through controls reachable from the entry node of an
 * unstructured. Reachability is defined by successors and predecessors
 * (i.e. the control flow graph is seen as non-directed.
 *
 * A list of control is built. Most of the time, it must be freed
 * on exit from CONTROL_MAP().
 */

/*
#define CONTROL_MAP( ctl, code, c, list ) \
{ \
    cons *_cm_list_init = (list) ; \
    cons *_cm_list = _cm_list_init ; \
    if( _cm_list == NIL ) {\
	 get_blocs( c, &_cm_list ) ; \
         _cm_list = gen_nreverse( _cm_list ) ; \
    }\
    MAPL( _cm_ctls, {control ctl = CONTROL( CAR( _cm_ctls )) ; \
 \
		 code ;}, \
	  _cm_list ) ; \
   if( _cm_list_init == NIL ) \
        list = _cm_list ; \
}
*/

#define CONTROL_MAP( ctl, code, c, list ) \
    GENERIC_CONTROL_MAP( control_map_get_blocs, ctl, code, c, list ) 

#define BACKWARD_CONTROL_MAP( ctl, code, c, list ) \
    GENERIC_CONTROL_MAP( backward_control_map_get_blocs, ctl, code, c, list ) 

#define FORWARD_CONTROL_MAP( ctl, code, c, list ) \
    GENERIC_CONTROL_MAP( forward_control_map_get_blocs, ctl, code, c, list ) 

#define WIDE_FORWARD_CONTROL_MAP( ctl, code, c, list ) \
    GENERIC_CONTROL_MAP( wide_forward_control_map_get_blocs, ctl, code, c, list ) 

#define GENERIC_CONTROL_MAP( get_controls, ctl, code, c, list ) \
{ \
    cons *_cm_list_init = (list) ; \
    cons *_cm_list = _cm_list_init ; \
    if( _cm_list == NIL ) {\
	 get_controls( c, &_cm_list ) ; \
         _cm_list = gen_nreverse( _cm_list ) ; \
    }\
    MAPL( _cm_ctls, {control ctl = CONTROL( CAR( _cm_ctls )) ; \
 \
		 code ;}, \
	  _cm_list ) ; \
   if( _cm_list_init == NIL ) \
        list = _cm_list ; \
}

/* that is all for ri-util-local.h
 */
