/* HPFC module by Fabien COELHO
 *
 * $RCSfile: hpfc.c,v $ ($Date: 1996/06/15 19:15:29 $, )
 * version $Revision$
 */
 
#include "defines-local.h"

#include "regions.h"
#include "semantics.h"
#include "effects.h"
#include "resources.h"
#include "pipsdbm.h"
#include "control.h"

/*************************************************************** REDUCTIONS */
GENERIC_GLOBAL_FUNCTION(hpf_reductions, statement_entities)

/****************************************************************** COMMONS */

GENERIC_STATIC_STATUS(static, the_commons, list, NIL, gen_free_list)

void add_a_common(entity c)
{ the_commons_object = gen_once(c, the_commons_object);}

static void compile_common(entity c)
{
    declaration_with_overlaps_for_module(c);
    clean_common_declaration(load_new_host(c));
    put_generated_resources_for_common(c);
}

/******************************************************************** PURES */

GENERIC_STATIC_STATUS(static, the_pures, list, NIL, gen_free_list)

void add_a_pure(entity f)
{ the_pures_object = gen_once(f, the_pures_object);}

/* ??? some intrinsics should also be considered as pure. all of them ?
 */
bool hpf_pure_p(entity f)
{ return gen_in_list_p(f, the_pures_object);}

/********************************************************************** IOS */

GENERIC_STATIC_STATUS(static, the_ios, list, NIL, gen_free_list)

void add_an_io_function(entity f)
{ the_ios_object = gen_once(f, the_ios_object);}

bool hpfc_special_io(entity f)
{ return gen_in_list_p(f, the_ios_object);}

/*************************************************************** REMAPPINGS */

/* list of already computed remappings...
 */
GENERIC_STATIC_STATUS(static, computed_remaps, list, NIL, gen_free_list)

/* exported interface 
 */

bool 
remapping_already_computed_p(renaming x)
{
    entity src = renaming_old(x), trg = renaming_new(x);

    MAP(REMAPPING, r, 
    {
	renaming p = remapping_renaming(r);
	if (renaming_old(p)==src && renaming_new(p)==trg) 
	    return TRUE;
    },
	get_computed_remaps());

    return FALSE;
}

void
add_remapping_as_computed(
    renaming r,                /* old -> new remapping */
    list /* of entity */ vars) /* variables to be declared */
{
    computed_remaps_object = CONS(REMAPPING, 
        make_remapping(copy_renaming(r), gen_copy_seq(vars)),
			   computed_remaps_object);
}

/***************************************************** ENTITIES IN INCLUDES */

GENERIC_STATIC_STATUS(static, include_entities, list, NIL, gen_free_list)

void
add_remapping_as_used(renaming x)
{
    entity src = renaming_old(x), trg = renaming_new(x);
    remapping p = remapping_undefined;

    MAP(REMAPPING, r, 
    {
	renaming y = remapping_renaming(r);

	if (renaming_old(y)==src && renaming_new(y)==trg) 
	{
	    p = r;
	    break;
	}
    },
	get_computed_remaps());

    pips_assert("defined remapping", !remapping_undefined_p(p));

    MAP(ENTITY, e, 
	include_entities_object = gen_once(e, include_entities_object), 
	remapping_referenced(p));
}

/*********************************************** COMPILER STATUS MANAGEMENT */

/* ??? some memory leaks in the hpfc_status management...
 */

static hpfc_status chs = (hpfc_status) NULL; /* current hpfc status */

/* initialization of data that belongs to the hpf compiler status
 */
static void init_hpfc_status()
{
    init_entity_status();
    init_data_status();
    init_hpf_number_status();
    init_overlap_status();
    init_the_commons();
    init_dynamic_status();
    init_the_pures();
    init_the_ios();
    init_computed_remaps();
    init_hpf_reductions();

    chs = make_hpfc_status(get_overlap_status(),
			   get_data_status(),
			   get_hpf_number_status(),
			   get_entity_status(),
			   get_the_commons(),
			   get_dynamic_status(),
			   get_the_pures(),
			   get_the_ios(),
			   get_computed_remaps(),
			   get_hpf_reductions());
}

static void reset_hpfc_status()
{
    reset_entity_status();
    reset_data_status();
    reset_hpf_number_status();
    reset_overlap_status();
    reset_the_commons();
    reset_dynamic_status();
    reset_the_pures();
    reset_the_ios();
    reset_computed_remaps();
    reset_hpf_reductions();

    chs = (hpfc_status) NULL;
}

