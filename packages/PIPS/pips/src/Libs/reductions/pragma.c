/**
 * @file pragma.c
 * @brief This file holds the generation of omp pragma for reductions.
 * @author pierre villalon <pierre.villalon@hpc-project.com>
 * @date 2009-05-24
 */
#ifdef HAVE_CONFIG_H
    #include "pips_config.h"
#endif

#include "local-header.h"

//***********************************************************Local constant
static const string OMP_PRAGMA_FOR_HEADER_C = "omp parallel for";
static const string OMP_PRAGMA_FOR_HEADER_F = "omp parallel do";
static const string REDUCTION_KEYWORD = "reduction";

//***********************************************************Local variable
static bool all_reduction = false;


//***********************************************************Local functions

///@brief reset the all_reduction flag
static void reset_all_reduction (void) {
  all_reduction = true;
#ifdef HAVE_CONFIG_H
    #include "pips_config.h"
#endif
}

///@return true if the statement is a reduction
///@param stmt, the statement to test for reduction
static bool statement_is_reduction (statement stmt) {
  // test that we have a reachable statement
  if (bound_printed_reductions_p (stmt) == false) return false;
  int size = gen_length (reductions_list (load_printed_reductions(stmt)));
  return (size != 0);
}

///@brief remenber if all the statement analazed are reduction
///since the last reset
///@return void
///@param stmt, the statement to test for reduction
static void compute_all_reduction (statement stmt) {
  all_reduction = all_reduction && statement_is_reduction (stmt);
}

///@return true if the reductions are applied on scalars only
///@param reds, the reductions to analyze
static bool reductions_on_scalar (reductions reds) {
  FOREACH (REDUCTION, red, reductions_list(reds)) {
    reference ref = reduction_reference (red);
    if (reference_indices (ref) != NIL) return false;
  }
  return true;
}

//******************************************************PRAGMA AS EXPRESSIONS

///@return an entiy describing the reduction operator
///@param o the reduction operator to analyze
static entity omp_operator_entity (reduction_operator o) {

  entity result = entity_undefined;
  tag t = reduction_operator_tag(o);

  switch(t) {
  case is_reduction_operator_none:
    pips_internal_error("unexpected none reduction operator tag!");
    break;
  case is_reduction_operator_sum:
    result = CreateIntrinsic(PLUS_OPERATOR_NAME);
    break;
  case is_reduction_operator_csum:
    result = CreateIntrinsic(PLUS_C_OPERATOR_NAME);
    break;
  case is_reduction_operator_prod:
    result = CreateIntrinsic(MULTIPLY_OPERATOR_NAME);
    break;
  case is_reduction_operator_min:
    result = CreateIntrinsic(MIN_OPERATOR_NAME);
    break;
  case is_reduction_operator_max:
    result = CreateIntrinsic(MAX_OPERATOR_NAME);
    break;
  case is_reduction_operator_and:
    result = (prettyprint_language_is_fortran_p () == true ?
	      CreateIntrinsic(AND_OPERATOR_NAME) :
	      CreateIntrinsic(C_AND_OPERATOR_NAME));
    break;
  case is_reduction_operator_or:
    result = (prettyprint_language_is_fortran_p () == true ?
	      CreateIntrinsic(OR_OPERATOR_NAME) :
	      CreateIntrinsic(C_OR_OPERATOR_NAME));
    break;
  case is_reduction_operator_bitwise_or:
    result = CreateIntrinsic(BITWISE_OR_OPERATOR_NAME);
    break;
  case is_reduction_operator_bitwise_xor:
    result = CreateIntrinsic(BITWISE_XOR_OPERATOR_NAME);
    break;
  case is_reduction_operator_bitwise_and:
    result = CreateIntrinsic(BITWISE_AND_OPERATOR_NAME);
    break;
  case is_reduction_operator_eqv:
    pips_assert ("not a C reduction operator", prettyprint_language_is_fortran_p () == true);
    result = CreateIntrinsic(EQUIV_OPERATOR_NAME);
    break;
  case is_reduction_operator_neqv:
    pips_assert ("not a C reduction operator", prettyprint_language_is_fortran_p () == true);
    result = CreateIntrinsic(NON_EQUIV_OPERATOR_NAME);
    break;
  default:
    pips_internal_error("unexpected reduction operator tag!");
    break;
  }
  return result;
}

///@return a list of expression for reduction r
///@param r the reduction to process
static expression reduction_as_expr (reduction r)
{
  // prepare the expressions list that specifies the variable
  // and the operator
  reference ref = copy_reference (reduction_reference(r));
  syntax s = make_syntax_reference (ref);
  expression expr = make_expression (s, normalized_undefined);
  list args = NIL;
  args = gen_expression_cons (expr, args);
  // and now the operator
  reduction_operator op = reduction_op (r);
  ref = make_reference (omp_operator_entity (op), NIL);
  s = make_syntax_reference (ref);
  expr = make_expression (s, normalized_undefined);
  args = gen_expression_cons (expr, args);

  // first prepare "reduction" as an expression
  entity e = CreateIntrinsic(OMP_REDUCTION_FUNCTION_NAME);
  call c = make_call (e, args);
  s = make_syntax_call (c);
  expression result = make_expression (s, normalized_undefined);

  pips_debug(5, "finish\n");
  return result;
}

//***********************************************************PRAGMA AS STRING

