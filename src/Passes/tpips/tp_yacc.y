
%token SEPARATOR
%token OPEN
%token CREATE
%token CLOSE
%token DELETE
%token MODULE
%token MAKE
%token APPLY
%token DISPLAY
%token ACTIVATE
%token SET
%token GET
%token INFO
%token OWNER_NAME
%token OWNER_ALL
%token OWNER_PROGRAM
%token OWNER_MAIN
%token OWNER_MODULE
%token OWNER_CALLERS
%token OWNER_CALLEES
%token MNAME
%token WORKSPACE
%token OPENPAREN
%token COMMA
%token CLOSEPAREN
%token PROPNAME
%token FILE_NAME
%token RESOURCENAME
%token PHASENAME
%token UNKNOWN_CHAR
%token SETVALUE

%type <status> line
%type <status> instruction
%type <status> i_open
%type <status> i_create
%type <status> i_close
%type <status> i_delete
%type <status> i_module
%type <status> i_make
%type <status> i_apply
%type <status> i_activate
%type <status> i_display
%type <status> i_set
%type <status> i_get
%type <name> rulename
%type <name> filename
%type <args> filename_list
%type <rn> resource_id
%type <rn> rule_id
%type <owner> owner
%type <owner> list_of_owner_name
%type <status> sep_list
%type <status> opt_sep_list

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "genC.h"

#include "ri.h"
#include "database.h"
#include "makefile.h"

#include "misc.h"

#include "pipsdbm.h"
#include "ri-util.h"
#include "resources.h"
#include "phases.h"
#include "builder_map.h"
#include "properties.h"
#include "pipsmake.h"

#include "list.h"
#include "ri-util.h"
#include "top-level.h"
#include "tpips.h"

extern char yytext[];
extern FILE * yyin; 

/* Default comand to print a file (if $PAGER is not set) */
#define CAT_COMMAND "cat"

/********************************************************** static functions */
static void print_property(char*,property);

/********************************************************** static variables */
static t_file_list the_file_list;
static execution_mode = TRUE;

%}

%union {
	int status;
	string name;
	res_or_rule rn;
	list owner;
	t_file_list *args;
}

%%

line:
	instruction
	{ $$ = $1; return $1;}
    ;

instruction:
	  i_open
	| i_create
  	| i_close
	| i_delete
	| i_module
	| i_make
	| i_apply
	| i_display
	| i_activate
	| i_set
	| i_get
	| error {$$ = FALSE;}
	;

i_open:
	OPEN
	sep_list
	WORKSPACE /* workspace name */
	opt_sep_list
	{
	    string main_module_name;

	    debug(7,"tp_parse","reduce rule i_open\n");

	    if (execution_mode) {
		if (db_get_current_program_name() != NULL)
		    close_program ();
		if (( $$ = open_program (yylval.name)))
		{
		    main_module_name = get_first_main_module();
		    
		    if (!string_undefined_p(main_module_name)) {
			/* Ok, we got it ! Now we select it: */
			user_log("Main module PROGRAM \"%s\" found.\n",
				 main_module_name);
			lazy_open_module(main_module_name);
		    }
		}
	    }
/*
	    free (yylval.name);
*/
	}
	;

i_create:
	CREATE
	sep_list
	WORKSPACE /* workspace name */
	{
	    $$ = (int) yylval.name;
	    the_file_list.argc = 0;
	}
	sep_list
	filename_list /* list of fortran files */
	{
	    string main_module_name;
	    
	    debug(7,"tp_parse","reduce rule i_create\n");
	    
	    if (execution_mode) {
		if (workspace_exists_p($<name>4))
		    user_error ("tp_parse",
				"the workspace %s exists\n",
				$<name>4);
		else {
		    db_create_workspace ((string) $<name>4);
		    free($<name>4);
		    create_program (&the_file_list.argc, the_file_list.argv);
		    main_module_name = get_first_main_module();
		    
		    if (!string_undefined_p(main_module_name)) {
			/* Ok, we got it ! Now we select it: */
			user_log("Main module PROGRAM \"%s\" found.\n",
				 main_module_name);
			lazy_open_module(main_module_name);
		    }
		    $$ = TRUE;
		}
	    }
/*
	    while (the_file_list.argc--)
		free (the_file_list.argv[the_file_list.argc]);
	    free ($<name>4);
*/
	}
	;