static void save_hpfc_status() /* GET them */
{
    pips_assert("some current hpfc status", chs);
    
    hpfc_status_entity_status(chs) = get_entity_status();
    hpfc_status_overlapsmap(chs) = get_overlap_status();
    hpfc_status_data_status(chs) = get_data_status();
    hpfc_status_numbers_status(chs) = get_hpf_number_status();
    hpfc_status_commons(chs) = get_the_commons() ;
    hpfc_status_dynamic_status(chs) = get_dynamic_status();
    hpfc_status_pures(chs) = get_the_pures();
    hpfc_status_ios(chs) = get_the_ios();
    hpfc_status_computed(chs) = get_computed_remaps();
    hpfc_status_reductions(chs) = get_hpf_reductions();

    DB_PUT_MEMORY_RESOURCE(DBR_HPFC_STATUS, "", chs);

    reset_hpfc_status(); /* cleaned! */
}

static void load_hpfc_status() /* SET them */
{
    chs = (hpfc_status) db_get_memory_resource(DBR_HPFC_STATUS, "", TRUE);

    set_entity_status(hpfc_status_entity_status(chs));
    set_overlap_status(hpfc_status_overlapsmap(chs));
    set_data_status(hpfc_status_data_status(chs));
    set_hpf_number_status(hpfc_status_numbers_status(chs));
    set_the_commons(hpfc_status_commons(chs));
    set_dynamic_status(hpfc_status_dynamic_status(chs));
    set_the_pures(hpfc_status_pures(chs));
    set_the_ios(hpfc_status_ios(chs));
    set_computed_remaps(hpfc_status_computed(chs));
    set_hpf_reductions(hpfc_status_reductions(chs));
}

/* never called... memory to be freed by pipsdbm...
 */
static void close_hpfc_status()
{
    int i = (int) close_hpfc_status ; i = i & i ; /* just to avoid a warning */

    close_entity_status();
    close_data_status();
    close_hpf_number_status();
    close_overlap_status();
    close_the_commons();
    close_dynamic_status();
    close_the_pures();
    close_the_ios();
    close_computed_remaps();
    close_hpf_reductions();
}

/************************************************************** COMPILATION */

static void automatic_translation(entity old, entity host, entity node)
{
    store_new_host_variable(host, old);
    store_new_node_variable(node, old);
}

static void set_resources_for_module(entity module)
{
    string module_name = strdup(module_local_name(module));

    /*   STATEMENT
     */
    set_current_module_statement
	((statement) db_get_memory_resource(DBR_CODE, module_name, FALSE));

    /*   PRECONDITIONS
     */
    set_precondition_map
	((statement_mapping)
	 db_get_memory_resource(DBR_PRECONDITIONS, module_name, TRUE));

    /*   POSTCONDITIONS
     */
    set_postcondition_map
	(compute_postcondition(get_current_module_statement(),
			       MAKE_STATEMENT_MAPPING(),
			       get_precondition_map()));

    /*   REGIONS
     */
    set_local_regions_map
	(effectsmap_to_listmap((statement_mapping)
	 db_get_memory_resource(DBR_REGIONS, module_name, TRUE)));
    
    /*   ONLY I/O
     */
    only_io_mapping_initialize(get_current_module_statement());
    
    /*   NEW VARIABLES NEEDED
     */
    reset_unique_variable_numbers();

    /*   OTHERS
     */
    make_hpfc_current_mappings();
    init_include_entities();

    /*  next in hpfc_init ???
     */
    hpfc_init_run_time_entities();

    /*   STOP is to be translated into hpfc_{host,node}_end.
     *   the special FCD calls.
     */
    automatic_translation(local_name_to_top_level_entity(STOP_FUNCTION_NAME),
			  hpfc_name_to_entity(HOST_END),
			  hpfc_name_to_entity(NODE_END));
    automatic_translation(hpfc_name_to_entity(HPF_PREFIX SYNCHRO_SUFFIX),
			  hpfc_name_to_entity(SYNCHRO),
			  hpfc_name_to_entity(SYNCHRO));
    automatic_translation(hpfc_name_to_entity(HPF_PREFIX TIMEON_SUFFIX),
			  hpfc_name_to_entity(HOST_TIMEON),
			  hpfc_name_to_entity(NODE_TIMEON));
    automatic_translation(hpfc_name_to_entity(HPF_PREFIX TIMEOFF_SUFFIX),
			  hpfc_name_to_entity(HOST_TIMEOFF),
			  hpfc_name_to_entity(NODE_TIMEOFF));
    automatic_translation(hpfc_name_to_entity(HPF_PREFIX HOSTSECTION_SUFFIX),
			  entity_intrinsic(CONTINUE_FUNCTION_NAME),
			  entity_intrinsic(CONTINUE_FUNCTION_NAME));

    free(module_name);
}