///@return a (static) string describing the reduction operator for open mp
///@param o the reduction operator to analyze
static string omp_operator_str (reduction_operator o) {

  tag t = reduction_operator_tag(o);
  string result = string_undefined;

  switch(t) {
  case is_reduction_operator_none:
    result = "none";
    break;
  case is_reduction_operator_sum:
    result = "+";
    break;
  case is_reduction_operator_csum:
    result = "+";
    break;
  case is_reduction_operator_prod:
    result = "*";
    break;
  case is_reduction_operator_min:
    result = "MIN";
    break;
  case is_reduction_operator_max:
    result = "MAX";
    break;
  case is_reduction_operator_and:
    result = (prettyprint_language_is_fortran_p () == true) ? ".AND." : "&&";
    break;
  case is_reduction_operator_or:
    result = (prettyprint_language_is_fortran_p () == true) ? ".OR." :"||";
    break;
  case is_reduction_operator_bitwise_or:
    result = "|";
    break;
  case is_reduction_operator_bitwise_xor:
    result = "^";
    break;
  case is_reduction_operator_bitwise_and:
    result = "&";
    break;
  case is_reduction_operator_eqv:
    pips_assert ("not a C reduction operator", prettyprint_language_is_fortran_p () == true);
    result = ".EQV.";
    break;
  case is_reduction_operator_neqv:
    pips_assert ("not a C reduction operator", prettyprint_language_is_fortran_p () == true);
    result = ".NEQV.";
    break;
  default:
    pips_internal_error("unexpected reduction operator tag!");
    break;
  }
  return result;
}

/* allocates and returns a string for reduction r
 */
static string reduction_as_str (reduction r)
{
  string str;
  str = concatenate (REDUCTION_KEYWORD,
					 "(", omp_operator_str (reduction_op(r)), ":",
					 words_to_string(words_reference(reduction_reference(r),NIL)),
					 ")", NULL);
  pips_debug(5, "finish with string: %s\n", str);
  return strdup (str);
}

//***********************************************************Global functions
// all global function names start with reductions_

///@brief initialize all what is needed to generate omp pragma
///@param mod_name, the module name to analyze for reductions
void reductions_pragma_omp_init (const char* mod_name) {
  // prepare data structure for reductions
  set_printed_reductions((pstatement_reductions)
			 db_get_memory_resource(DBR_CUMULATED_REDUCTIONS,
						mod_name, true));
}

///@brief release what have been initialize before
void reductions_pragma_omp_end (void) {
  //  release data structure for reductions
  reset_printed_reductions();
}

//@return a list of expressions with omp pragma for reductions, NULL if nothing
//to return
//@param l, the loop associated with the statement
//@param stmt, the statement to analyzed for reductions, must be a loop
list reductions_get_omp_pragma_expr (loop l, statement stmt) {
  list exprs = NIL;
  // check that reduction as been detected at loop level
  if  (statement_is_reduction (stmt) == true) {
    reductions rs = load_printed_reductions(stmt);
    // check that the reductions are done on scalars and not arrays
    if (reductions_on_scalar (rs) == true) {
      // reset the all reduction flag
      reset_all_reduction ();
      // check that all the statements of the loop are reductions otherwise, do
      // not generate omp reduction pragma
      // the test is too restrictive so need to be improved
      gen_recurse(l, statement_domain, gen_true, compute_all_reduction);
      if (all_reduction == true) {
				reductions rs = load_printed_reductions(stmt);
				FOREACH (REDUCTION, red, reductions_list(rs)) {
				  exprs = gen_expression_cons (reduction_as_expr (red), exprs);
				}
				//secondly get "omp parallel for" as an expr and concatenate
				list parallel_for = pragma_omp_parallel_for_as_exprs ();
				exprs = gen_nconc (exprs, parallel_for);
      }
    }
  }
  pips_debug(5, "finish with pragma\n");
  return exprs;
}

///@return a string with omp pragma for reductions
///@param l, the loop associated with the statement
///@param stmt, the statement to analyzed for reductions, must be a loop
string reductions_get_omp_pragma_str (loop l, statement stmt) {
  string_buffer buf  = string_buffer_make (false);
  // check that reduction as been detected at loop level
  if  (statement_is_reduction (stmt) == true) {
    reductions rs = load_printed_reductions(stmt);
    // check that the reductions are done on scalars and not arrays
    if (reductions_on_scalar (rs) == true) {
      // reset the all reduction flag
      reset_all_reduction ();
      // check that all the statements of the loop are reductions otherwise, do
      // not generate omp reduction pragma
      // the test is too restrictive so need to be improved
      gen_recurse(l, statement_domain, gen_true, compute_all_reduction);
      if (all_reduction == true) {
	string header = (prettyprint_language_is_fortran_p ()
			 ? OMP_PRAGMA_FOR_HEADER_F :
			 OMP_PRAGMA_FOR_HEADER_C);
	string_buffer_append (buf, strdup (header));
	string_buffer_append (buf, strdup (" "));
	FOREACH (REDUCTION, red, reductions_list(rs)) {
	  string_buffer_append (buf, reduction_as_str (red));
	}
      }
    }
  }
  string result = string_buffer_to_string (buf);
  pips_debug(5, "finish with pragma: %s\n", result);
  string_buffer_free_all (&buf);
  return result;
}