i_close:
	CLOSE
	opt_sep_list
	{
	    debug(7,"tp_parse","reduce rule i_close\n");

	    if (execution_mode) {
		if (db_get_current_program_name() != NULL) {
		    close_program ();
		    $$ = TRUE;
		} else {
		    user_warning ("tp_parse","No workspace to close\n");
		    $$ = FALSE;
		}	
		$$ = TRUE;
	    }
	}
	;

i_delete:
	DELETE
	sep_list
	WORKSPACE /* workspace name */
	opt_sep_list
	{
	    char *t = yylval.name;

	    debug(7,"tp_parse","reduce rule i_delete\n");

	    if (execution_mode) {
		if (db_get_current_program_name() == NULL) {
		    delete_program (t);
		    user_log ("Workspace %s deleted.\n", t);
		    $$ = TRUE;
		} else {
		    user_warning ("tp_parse","Workspace opened\n");
		    $$ = FALSE;
		}
	    }
/*
	    free(t);
*/
	}
	;

i_module:
	MODULE
	sep_list
	WORKSPACE /* module name */
	opt_sep_list
	{
	    char *t = yylval.name;

	    debug(7,"tp_parse","reduce rule i_module\n");

	    if (execution_mode) {
		debug(7,"tp_parse","reduce rule i_module\n");
		lazy_open_module (strupper(t,t));
		$$ = TRUE;
	    }
/*
	    free(t);
*/
	}
	;

i_make:
	MAKE
	sep_list
	resource_id
	opt_sep_list
	{
	    debug(7,"tp_parse","reduce rule i_make\n");

	    if (execution_mode) {
		MAPL(e, {
		    if (safe_make_p ($3.the_name, STRING(CAR(e))) == FALSE) {
			$$ = FALSE;
			break;
		    }
		}, $3.the_owners);
		
		$$ = TRUE;
	    }
/*	    free ($3.the_name);
	    gen_free_list ($3.the_owners);
*/
	}
	;

i_apply:
	APPLY
	sep_list
	rule_id
	opt_sep_list
	{
	    debug(7,"tp_parse","reduce rule i_apply\n");

	    if (execution_mode) {
		MAPL(e, {
		    (void) safe_apply ($3.the_name, STRING(CAR(e)));
		}, $3.the_owners);
		
		$$ = TRUE;
	    }
/*	    free ($3.the_name);
	    gen_free_list ($3.the_owners);
*/
	}
	;

i_display:
	DISPLAY
	sep_list
	resource_id
	opt_sep_list
	{
	    debug(7,"tp_parse","reduce rule i_display\n");

	    if (execution_mode) {
		string pager;

		if (!(pager = getenv("PAGER")))
		    pager = CAT_COMMAND;
		
		MAPL(e, {
		    lazy_open_module (STRING(CAR(e)));
		    system(concatenate(pager,
				       " ",
				       build_view_file($3.the_name),
				       NULL));
		}, $3.the_owners);
		$$ = TRUE;
	    }
/*
	    free ($3.the_name);
	    gen_free_list ($3.the_owners);
*/
	}
	;

i_activate:
	ACTIVATE
	sep_list
	rulename
	opt_sep_list
	{
	    debug(7,"tp_parse","reduce rule i_activate\n");

	    if (execution_mode) {
		activate ($3);
		$$ = TRUE;
	    }
	}
	;

i_set:
	SET
	sep_list
	PROPNAME
	{ $$ = (list) yylval.name; /* bof */}
	sep_list
	SETVALUE
	opt_sep_list
	{
	    property p ;
	    bool status = TRUE;
	    
	    debug(7,"tp_parse","reduce rule i_set(%s,%s)\n",
		  $<name>4,yylval.name);

	    if (execution_mode) {
	    
		p = get_property ($<name>4);
		strupper (yylval.name,yylval.name);
		
		switch (property_tag(p))
		{
		case is_property_bool:
		{
		    if (!strcmp ("TRUE",yylval.name))
			set_bool_property ($<name>4, TRUE);
		    else if (!strcmp ("FALSE",yylval.name))
			set_bool_property ($<name>4, FALSE);
		    else {
			yyerror ("type mismatch");
			status = FALSE;
		    }
		    break;
		}
		case is_property_int:
		{
		    char **ptr;
		    long l;
		    
		    l = strtol (yylval.name, ptr, 0);
		    if (**ptr != '\0') {
			yyerror ("type mismatch");
			status = FALSE;
		    } else
			set_int_property($<name>4, (int) l);
		    break;
		}
		case is_property_string:
		{
		    char *q = strrchr(yylval.name, '"');
		    if (!q)
			pips_error("yyparse",
				   "Did not find a quote in the string\n");
		    *q = '\0';
		    set_string_property($<name>4, yylval.name + 1);
		    break;
		}
		}
		if (status)
		{
		    print_property($<name>4,p);
		    $$ = TRUE;
		} else
		    $$ = FALSE;
	    }
/*
	    free (yylval.name);
	    free ($<name>4);
*/
	}
	;