static void 
reset_resources_for_module()
{
    reset_current_module_statement();
    reset_local_regions_map();
    reset_precondition_map();

    free_only_io_map();
    free_postcondition_map();

    free_hpfc_current_mappings();
    close_include_entities();
}

static void 
compile_module(entity module)
{
    statement s, host_stat, node_stat;

    /*   INIT
     */
    set_resources_for_module(module);
    s = get_current_module_statement();
    make_host_and_node_modules(module);
    hpfc_init_dummy_to_prime();

    /*   NORMALIZATIONS
     */
    NormalizeHpfDeclarations();
    NormalizeCodeForHpfc(s);

    /* here because the module was updated with some external declarations
     */
    init_host_and_node_entities(); 

    /*   ACTUAL COMPILATION
     */
    hpf_compiler(s, &host_stat, &node_stat);

    if (entity_main_module_p(module))
	add_pvm_init_and_end(&host_stat, &node_stat);

    declaration_with_overlaps_for_module(module);

    update_object_for_module(node_stat, node_module);
    update_object_for_module(entity_code(node_module), node_module);
    insure_declaration_coherency(node_module, node_stat,
				 get_include_entities());
    kill_statement_number_and_ordering(node_stat);
    statement_structural_cleaning(node_stat);
    
    update_object_for_module(host_stat, host_module);
    update_object_for_module(entity_code(host_module), host_module);
    insure_declaration_coherency(host_module, host_stat, NIL);
    kill_statement_number_and_ordering(host_stat);
    statement_structural_cleaning(host_stat);

    /*   PUT IN DB
     */
    put_generated_resources_for_module(s, host_stat, node_stat);

    /*   CLOSE
     */
    hpfc_close_dummy_to_prime();
    reset_resources_for_module();
}

/********************************************* FUNCTIONS CALLED BY PIPSMAKE */

/* bool hpfc_init(string name)
 *
 * what: initialize the hpfc status for a program.
 * input: the program (workspace) name.
 * output: none.
 * side effects:
 *  - the hpfc status is initialized and stored in the pips dbm.
 * bugs or features:
 *  - some callees are filtered out with a property, to deal with pipsmake.
 */
bool hpfc_init(string name)
{
    debug_on("HPFC_DEBUG_LEVEL");
    pips_debug(1, "considering program %s\n", name);

    set_bool_property("PRETTYPRINT_HPFC", TRUE);
    set_bool_property("HPFC_FILTER_CALLEES", TRUE); /* drop hpfc specials */
    set_bool_property("GLOBAL_EFFECTS_TRANSLATION", FALSE);
    set_bool_property("WARNING_ON_STAT_ERROR", FALSE); /* for my fake files */
    set_bool_property("PRETTYPRINT_IO_EFFECTS", FALSE); /* no LUNS(*) ! */

    /* where the specials dummy/variables are stored... ??? */
    (void) make_empty_program(HPFC_PACKAGE);

    init_hpfc_status();
    save_hpfc_status();

    debug_off();
    return TRUE;
}

/* bool hpfc_filter(string name)
 *
 * what: filter the source code for module name. to be called by pipsmake.
 * how: call to a shell script, "hpfc_directives", that transforms 
 *      hpf directives in "special" subroutine calls to be parsed by 
 *      the fortran 77 parser.
 * input: the module name.
 * output: none.
 * side effects:
 *  - a new source code file is created for module name.
 *  - the old one is saved...
 * bugs or features:
 *  - ??? not all hpf syntaxes are managable this way.
 */
#define HPFC_FILTERED_SUFFIX ".hpfc_filtered"

