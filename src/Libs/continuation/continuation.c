/* package continuation :  Be'atrice Creusillet, 1996
 *
 * This File contains the functions computing continuation conditions 
 * of a module (over- and under-approximations.
 *
 *
 */
#include <stdio.h>
#include <string.h>

#include "genC.h"
#include "ri.h"
#include "database.h"

#include "ri-util.h"
#include "control.h"
#include "constants.h"
#include "misc.h"
#include "text-util.h"
#include "text.h"
#include "properties.h"
#include "effects.h"
#include "transformer.h"
#include "semantics.h"

#include "pipsdbm.h"
#include "resources.h"

#include "continuation.h"

/*********************************************************************************/
/* LOCAL DEFINITIONS                                                             */
/*********************************************************************************/

/* CONTINUATION CONDITION MAPS AND SUMMARIES */
GENERIC_MAPPING(/**/,must_continuation,transformer,statement)
GENERIC_MAPPING(/**/,may_continuation,transformer,statement)

static transformer must_sum_cont_t = transformer_undefined;
static transformer may_sum_cont_t = transformer_undefined;

/* statement stack */
DEFINE_LOCAL_STACK(current_stmt, statement)


/*********************************************************************************/
/* LOCAL FUNCTION HEADERS                                                        */
/*********************************************************************************/
static void module_continuation_conditions(statement mod_stat);
static void statement_continuation_conditions(statement s);
static void block_continuation_conditions(list l_stat);
static void test_continuation_conditions(test t);
static void loop_continuation_conditions(loop l);
static void call_continuation_conditions(call c);
static void unstructured_continuation_conditions(unstructured u);
static void module_summary_continuation_conditions(statement mod_stat);


/*********************************************************************************/
/* INTERFACE                                                                     */
/*********************************************************************************/

bool continuation_conditions(char *module_name)
{

    /* Get the code of the module. */
    set_current_module_statement( (statement)
	db_get_memory_resource(DBR_CODE, module_name, TRUE) );
    /* predicates defining summary regions from callees have to be 
       translated into variables local to module */
    set_current_module_entity( local_name_to_top_level_entity(module_name) );
    /* cumulated effects */
    set_cumulated_effects_map( effectsmap_to_listmap((statement_mapping)
	   db_get_memory_resource(DBR_CUMULATED_EFFECTS, module_name, TRUE)) );
    module_to_value_mappings(get_current_module_entity());
    
    /* Get the transformers of the module. */
    set_transformer_map( (statement_mapping)
	db_get_memory_resource(DBR_TRANSFORMERS, module_name, TRUE) );	

    /* initialisation of local maps */
    set_must_continuation_map( MAKE_STATEMENT_MAPPING() );
    set_may_continuation_map( MAKE_STATEMENT_MAPPING() );

    debug_on("CONTINUATION_DEBUG_LEVEL");
    module_continuation_conditions(get_current_module_statement());
    module_summary_continuation_conditions(get_current_module_statement());
    debug_off();

    DB_PUT_MEMORY_RESOURCE(DBR_MUST_CONTINUATION, 
			   strdup(module_name),
			   (char*) get_must_continuation_map() );
    DB_PUT_MEMORY_RESOURCE(DBR_MAY_CONTINUATION, 
			   strdup(module_name),
			   (char*) get_may_continuation_map() );
    DB_PUT_MEMORY_RESOURCE(DBR_MUST_SUMMARY_CONTINUATION, 
			   strdup(module_name),
			   (char*) must_sum_cont_t);
    DB_PUT_MEMORY_RESOURCE(DBR_MAY_SUMMARY_CONTINUATION, 
			   strdup(module_name),
			   (char*) may_sum_cont_t);
    reset_current_module_entity();
    reset_current_module_statement();
    reset_transformer_map();
    free_cumulated_effects_map();
    reset_must_continuation_map();
    reset_may_continuation_map();

    return(TRUE);
}

/*********************************************************************************/
/* LOCAL FUNCTION                                                                */
/*********************************************************************************/

/* INTRAPROCEDURAL PROPAGATION */

/* static void module_continuation_conditions(statement mod_stat)
 * input    : main statement of the current module
 * output   : nothing
 * modifies : continuation conditions maps
 * comment  :
 */
static void module_continuation_conditions(statement mod_stat)
{    
    make_current_stmt_stack();
    gen_recurse(mod_stat, statement_domain, gen_true,
		statement_continuation_conditions); 
    free_current_stmt_stack();
}



/* static void statement_continuation_conditions(statement s)
 * input    : the current instruction
 * output   : nothing
 * modifies : continuation conditions maps
 * comment  :
 */
static void statement_continuation_conditions(statement s)
{
    instruction i = statement_instruction(s);

    pips_debug(1, "BEGIN: statement %03d\n", statement_number(s));

    current_stmt_push(s);
    switch(instruction_tag(i))
    {
    case is_instruction_goto:
	store_statement_must_continuation(s,transformer_identity());
	store_statement_may_continuation(s,transformer_identity());
        break;

    case is_instruction_block: 
	pips_debug(3, "block\n");
	block_continuation_conditions(instruction_block(i));
	break;

    case is_instruction_test:
	pips_debug(3, "test\n");
        test_continuation_conditions(instruction_test(i));
	break;

    case is_instruction_loop:
	pips_debug(3, "loop\n");
        loop_continuation_conditions(instruction_loop(i));
        break;

    case is_instruction_call:
	pips_debug(3, "call\n");
        call_continuation_conditions(instruction_call(i));
        break;

    case is_instruction_unstructured: 
	pips_debug(3, "unstructured\n");
	unstructured_continuation_conditions(instruction_unstructured( i ));
	break ;

    default:
        pips_debug(1, "unexpected tag %d\n", instruction_tag(i));
    }
    current_stmt_pop();
    pips_debug(1,"END: statement %03d\n", statement_number(s));
}

static void block_continuation_conditions(list l_stat)
{
    statement s = current_stmt_head();
    store_statement_must_continuation(s,transformer_empty());
    store_statement_may_continuation(s,transformer_identity());
}

static void test_continuation_conditions(test t)
{
    statement s = current_stmt_head();
    store_statement_must_continuation(s,transformer_empty());
    store_statement_may_continuation(s,transformer_identity());
}

static void loop_continuation_conditions(loop l)
{
    statement s = current_stmt_head();
    store_statement_must_continuation(s,transformer_empty());
    store_statement_may_continuation(s,transformer_identity());
}

static void call_continuation_conditions(call c)
{
    statement s = current_stmt_head();
    store_statement_must_continuation(s,transformer_empty());
    store_statement_may_continuation(s,transformer_identity());
}

static void unstructured_continuation_conditions(unstructured u)
{
    statement s = current_stmt_head();
    
    store_statement_must_continuation(s,transformer_empty());
    store_statement_may_continuation(s,transformer_identity());
}


/* INTERPROCEDURAL SUMMARIZATION */

/* static void module_summary_continuation_conditions(statement mod_stat)
 * input    : main statement of the current module
 * output   : nothing
 * modifies : continuation conditions maps
 * comment  :
 */
static void module_summary_continuation_conditions(statement mod_stat)
{
    must_sum_cont_t = transformer_empty();
    may_sum_cont_t = transformer_identity();   
}