i_get:
	GET
	sep_list
	PROPNAME
	opt_sep_list
	{
	    property p;

	    debug(7,"tp_parse","reduce rule i_get (%s)\n",
		  yylval.name);
	    
	    if (execution_mode) {
		p = get_property (yylval.name);
	    
		print_property(yylval.name, p);
		$$ = TRUE;
	    }
/*
	    free (yylval.name);
*/
	}
	;

rulename:
	PHASENAME
	{
	    debug(7,"tp_parse","reduce rule rulename (%s)\n",yylval.name);
	    $$ = yylval.name;
	}
	;

filename_list:
	filename
	sep_list
	filename_list
	{
	    debug(7,"tp_parse","reduce rule filename_list (%s)\n", $1);

	    if (the_file_list.argc < FILE_LIST_MAX_LENGTH) {
		the_file_list.argv[the_file_list.argc] = $1;
		the_file_list.argc++;
	    } else
		YYERROR;
	    
	}
	|
	filename
	opt_sep_list
	{
	    debug(7,"tp_parse","reduce rule filename_list (%s)\n", $1);

	    if (the_file_list.argc < FILE_LIST_MAX_LENGTH) {
		the_file_list.argv[the_file_list.argc] = $1;
		the_file_list.argc++;
	    } else
		YYERROR;
	}
	;

filename:
	FILE_NAME
	{ $$ = yylval.name;}
	;

resource_id:
	RESOURCENAME
	{ $<name>$ = yylval.name;}
	owner
	{
	    debug(7,"tp_parse","reduce rule resource_id (%s)\n",$<name>2);

	    $$.the_name = $<name>2;
	    $$.the_owners = $3;
	}
	;

rule_id:
	PHASENAME
	{ $<name>$ = yylval.name;}
	owner
	{
	    debug(7,"tp_parse","reduce rule rule_id (%s)\n",$<name>2);

	    $$.the_name = $<name>2;
	    $$.the_owners = $3;
	}
	;