bool hpfc_filter(string name)
{
    string file_name, dir_name, new_name;

    dir_name = db_get_current_workspace_directory();
    file_name = db_get_file_resource(DBR_SOURCE_FILE, name, TRUE);
    new_name = strdup(concatenate(name, HPFC_FILTERED_SUFFIX, NULL));

    debug_on("HPFC_DEBUG_LEVEL");
    pips_debug(1, "considering module %s\n", name);

    safe_system(concatenate(hpf_directive_string_p(name) ?
                            "cat" : "$HPFC_TOOLS/hpfc_directives", " < ",
			    dir_name, "/", file_name, " > ",
			    dir_name, "/", new_name, NULL));

    DB_PUT_FILE_RESOURCE(DBR_HPFC_FILTERED_FILE, name, new_name);

    debug_off();
    return TRUE;
}

/* what: deals with directives. to be called by pipsmake.
 * input: the name of the module.
 * output: none.
 * side effects: (many)
 *  - the module's code statement will be modified.
 *  - the hpf mappings and so are stored in the compiler status.
 * bugs or features:
 *  - fortran library, reduction and hpfc special functions are skipped.
 *  - ??? obscure problem with the update of common entities.
 */
static bool hpfc_directives_handler(string name, bool dyn)
{
    entity module = local_name_to_top_level_entity(name);

    debug_on("HPFC_DEBUG_LEVEL");
    pips_debug(1, "considering module %s\n", name);
    debug_on("HPFC_DIRECTIVES_DEBUG_LEVEL");

    if (!hpfc_entity_reduction_p(module) &&
	!hpf_directive_entity_p(module) &&
	!fortran_library_entity_p(module))
    {
	statement s;
	s = (statement) db_get_memory_resource(DBR_CODE, name, TRUE);

	if (dyn)
	set_proper_effects_map(effectsmap_to_listmap((statement_mapping) 
            db_get_memory_resource(DBR_PROPER_EFFECTS, name, TRUE)));

	set_current_module_entity(module);
	set_current_module_statement(s);
	load_hpfc_status();
	make_update_common_map(); 
	hpfc_init_run_time_entities();

	if (!dyn)
	    NormalizeCommonVariables(module, s);
	handle_hpf_directives(s, dyn); /* do the job... */

	free_update_common_map(); 
	save_hpfc_status();
	reset_current_module_statement();
	reset_current_module_entity();

	if (dyn)
	free_proper_effects_map();
	
	DB_PUT_MEMORY_RESOURCE(DBR_CODE, name, s);

	/* ??? should not be necessary, some bug in pipsmake
	 */
	if (dyn)
	    db_unput_a_resource(DBR_PROPER_EFFECTS, name);
    }

    debug_off(); 
    debug_off();
    return TRUE;
}

bool hpfc_directives(string name)
{
    hpfc_static_directives(name);
    hpfc_dynamic_directives(name);
    return TRUE;
}

bool hpfc_static_directives(string name)
{
    hpfc_directives_handler(name, FALSE);
    return TRUE;
}

bool hpfc_dynamic_directives(string name)
{
    hpfc_directives_handler(name, TRUE);
    return TRUE;
}


/* bool hpfc_compile(string name)
 *
 * what: hpf compile module name. to be called by pipsmake.
 * input: the name of the module to compile.
 * output: none
 * side effects: (many)
 *  - creates the statements for the host and nodes.
 *  - store the generated resources.
 * bugs or features:
 *  - fortran library, reduction and hpfc special functions are skipped.
 *  - a fake file is put as the generated resource for such modules.
 */
bool hpfc_compile(string name)
{
    entity module = local_name_to_top_level_entity(name);
    bool do_compile = 
	!hpfc_entity_reduction_p(module) &&
	!hpf_directive_entity_p(module) &&
	!fortran_library_entity_p(module);

    debug_on("HPFC_DEBUG_LEVEL");
    pips_debug(1, "considering module %s\n", name);

    if (do_compile)
    {
	load_hpfc_status();
	set_current_module_entity(module);

	set_bool_property("PRETTYPRINT_COMMONS", FALSE); 

	if (hpfc_special_io(module))
	    compile_a_special_io_function(module);
	else if (hpf_pure_p(module))
	    compile_a_pure_function(module);
	else
	    compile_module(module);

	reset_current_module_entity();
	save_hpfc_status();
    }
    else /* just fake for pipsmake... */
    {
	DB_PUT_FILE_RESOURCE(DBR_HPFC_PARAMETERS, name, NO_FILE);
	DB_PUT_FILE_RESOURCE(DBR_HPFC_HOST, name, NO_FILE);
	DB_PUT_FILE_RESOURCE(DBR_HPFC_NODE, name, NO_FILE);
	DB_PUT_FILE_RESOURCE(DBR_HPFC_RTINIT, name, NO_FILE);
    }

    debug_off();
    return TRUE;
}