owner:
	OPENPAREN
	OWNER_ALL
	CLOSEPAREN
	{
	    int nmodules = 0;
	    char *module_list[ARGS_LENGTH];
	    int i;
	    list result = NIL;

	    debug(7,"tp_parse","reduce rule owner (ALL)\n");

	    if (execution_mode) {
		db_get_module_list(&nmodules, module_list);
		pips_assert("yyparse", nmodules>0);
		for(i=0; i<nmodules; i++) {
		    string on = module_list[i];

		    result = gen_nconc(result, 
				       CONS(STRING, on, NIL));
		}
		$$ = result;
	    }
	}
	|
	OPENPAREN
	OWNER_PROGRAM
	CLOSEPAREN
	{
	    debug(7,"tp_parse","reduce rule owner (PROGRAM)\n");

	    if (execution_mode) {
		$$ = CONS(STRING, db_get_current_program_name (), NIL);
	    }
	}
	|
	OPENPAREN
	OWNER_MAIN
	CLOSEPAREN
	{
	    int nmodules = 0;
	    char *module_list[ARGS_LENGTH];
	    int i;
	    int number_of_main = 0;

	    debug(7,"tp_parse","reduce rule owner (MAIN)\n");

	    if (execution_mode) {
		db_get_module_list(&nmodules, module_list);
		pips_assert("yyparse", nmodules>0);
		for(i=0; i<nmodules; i++) {
		    string on = module_list[i];

		    if (entity_main_module_p
			(local_name_to_top_level_entity(on)) == TRUE)
		    {
			if (number_of_main)
			    pips_error("build_real_resources",
				       "More the one main\n");
			
			number_of_main++;
			$$ = CONS(STRING, on, NIL);
		    }
		}
	    }
	}
	|
	OPENPAREN
	OWNER_MODULE
	CLOSEPAREN
	{
	    debug(7,"tp_parse","reduce rule owner (MODULE)\n");

	    if (execution_mode) {
		$$ = CONS(STRING, db_get_current_module_name (), NIL);
	    }
	}
	|
	OPENPAREN
	OWNER_CALLEES
	CLOSEPAREN
	{
	    callees called_modules;
	    list lcallees;
	    list ps;
	    list result = NIL;

	    debug(7,"tp_parse","reduce rule owner (CALLEES)\n");

	    if (execution_mode) {
		if (safe_make_p(DBR_CALLEES, db_get_current_module_name())
		    == FALSE)
		    YYERROR;	

		called_modules = (callees) 
		db_get_memory_resource(DBR_CALLEES,
				       db_get_current_module_name(),TRUE);
		lcallees = callees_callees(called_modules);

		for (ps = lcallees; ps != NIL; ps = CDR(ps)) {
		    string on = STRING(CAR(ps));
		    result = gen_nconc(result, CONS(STRING, on, NIL));
		}
		$$ = result;
	    }
	}
	|
	OPENPAREN
	OWNER_CALLERS
	CLOSEPAREN
	{
	    callees caller_modules;
	    list lcallers;
	    list ps;
	    list result = NIL;

	    debug(7,"tp_parse","reduce rule owner (CALLERS)\n");
	    if (execution_mode) {
		if (safe_make_p(DBR_CALLERS, db_get_current_module_name())
		    == FALSE)
		    YYERROR;	

		caller_modules = (callees) 
		    db_get_memory_resource(DBR_CALLERS,
					   db_get_current_module_name(),TRUE);
		lcallers = callees_callees(caller_modules);

		for (ps = lcallers; ps != NIL; ps = CDR(ps)) {
		    string on = STRING(CAR(ps));
		    result = gen_nconc(result, CONS(STRING, on, NIL));
		}
		$$ = result;
	    }
	}
	|
	OPENPAREN
	list_of_owner_name
	{ $$ = $2; }
	|
	{
	    debug(7,"tp_parse","reduce rule owner (none)\n");

	    if (execution_mode) {
			$$ = CONS(STRING, db_get_current_module_name (), NIL);
	    }
	}
	;

list_of_owner_name:
	OWNER_NAME
	{ $$ = (list) yylval.name; }
	COMMA
	list_of_owner_name
	{
	    debug(7,"tp_parse",
		  "reduce rule owner list (name = %s)\n",$<name>2);

	    if (execution_mode) {
		char *c = $<name>2;
		strupper (c, c);
		$$ = gen_nconc($4,CONS(STRING, c, NIL));
	    }
	}
	|
	OWNER_NAME
	{ $$ = (list) yylval.name; }
	CLOSEPAREN
	{
	    debug(7,"tp_parse","reduce rule owner list(name = %s)\n",$<name>2);

	    if (execution_mode) {
		char *c = $<name>2;
		strupper (c, c);
		$$ = CONS(STRING, c, NIL);
	    }
	}
	;

opt_sep_list:
	sep_list
	{$$ = 0;}
	|
	{$$ = 0;}
	;

sep_list:
	sep_list
	SEPARATOR
	{$$ = $1;}
	|
	SEPARATOR 
	{
	    debug(7,"tp_parse","reduce separator list\n");
	    $$ = 0;
	}
	;

%%

void yyerror(s)
char * s;
{
    tpips_lex_print_pos(stderr);
    user_error("[yyparse]"," %s\n", s);
}

void close_workspace_if_opened()
{
    if (db_get_current_program_name() != NULL)
	close_program ();
}

static void print_property(char* pname, property p)
{
    switch (property_tag(p))
    {
    case is_property_bool:
    {
		fprintf (stderr, "%s = %s\n",
			pname,
			get_bool_property (pname) == TRUE ?
			"TRUE" : "FALSE");
		break;
    }
    case is_property_int:
    {
		fprintf (stderr, "%s = %d\n",
			pname,
			get_int_property (pname));
		break;
    }
    case is_property_string:
    {
		fprintf (stderr, "%s = %s\n",
			pname,
			get_string_property (pname));
		break;
    }
    }
}

void init_execution_mode()
{
    execution_mode = get_bool_property("TPIPS_NO_EXECUTION_MODE") ?
	FALSE : TRUE ;
}