/* bool hpfc_common(string name)
 *
 * what: compile a common, that is generate the common for both host and nodes.
 * how: generate files to be included.
 * input: a common name.
 * output: none.
 * side effects:
 *  - uses the hpfc_status.
 *  - generates 4 files.
 * bugs or features:
 *  - never called by pipsmake (:-)
 */
bool hpfc_common(string name)
{
    debug_on("HPFC_DEBUG_LEVEL");
    pips_debug(1, "considering common %s\n", name);

    load_hpfc_status();

    set_bool_property("PRETTYPRINT_COMMONS", TRUE);
    compile_common(local_name_to_top_level_entity(name));

    save_hpfc_status();

    debug_off();
    return TRUE;
}

/* bool hpfc_close(string name)
 *
 * what: closes the hpf compiler execution. to be called by pipsmake.
 * input: the program (workspace) name.
 * output: none.
 * side effects:
 *  - deals with the commons.
 *  - generates global files.
 * bugs or features:
 *  - ??? COMMON should be managable thru pipsmake ad-hoc rules.
 */
bool hpfc_close(string name)
{
    debug_on("HPFC_DEBUG_LEVEL");
    pips_debug(1, "considering program %s\n", name);
 
    load_hpfc_status();
    
    set_bool_property("PRETTYPRINT_COMMONS", TRUE); /* commons compilation */
    gen_map(compile_common, get_the_commons());

    put_generated_resources_for_program(name);      /* global informations */

    /* not close, because it would free the memory and 
     * pipsdbm will run into troubles when trying to free the resource...
     */
    save_hpfc_status();
    
    DB_PUT_FILE_RESOURCE(DBR_HPFC_COMMONS, name, NO_FILE); /* fake */

    debug_off();
    return TRUE;
}

/* bool hpfc_install(string name)
 *
 * what: install generated files in a directory. done for wpips.
 * how: all in the hpfc_install shell script.
 * input: program name.
 * output: none.
 * side effects:
 *  - creates an hpfc directory in the database
 *  - copies the files in this directory...
 * bugs or features:
 */
bool hpfc_install(string name)
{
    string dir, wks;

    debug_on("HPFC_DEBUG_LEVEL");
    pips_debug(1, "considering program %s\n", name);

    dir = db_get_current_workspace_directory();
    wks = db_get_current_workspace_name();

    safe_system(concatenate("$HPFC_TOOLS/hpfc_install -iob ", dir, 
			    " -n ", wks, NULL));

    DB_PUT_FILE_RESOURCE(DBR_HPFC_INSTALLATION, name, NO_FILE);

    debug_off();
    return TRUE;
}

/* bool hpfc_make(string name)
 *
 * what: compile the generated and installed code. for wpips.
 * how: system call to $HPFC_MAKE
 * input: the workspace name (which is not used)
 * output: none.
 * side effects: may stop if can't execute
 * bugs or features:
 */
bool hpfc_make(string name)
{
    string dir = db_get_current_workspace_directory();
    
    debug_on("HPFC_DEBUG_LEVEL");
    pips_debug(1, "considering program %s\n", name);

    safe_system(concatenate("cd ", dir, "/hpfc ; $HPFC_MAKE make &", NULL));

    debug_off();
    return TRUE;
}

/* bool hpfc_run(string name)
 *
 * what: run the program.
 * how: system call sut $HPFC_MAKE...
 * input: none.
 * output: none.
 * side effects:
 *  - forks a process.
 *  - may stop if cannot execute.
 * bugs or features:
 */
bool hpfc_run(string name)
{
    string dir = db_get_current_workspace_directory();

    debug_on("HPFC_DEBUG_LEVEL");
    pips_debug(1, "considering program %s\n", name);

    safe_system(concatenate("cd ", dir, "/hpfc ; $HPFC_MAKE run &", NULL));

    debug_off();
    return TRUE;
}

/*   that is all
 */
