/* 	%A% ($Date: 1996/11/13 13:54:58 $, ) version $Revision$, got on %D%, %T% [%P%].\n Copyright (c) �cole des Mines de Paris Proprietary.	 */

#ifndef lint
char lib_ri_util_prettyprint_c_vcid[] = "%A% ($Date: 1996/11/13 13:54:58 $, ) version $Revision$, got on %D%, %T% [%P%].\n Copyright (c) �cole des Mines de Paris Proprietary.";
#endif /* lint */
 /*
  * Prettyprint all kinds of ri related data structures
  *
  *  Modifications:
  * - In order to remove the extra parentheses, I made the several changes:
  * (1) At the intrinsic_handler, the third term is added to indicate the
  *     precendence, and accordingly words_intrinsic_precedence(obj) is built
  *     to get the precedence of the call "obj".
  * (2) words_subexpression is created to distinguish the
  *     words_expression.  It has two arguments, expression and
  *     precedence. where precedence is newly added. In case of failure
  *     of words_subexpression , that is, when
  *     syntax_call_p is FALSE, we use words_expression instead.
  * (3) When words_call is firstly called , we give it the lowest precedence,
  *        that is 0.
  *    Lei ZHOU           Nov. 4, 1991
  *
  * - Addition of CMF and CRAFT prettyprints. Only text_loop() has been
  * modified.
  *    Alexis Platonoff, Nov. 18, 1994

  * - Modifications of sentence_area to deal with  the fact that
  *   " only one appearance of a symbolic name as an array name in an 
  *     array declarator in a program unit is permitted."
  *     (Fortran standard, number 8.1, line 40) 
  *   array declarators now only appear with the type declaration, not with the
  *   area. - BC - october 196.
  * - Modification of text_entity_declaration to ensure that the OUTPUT of PIPS
  *   can also be used as INPUT; in particular, variable declarations must appear
  *   before common declarations. BC.
  * - Also, EQUIVALENCE statements are not generated for the moment. BC.
  */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "genC.h"
#include "text.h"
#include "text-util.h"
#include "ri.h"
#include "ri-util.h"

#include "misc.h"
#include "properties.h"

#include "control.h"

/* Define the markers used in the raw unstructured output when the
   PRETTYPRINT_UNSTRUCTURED_AS_A_GRAPH property is true: */
#define PRETTYPRINT_UNSTRUCTURED_BEGIN_MARKER "\200Unstructured"
#define PRETTYPRINT_UNSTRUCTURED_END_MARKER "\201Unstructured End"
#define PRETTYPRINT_UNSTRUCTURED_ITEM_MARKER "\202Unstructured Item"
#define PRETTYPRINT_UNSTRUCTURED_SUCCESSOR_MARKER "\203Unstructured Successor ->"
#define PRETTYPRINT_UNREACHABLE_EXIT_MARKER "\204Unstructured Unreachable"


text empty_text(entity e, int m, statement s)
{ return( make_text(NIL));}

static text (*text_statement_hook)(entity, int, statement) = empty_text;

void init_prettyprint( hook )
text (*hook)(entity, int, statement) ;
{
    /* checks that the prettyprint hook was actually reset...
     */
    pips_assert("prettyprint hook not set", text_statement_hook==empty_text);
    text_statement_hook = hook ;
}

/* because some prettyprint functions may be used for debug, so
 * the last hook set by somebody may have stayed there although
 * being non sense...
 */
void close_prettyprint()
{
    text_statement_hook = empty_text;
}

/* We have no way to distinguish between the SUBROUTINE and PROGRAM
 * They two have almost the same properties.
 * For the time being, especially for the PUMA project, we have a temporary
 * idea to deal with it: When there's no argument(s), it should be a PROGRAM,
 * otherwise, it should be a SUBROUTINE. 
 * Lei ZHOU 18/10/91
 *
 * correct PROGRAM and SUBROUTINE distinction added, FC 18/08/94
 */
sentence sentence_head(e)
entity e;
{
    cons *pc = NIL;
    type te = entity_type(e);
    functional fe;
    type tr;
    cons *args = words_parameters(e);

    pips_assert("sentence_head", type_functional_p(te));

    fe = type_functional(te);
    tr = functional_result(fe);

    
    if (type_void_p(tr)) 
    {
	/* the condition was ENDP(args) */
	pc = CHAIN_SWORD(pc, entity_main_module_p(e) ? 
			 "PROGRAM " : "SUBROUTINE ");
    }
    else if (type_variable_p(tr)) {
	pc = gen_nconc(pc, words_basic(variable_basic(type_variable(tr))));
	pc = CHAIN_SWORD(pc, " FUNCTION ");
    }
    else {
	pips_error("sentence_head", "unexpected type for result\n");
    }
    pc = CHAIN_SWORD(pc, module_local_name(e));

    if ( !ENDP(args) ) {
	pc = CHAIN_SWORD(pc, "(");
	pc = gen_nconc(pc, args);
	pc = CHAIN_SWORD(pc, ")");
    }
    return(make_sentence(is_sentence_unformatted, 
			 make_unformatted(NULL, 0, 0, pc)));
}

sentence sentence_tail()
{
    return(MAKE_ONE_WORD_SENTENCE(0, "END"));
}

sentence sentence_variable(e)
entity e;
{
    cons *pc = NIL;
    type te = entity_type(e);

    pips_assert("sentence_variable", type_variable_p(te));

    pc = gen_nconc(pc, words_basic(variable_basic(type_variable(te))));
    pc = CHAIN_SWORD(pc, " ");

    pc = gen_nconc(pc, words_declaration(e, TRUE));

    return(make_sentence(is_sentence_unformatted, 
			 make_unformatted(NULL, 0, 0, pc)));
}

/*  special management of empty commons added.
 *  this may happen in the hpfc generated code.
 */
sentence sentence_area(e, module)
entity e, module;
{
    string area_name = entity_local_name(e);
    type te = entity_type(e);
    bool prettyprint_hpfc = get_bool_property("PRETTYPRINT_HPFC");
    list 
	pc = NIL,
	entities = NIL;

    if (dynamic_area_p(e))
	return(make_sentence(is_sentence_formatted, ""));

    assert(type_area_p(te));

    if (!ENDP(area_layout(type_area(te))))
    {
	MAP(ENTITY, ee,
	 {
	     if (local_entity_of_module_p(ee, module) || prettyprint_hpfc)
		 entities = CONS(ENTITY, ee, entities);
	 },
	     area_layout(type_area(te)));	     

	/*  the common is not output if it is empty
	 */
	if (!ENDP(entities))
	{
	    bool comma = FALSE;

	    if (static_area_p(e))
	    {
		pc = CHAIN_SWORD(pc, "SAVE ");
	    }
	    else
	    {
		pc = CHAIN_SWORD(pc, "COMMON ");
		if (strcmp(area_name, BLANK_COMMON_LOCAL_NAME) != 0) 
		{
		    pc = CHAIN_SWORD(pc, "/");
		    pc = CHAIN_SWORD(pc, area_name);
		    pc = CHAIN_SWORD(pc, "/ ");
		}
	    }
	    entities = gen_nreverse(entities);
	    
	    MAP(ENTITY, ee, 
	     {
		 if (comma) pc = CHAIN_SWORD(pc, ",");
		 else comma = TRUE;
		 pc = gen_nconc(pc, words_declaration(ee, FALSE));
	     },
		 entities);

	    gen_free_list(entities);
	}
    }

    return(make_sentence(is_sentence_unformatted, 
			 make_unformatted(NULL, 0, 0, pc)));
}

sentence sentence_goto(module, margin, obj)
entity module;
int margin;
statement obj;
{
    string label = entity_local_name(statement_label(obj)) + 
	           strlen(LABEL_PREFIX);

    return( sentence_goto_label(module, margin, label)) ;
}

sentence sentence_goto_label(module, margin, label)
entity module;
int margin;
string label;
{
    cons *pc = NIL;

    if (strcmp(label, RETURN_LABEL_NAME) == 0) {
	pc = CHAIN_SWORD(pc, RETURN_FUNCTION_NAME);
    }
    else {
	pc = CHAIN_SWORD(pc, "GOTO ");
	pc = CHAIN_SWORD(pc, label);
    }

    return(make_sentence(is_sentence_unformatted, 
			 make_unformatted(NULL, 0, margin, pc)));
}

sentence sentence_basic_declaration(e)
entity e;
{
    list decl = NIL;
    basic b = entity_basic(e);

    pips_assert("sentence_basic_declaration", !basic_undefined_p(b));

    decl = CHAIN_SWORD(decl, basic_to_string(b));
    decl = CHAIN_SWORD(decl, " ");
    decl = CHAIN_SWORD(decl, entity_local_name(e));

    return(make_sentence(is_sentence_unformatted, 
			 make_unformatted(NULL, 0, 0, decl)));
}

sentence sentence_external(f)
entity f;
{
    list pc = NIL;

    pc = CHAIN_SWORD(pc, "EXTERNAL ");
    pc = CHAIN_SWORD(pc, entity_local_name(f));

    return(make_sentence(is_sentence_unformatted, 
			 make_unformatted(NULL, 0, 0, pc)));
}

sentence sentence_symbolic(f)
entity f;
{
    cons *pc = NIL;
    value vf = entity_initial(f);
    expression e = symbolic_expression(value_symbolic(vf));

    pc = CHAIN_SWORD(pc, "PARAMETER (");
    pc = CHAIN_SWORD(pc, entity_local_name(f));
    pc = CHAIN_SWORD(pc, " = ");
    pc = gen_nconc(pc, words_expression(e));
    pc = CHAIN_SWORD(pc, ")");

    return(make_sentence(is_sentence_unformatted, 
			 make_unformatted(NULL, 0, 0, pc)));
}

sentence sentence_data(e)
entity e;
{
    cons *pc = NIL;
    constant c;

    if (! value_constant_p(entity_initial(e)))
	return(sentence_undefined);

    c = value_constant(entity_initial(e));

    if (! constant_int_p(c))
	return(sentence_undefined);

    pc = CHAIN_SWORD(pc, "DATA ");
    pc = CHAIN_SWORD(pc, entity_local_name(e));
    pc = CHAIN_SWORD(pc, " /");
    pc = CHAIN_IWORD(pc, constant_int(c));
    pc = CHAIN_SWORD(pc, "/");

    return(make_sentence(is_sentence_unformatted, 
			 make_unformatted(NULL, 0, 0, pc)));
}

#define ADD_WORD_LIST_TO_TEXT(t, l)\
  if (!ENDP(l)) ADD_SENTENCE_TO_TEXT(t,\
	        make_sentence(is_sentence_unformatted, \
			      make_unformatted(NULL, 0, 0, l)));

/* We add this function to cope with the declaration
 * When the user declare sth. there's no need to declare sth. for the user.
 * When nothing is declared ( especially there is no way to know whether it's 
 * a SUBROUTINE or PROGRAM). We will go over the entire module to find all the 
 * variables and declare them properly.
 * Lei ZHOU 18/10/91
 *
 * the float length is now tested to generate REAL*4 or REAL*8.
 * ??? something better could be done, printing "TYPE*%d".
 * the problem is that you cannot mix REAL*4 and REAL*8 in the same program
 * Fabien Coelho 12/08/93 and 15/09/93
 *
 * pf4 and pf8 distinction added, FC 26/10/93
 *
 * Is it really a good idea to print overloaded type variables~? FC 15/09/93
 * PARAMETERS added. FC 15/09/93
 *
 * typed PARAMETERs FC 13/05/94
 * EXTERNALS are missing: added FC 13/05/94
 *
 * Bug: parameters and their type should be put *before* other declarations
 *      since they may use them. Changed FC 08/06/94
 *
 * COMMONS are also missing:-) added, FC 19/08/94
 *
 * updated to fully control the list to be used.
 */
/* hook for commons, when not generated...
 */
static string default_common_hook(entity module, entity common)
{
    return strdup(concatenate
        ("common to include: ", entity_local_name(common), "\n", NULL));
}

static string (*common_hook)(entity, entity) = default_common_hook; 
void set_prettyprinter_common_hook(string(*f)(entity,entity)){ common_hook=f;}
void reset_prettyprinter_common_hook(){ common_hook=default_common_hook;}

static text
text_equivalence_class(list /* of entities */ l_equiv)
{
    text t_equiv = make_text(NIL);

    /* FIRST, sort the list by increasing offset from the beginning of
       the memory suite. If two variables have the same offset, the longest 
       one comes first; if they have the same lenght, use a lexicographic
       ordering */

    /* THEN, prettyprint the sorted list*/

    return t_equiv;
}

text 
text_equivalences(entity module, list ldecl)
{
    list equiv_classes = NIL;
    text t_equiv_class;

    /* FIRST BUILD EQUIVALENCE CLASSES */
    /* consider each entity in the declaration */
    MAP(ENTITY, e,
	{
	    /* but only variables which have a ram storage must be considered */
	    if (type_variable_p(entity_type(e)) &&
		storage_ram_p(entity_storage(e)))
	    {
		list shared = ram_shared(storage_ram(entity_storage(e)));
		
		/* If this variable is statically aliased */
		if (!ENDP(shared))
		{
		    bool found = FALSE;
		    list found_equiv_class = NIL;

		    shared = CONS(ENTITY, e, shared);

		    /* We first look in already found equivalence classes
		     * if there is already a class in which one of the
		     * aliased variables appears 
		     */
		    MAP(LIST, equiv_class,
			{
			    MAP(ENTITY, ent,
				{
				    if (variable_in_list_p(ent, equiv_class))
				    {
					found = TRUE;
					break;
				    }
				},
				shared);
			    if (found)
			    {
				found_equiv_class = equiv_class;
				break;
			    }
			}, 
			equiv_classes);

		    if (found)
		    {
			/* add the entities of shared which are not already in 
			 * the existing equivalence class
			 */
			MAP(ENTITY, ent,
			    {
				if(!variable_in_list_p(ent, found_equiv_class))
				    found_equiv_class =
					gen_nconc(found_equiv_class,
						  CONS(ENTITY, ent, NIL));
			    },
			    shared)
		    }
		    else
		    {
			equiv_classes = gen_nconc(equiv_classes, shared);
		    }
		}
	    }
	},
	ldecl);

    /* SECOND, PRETTYPRINT THEM */
    t_equiv_class = make_text(NIL); 
    MAP(LIST, equiv_class,
	{
	   MERGE_TEXTS(t_equiv_class, text_equivalence_class(equiv_class));
	},
	equiv_classes);
    return(t_equiv_class);
}

static text 
text_entity_declaration(entity module, list ldecl)
{
    bool print_commons = get_bool_property("PRETTYPRINT_COMMONS"),
         from_hpfc = get_bool_property("PRETTYPRINT_HPFC");
    text r, t_chars;
    list before = NIL, area_decl = NIL, ph = NIL,
	pi = NIL, pf4 = NIL, pf8 = NIL, pl = NIL, pc = NIL, ps = NIL,
	equivalence_decl = NIL;

    t_chars = make_text(NIL); 

    MAP(ENTITY, e,
     {
	 type te = entity_type(e);
	 bool func = 
		 type_functional_p(te) && storage_rom_p(entity_storage(e));
	 bool param = func && value_symbolic_p(entity_initial(e));
	 bool external =     /* subroutines won't be declared */
		 (func && 
		  value_code_p(entity_initial(e)) &&
		  !type_void_p(functional_result(type_functional(te))));
	 bool area_p = type_area_p(te);
	 bool var = type_variable_p(te);
	 bool in_ram = storage_ram_p(entity_storage(e));
	 
	 pips_debug(3, "entity name is %s\n", entity_name(e));

	 if (!print_commons && area_p && !SPECIAL_COMMON_P(e))
	 {
	     area_decl = 
		 CONS(SENTENCE, make_sentence(is_sentence_formatted,
					      common_hook(module, e)),
		      area_decl);
	 }

	 if (!print_commons && 
	     (area_p || (var && in_ram && 
             !SPECIAL_COMMON_P(ram_section(storage_ram(entity_storage(e)))))))
	 {
	     pips_debug(5, "skipping entity %s\n", entity_name(e));
	 }
	 else if (param || external)
	 {
	     if (param) 
		 /*        PARAMETER
		  */
		 before = CONS(SENTENCE, sentence_symbolic(e), 
			       before);
	     else 
		 /*        EXTERNAL
		  */
		 before = CONS(SENTENCE, sentence_external(e), 
			       before);

	     before = CONS(SENTENCE, sentence_basic_declaration(e), 
			   before);
	 }
	 else if (area_p)
	 {
	     /*            AREAS 
	      */	     
	     area_decl = CONS(SENTENCE, sentence_area(e, module),
			   area_decl);
	 }
	 else if (var)
	 {
	     basic b = variable_basic(type_variable(te));
	     
	     switch (basic_tag(b)) 
	     {
	     case is_basic_int:
		 /* simple integers are moved ahead...
		  */
		 if (variable_dimensions(type_variable(te)))
		 {
		     pi = CHAIN_SWORD(pi, pi==NIL ? "INTEGER " : ",");
		     pi = gen_nconc(pi, words_declaration(e, !from_hpfc)); 
		 }
		 else
		 {
		     ph = CHAIN_SWORD(ph, ph==NIL ? "INTEGER " : ",");
		     ph = gen_nconc(ph, words_declaration(e, !from_hpfc)); 
		 }
		 break;
	     case is_basic_float:
		 switch (basic_float(b))
		 {
		 case 4:
		     pf4 = CHAIN_SWORD(pf4, pf4==NIL ? "REAL*4 " : ",");
		     pf4 = gen_nconc(pf4, words_declaration(e, !from_hpfc));
		     break;
		 case 8:
		 default:
		     pf8 = CHAIN_SWORD(pf8, pf8==NIL ? "REAL*8 " : ",");
		     pf8 = gen_nconc(pf8, words_declaration(e, !from_hpfc));
		     break;
		 }
		 break;			
	     case is_basic_logical:
		 pl = CHAIN_SWORD(pl, pl==NIL ? "LOGICAL " : ",");
		 pl = gen_nconc(pl, words_declaration(e, !from_hpfc));
		 break;
	     case is_basic_overloaded:
		 /* nothing! some in hpfc I guess...
		  */
		 break; 
	     case is_basic_complex:
		 pc = CHAIN_SWORD(pc, pc==NIL ? "COMPLEX " : ",");
		 pc = gen_nconc(pc, words_declaration(e, !from_hpfc));
		 break;
	     case is_basic_string:
	     {
		 value v = basic_string(b);

		 if (value_constant_p(v) && constant_int_p(value_constant(v)))
		 {
		     int i = constant_int(value_constant(v));

		     if (i==1)
		     {
			 ps = CHAIN_SWORD(ps, ps==NIL ? "CHARACTER " : ",");
			 ps = gen_nconc(ps, words_declaration(e, !from_hpfc));
		     }
		     else
		     {
			 list chars=NIL;
			 chars = CHAIN_SWORD(chars, "CHARACTER*");
			 chars = CHAIN_IWORD(chars, i);
			 chars = CHAIN_SWORD(chars, " ");
			 chars = gen_nconc(chars, 
					   words_declaration(e, !from_hpfc));
			 attach_declaration_size_type_to_words(chars, "CHARACTER", i);
			 ADD_WORD_LIST_TO_TEXT(t_chars, chars);
		     }
		 }
		 else if (value_unknown_p(v))
		 {
			 list chars=NIL;
			 chars = CHAIN_SWORD(chars, "CHARACTER*(*) ");
			 chars = gen_nconc(chars, 
					   words_declaration(e, !from_hpfc));
			 attach_declaration_type_to_words(chars, "CHARACTER*(*)");
			 ADD_WORD_LIST_TO_TEXT(t_chars, chars);
		     }
		 else
		     pips_internal_error("unexpected value\n");
		 break;
		 }
	     default:
		 pips_internal_error("unexpected basic tag (%d)\n",
				     basic_tag(b));
	     }
	 }
     }, ldecl);

    r = make_text(before);

    ADD_WORD_LIST_TO_TEXT(r, ph);
    attach_declaration_type_to_words(ph, "INTEGER");
    ADD_WORD_LIST_TO_TEXT(r, pi);
    attach_declaration_type_to_words(pi, "INTEGER");
    ADD_WORD_LIST_TO_TEXT(r, pf4);
    /* Could use attach_declaration_size_type_to_words(): */
    attach_declaration_type_to_words(pf4, "REAL*4");
    ADD_WORD_LIST_TO_TEXT(r, pf8);
    attach_declaration_type_to_words(pf8, "REAL*8");
    ADD_WORD_LIST_TO_TEXT(r, pl);
    attach_declaration_type_to_words(pl, "LOGICAL");
    ADD_WORD_LIST_TO_TEXT(r, pc);
    attach_declaration_type_to_words(pc, "COMPLEX");
    ADD_WORD_LIST_TO_TEXT(r, ps);
    attach_declaration_type_to_words(ps, "CHARACTER");
    MERGE_TEXTS(r, t_chars);
    MERGE_TEXTS(r, make_text(area_decl));

    /* And lastly, equivalence statements... - BC -*/
    MERGE_TEXTS(r, text_equivalences(module, ldecl));

    return (r);
}

text text_declaration(module)
entity module;
{
    return(text_entity_declaration(module,
				   code_declarations(entity_code(module))));
}

/* needed for hpfc 
 */
text text_common_declaration(common, module)
entity common, module;
{
    type t = entity_type(common);
    list ldecl;
    text result;

    pips_assert("indeed a common", type_area_p(t));

    ldecl = CONS(ENTITY, common, gen_copy_seq(area_layout(type_area(t))));
    result = text_entity_declaration(module, ldecl);

    gen_free_list(ldecl);
    return result;
}

text text_instruction(module, label, margin, obj, n)
entity module;
string label ;
int margin;
instruction obj;
int n ;
{
    text r=text_undefined, text_block(), text_unstructured() ;

    if (instruction_block_p(obj)) {
	r = text_block(module, label, margin, instruction_block(obj), n) ;
    }
    else if (instruction_test_p(obj)) {
	r = text_test(module, label, margin, instruction_test(obj),n);
    }
    else if (instruction_loop_p(obj)) {
	r = text_loop(module, label, margin, instruction_loop(obj),n);
    }
    else if (instruction_goto_p(obj)) {
	r = make_text(CONS(SENTENCE, 
			   sentence_goto(module, margin,
					 instruction_goto(obj)), 
			   NIL));
    }
    else if (instruction_call_p(obj)) {
	unformatted u;
	sentence s;

	if (instruction_continue_p(obj) &&
	    empty_local_label_name_p(label) &&
	    !get_bool_property("PRETTYPRINT_ALL_LABELS")) {
	    debug(5, "text_instruction", "useless CONTINUE not printed\n");
	    r = make_text(NIL);
	}
	else {
	    u = make_unformatted(strdup(label), n, margin, 
				 words_call(instruction_call(obj), 0));

	    s = make_sentence(is_sentence_unformatted, u);

	    r = make_text(CONS(SENTENCE, s, NIL));
	}
    }
    else if (instruction_unstructured_p(obj)) {
	r = text_unstructured(module, label, margin, 
			      instruction_unstructured(obj), n) ;
    }
    else {
	pips_error("text_instruction", "unexpected tag");
    }

    return(r);
}

text text_block(module, label, margin, objs, n)
entity module;
string label ;
int margin;
cons *objs;
int n;
{
    text r = make_text(NIL);
    cons *pbeg, *pend ;

    pend = NIL;

    if (ENDP(objs) && !get_bool_property("PRETTYPRINT_EMPTY_BLOCKS")) {
	return(r) ;
    }

    pips_assert("text_block", strcmp(label, "") == 0) ;
    
    if (get_bool_property("PRETTYPRINT_ALL_EFFECTS") ||
	get_bool_property("PRETTYPRINT_BLOCKS")) {
	unformatted u;
	
	if (get_bool_property("PRETTYPRINT_FOR_FORESYS")){
	    ADD_SENTENCE_TO_TEXT(r, make_sentence(is_sentence_formatted, 
						  strdup("C$BB\n")));
	}
	else {
	    pbeg = CHAIN_SWORD(NIL, "BEGIN BLOCK");
	    pend = CHAIN_SWORD(NIL, "END BLOCK");
	    
	    u = make_unformatted(strdup("C"), n, margin, pbeg);
	    ADD_SENTENCE_TO_TEXT(r, 
				 make_sentence(is_sentence_unformatted, u));
	}
    }

    for (; objs != NIL; objs = CDR(objs)) {
	statement s = STATEMENT(CAR(objs));

	text t = text_statement(module, margin, s);
	text_sentences(r) = 
	    gen_nconc(text_sentences(r), text_sentences(t));
	text_sentences(t) = NIL;
	gen_free(t);
    }

    if (!get_bool_property("PRETTYPRINT_FOR_FORESYS") &&
			   (get_bool_property("PRETTYPRINT_ALL_EFFECTS") ||
			    get_bool_property("PRETTYPRINT_BLOCKS"))) {
	unformatted u;

	u = make_unformatted(strdup("C"), n, margin, pend);

	ADD_SENTENCE_TO_TEXT(r, 
			     make_sentence(is_sentence_unformatted, u));
    }
    return(r) ;
}

/* private variables.
 * modified 2-8-94 by BA.
 * extracted as a function and cleaned a *lot*, FC.
 */
static list /* of string */
loop_private_variables(loop obj)
{
    bool
	all_private = get_bool_property("PRETTYPRINT_ALL_PRIVATE_VARIABLES"),
	hpf_private = get_bool_property("PRETTYPRINT_HPF"),
	some_before = FALSE;
    list l = NIL;

    /* comma-separated list of private variables. 
     * ??? should be build in reverse order to avoid adding at the end...
     */
    MAP(ENTITY, p,
    {
	if((p!=loop_index(obj)) || all_private) 
	{
	    if (some_before) 
		l = CHAIN_SWORD(l, ",");
	    else
		some_before = TRUE; /* from now on commas... */

	    l = gen_nconc(l, words_declaration(p,TRUE));
	}
    }, 
	loop_locals(obj)) ; /* end of MAP */
    
    pips_debug(5, "#printed %d/%d\n", gen_length(l), 
	       gen_length(loop_locals(obj)));

    /* stuff around if not empty
     */
    if (l)
    {
	l = CONS(STRING, MAKE_SWORD(hpf_private ? "NEW(" : "PRIVATE "), l);
	if (hpf_private) CHAIN_SWORD(l, ")");
    }

    return l;
}

/* returns a formatted text for the HPF independent and new directive 
 * well, no continuations and so, but the directives do not fit the 
 * unformatted domain, because the directive prolog would not be well
 * managed there.
 */
static text 
text_hpf_directive(
    loop obj,   /* the loop we're interested in */
    int margin) /* margin */
{
    list /* of string */ l = NIL, ln = NIL,
         /* of sentence */ ls = NIL;
    
    if (execution_parallel_p(loop_execution(obj)))
    {
	l = loop_private_variables(obj);
	ln = CHAIN_SWORD(ln, "INDEPENDENT");
	if (l) ln = CHAIN_SWORD(ln, ", ");
    }
    else if (get_bool_property("PRETTYPRINT_ALL_PRIVATE_VARIABLES"))
	l = loop_private_variables(obj);
    
    ln = gen_nconc(ln, l);
    
    /* ??? directly put as formatted, doesn't matter?
     */
    if (ln) 
    {
	ls = CONS(SENTENCE, 
		  make_sentence(is_sentence_formatted, strdup("\n")), NIL);
	
	ln = gen_nreverse(ln);

	MAPL(ps, 
	{
	    ls = CONS(SENTENCE,
		make_sentence(is_sentence_formatted, STRING(CAR(ps))), ls);
	},
	     ln);
	
	for (; margin>0; margin--) /* margin managed by hand:-) */
	    ls = CONS(SENTENCE, make_sentence(is_sentence_formatted, 
					    strdup(" ")), ls);

	ls = CONS(SENTENCE, make_sentence(is_sentence_formatted, 
					  strdup("CHPF$ ")), ls);

	gen_free_list(ln);
    }

    return make_text(ls);
}

text text_loop(module, label, margin, obj, n)
entity module;
string label;
int margin;
loop obj;
int n ;
{
    list pc = NIL;
    sentence first_sentence;
    unformatted u;
    text r = make_text(NIL);
    statement body = loop_body( obj ) ;
    entity the_label = loop_label(obj);
    string do_label = entity_local_name(the_label)+strlen(LABEL_PREFIX) ;
    bool structured_do = empty_local_label_name_p(do_label),
         doall_loop_p = FALSE,
         hpf_prettyprint = get_bool_property("PRETTYPRINT_HPF"),
         do_enddo_p = get_bool_property("PRETTYPRINT_DO_LABEL_AS_COMMENT"),
         all_private =  get_bool_property("PRETTYPRINT_ALL_PRIVATE_VARIABLES");

    /* small hack to show the initial label of the loop to name it...
     */
    if(!structured_do && do_enddo_p)
    {
	ADD_SENTENCE_TO_TEXT(r, make_sentence(is_sentence_formatted,
	  strdup(concatenate("C     INITIALLY: DO ", do_label, "\n", NULL))));
    }

    /* quite ugly management of other prettyprints...
     */
    switch(execution_tag(loop_execution(obj)) ) {
    case is_execution_sequential:
	doall_loop_p = FALSE;
	break ;
    case is_execution_parallel:
        if (get_bool_property("PRETTYPRINT_CMFORTRAN")) {
          text aux_r;
          if((aux_r = text_loop_cmf(module, label, margin, obj, n, NIL, NIL))
             != text_undefined) {
            MERGE_TEXTS(r, aux_r);
            return(r) ;
          }
        }
        if (get_bool_property("PRETTYPRINT_CRAFT")) {
          text aux_r;
          if((aux_r = text_loop_craft(module, label, margin, obj, n, NIL, NIL))
             != text_undefined) {
            MERGE_TEXTS(r, aux_r);
            return(r);
          }
        }
	if (get_bool_property("PRETTYPRINT_FORTRAN90") && 
	    instruction_assign_p(statement_instruction(body)) ) {
	    MERGE_TEXTS(r, text_loop_90(module, label, margin, obj, n));
	    return(r) ;
	}
	doall_loop_p = !get_bool_property("PRETTYPRINT_CRAY") &&
	    !get_bool_property("PRETTYPRINT_CMFORTRAN") &&
		!get_bool_property("PRETTYPRINT_CRAFT") && !hpf_prettyprint;
	break ;
    default:
	pips_error("text_loop", "Unknown tag\n") ;
    }

    /* HPF directives before the loop if required (INDEPENDENT and NEW)
     */
    if (hpf_prettyprint)
	MERGE_TEXTS(r, text_hpf_directive(obj, margin));

    /* LOOP prologue.
     */
    pc = CHAIN_SWORD(NIL, (doall_loop_p) ? "DOALL " : "DO " );
    
    if(!structured_do && !doall_loop_p && !do_enddo_p) {
	pc = CHAIN_SWORD(pc, concatenate(do_label, " ", NULL));
    }
    pc = CHAIN_SWORD(pc, entity_local_name(loop_index(obj)));
    pc = CHAIN_SWORD(pc, " = ");
    pc = gen_nconc(pc, words_loop_range(loop_range(obj)));
    u = make_unformatted(strdup(label), n, margin, pc) ;
    ADD_SENTENCE_TO_TEXT(r, first_sentence = make_sentence(is_sentence_unformatted, u));

    /* builds the PRIVATE scalar declaration if required
     */
    if(!ENDP(loop_locals(obj)) && (doall_loop_p || all_private)
       && !hpf_prettyprint) 
    {
	list /* of string */ lp = loop_private_variables(obj);

	if (lp) 
	    ADD_SENTENCE_TO_TEXT(r, make_sentence(is_sentence_unformatted,
	        make_unformatted(NULL, 0, margin+INDENTATION, lp)));
    }

    /* loop BODY
     */
    MERGE_TEXTS(r, text_statement(module, margin+INDENTATION, body));

    /* LOOP postlogue
     */
    if(structured_do || doall_loop_p || do_enddo_p ||
       get_bool_property("PRETTYPRINT_CRAY") ||
       get_bool_property("PRETTYPRINT_CRAFT") ||
       get_bool_property("PRETTYPRINT_CMFORTRAN"))
    {
	ADD_SENTENCE_TO_TEXT(r, MAKE_ONE_WORD_SENTENCE(margin,"ENDDO"));
    }

    attach_loop_to_sentence_up_to_end_of_text(first_sentence, r, obj);
    return r;
}

text init_text_statement(module, margin, obj)
entity module;
int margin;
statement obj;
{
    instruction i = statement_instruction(obj);
    text r = make_text( NIL ) ;
    /* string comments = statement_comments(obj); */

    if (get_bool_property("PRETTYPRINT_ALL_EFFECTS")
	|| !((instruction_block_p(i) && 
	      !get_bool_property("PRETTYPRINT_BLOCKS")) || 
	     (instruction_unstructured_p(i) && 
	      !get_bool_property("PRETTYPRINT_UNSTRUCTURED")))) {
	r = (*text_statement_hook)( module, margin, obj );
	
	if (text_statement_hook != empty_text)
	    attach_decoration_to_text(r);
    }

    /*
    if (! string_undefined_p(comments)) {
	ADD_SENTENCE_TO_TEXT(r, make_sentence(is_sentence_formatted, 
					      comments));
    }
    */
    if (get_bool_property("PRETTYPRINT_ALL_EFFECTS") ||
	get_bool_property("PRETTYPRINT_STATEMENT_ORDERING")) {
	static char buffer[ 256 ] ;
	int so = statement_ordering(obj) ;

	if (!(instruction_block_p(statement_instruction(obj)) && 
	      (! get_bool_property("PRETTYPRINT_BLOCKS")))) {

	    if (so != STATEMENT_ORDERING_UNDEFINED) {
		sprintf(buffer, "C (%d,%d)\n", 
			ORDERING_NUMBER(so), ORDERING_STATEMENT(so)) ;
		ADD_SENTENCE_TO_TEXT(r, 
				     make_sentence(is_sentence_formatted, 
						   strdup(buffer))) ;
	    }
	}
    }
    return( r ) ;
}

/* Handles all statements but tests with are nodes of an unstructured
 * Those are handled by text_control.
 */
text text_statement(module, margin, stmt)
entity module;
int margin;
statement stmt;
{
    instruction i = statement_instruction(stmt);
    text r= make_text(NIL);
    text temp;
    string label = 
	    entity_local_name(statement_label(stmt)) + strlen(LABEL_PREFIX);
    string comments = statement_comments(stmt);

    debug(2, "text_statement", "Begin for statement %s\n",
	  statement_identification(stmt));

    if (strcmp(label, RETURN_LABEL_NAME) == 0) {
	/* do not add a redundant RETURN before an END, unless required */
	if(get_bool_property("PRETTYPRINT_FINAL_RETURN")) {
	    /*
	    ADD_SENTENCE_TO_TEXT(temp,
				 MAKE_ONE_WORD_SENTENCE(margin,
							RETURN_FUNCTION_NAME));
	    */
	    sentence s = MAKE_ONE_WORD_SENTENCE(margin,
						RETURN_FUNCTION_NAME);
	    temp = make_text(CONS(SENTENCE, s ,NIL));
	}
	else {
	    temp = make_text(NIL);
	}
    }
    else {
	temp = text_instruction(module, label, margin, i,
				statement_number(stmt)) ;
    }

    if(!ENDP(text_sentences(temp))) {
	MERGE_TEXTS(r, init_text_statement(module, margin, stmt)) ;
	if (! string_undefined_p(comments)) {
	    ADD_SENTENCE_TO_TEXT(r, make_sentence(is_sentence_formatted, 
						  comments));
	}
	MERGE_TEXTS(r, temp);
    }
    else {
	/* Preserve comments */
	if (! string_undefined_p(comments)) {
	    ADD_SENTENCE_TO_TEXT(r, make_sentence(is_sentence_formatted, 
						  comments));
	}
    }

    debug(2, "text_statement", "End for statement %s\n",
	  statement_identification(stmt));
       
    return(r);
}

text text_test(module, label, margin, obj,n)
entity module;
string label ;
int margin;
test obj;
int n;
{
    text r = make_text(NIL);
    cons *pc = NIL;
    statement test_false_obj;

    pc = CHAIN_SWORD(pc, "IF (");
    pc = gen_nconc(pc, words_expression(test_condition(obj)));
    pc = CHAIN_SWORD(pc, ") THEN");

    ADD_SENTENCE_TO_TEXT(r, 
			 make_sentence(is_sentence_unformatted, 
				       make_unformatted(strdup(label), n, 
							margin, pc)));
    MERGE_TEXTS(r, text_statement(module, margin+INDENTATION, 
				  test_true(obj)));

    test_false_obj = test_false(obj);
    if(statement_undefined_p(test_false_obj)){
      pips_error("text_test","undefined statement\n");
    }
    if (((!empty_statement_p(test_false_obj)) &&
	(!statement_continue_p(test_false_obj))) ||
	(empty_statement_p(test_false_obj) &&
	 (get_bool_property("PRETTYPRINT_EMPTY_BLOCKS"))) ||
	(statement_continue_p(test_false_obj) &&
	 (get_bool_property("PRETTYPRINT_ALL_LABELS")))) {
	ADD_SENTENCE_TO_TEXT(r, MAKE_ONE_WORD_SENTENCE(margin,"ELSE"));
	MERGE_TEXTS(r, text_statement(module, margin+INDENTATION, 
				      test_false_obj));
      }

    ADD_SENTENCE_TO_TEXT(r, MAKE_ONE_WORD_SENTENCE(margin,"ENDIF"));

    return(r);
}

/* hook for adding something in the head. used by hpfc.
 * done so to avoid hpfc->prettyprint dependence in the libs. 
 * FC. 29/12/95.
 */
static string (*head_hook)(entity) = NULL;
void set_prettyprinter_head_hook(string(*f)(entity)){ head_hook=f;}
void reset_prettyprinter_head_hook(){ head_hook=NULL;}

/* function text text_module(module, stat)
 *
 * carefull! the original text of the declarations is used
 * if possible. Otherwise, the function text_declaration is called.
 */
text
text_module(entity module,
	    statement stat)
{
    text r = make_text(NIL);
    code c = entity_code(module);
    string s = code_decls_text(c);

    if ( strcmp(s,"") == 0 
	|| get_bool_property("PRETTYPRINT_ALL_DECLARATIONS") )
    {
	if (get_bool_property("PRETTYPRINT_HEADER_COMMENTS"))
	    /* Add the original header comments if any: */
	    ADD_SENTENCE_TO_TEXT(r, get_header_comments(module));
	
	ADD_SENTENCE_TO_TEXT(r, attach_head_to_sentence(sentence_head(module),
							module));
	if (head_hook) 
	    ADD_SENTENCE_TO_TEXT(r, make_sentence(is_sentence_formatted,
						  head_hook(module)));
	
	if (get_bool_property("PRETTYPRINT_HEADER_COMMENTS"))
	    /* Add the original header comments if any: */
	    ADD_SENTENCE_TO_TEXT(r, get_declaration_comments(module));
	
	MERGE_TEXTS(r, text_declaration(module));
    }
    else {
	ADD_SENTENCE_TO_TEXT(r, attach_head_to_sentence(make_sentence(is_sentence_formatted, s),
							module));
    }

    if (stat != statement_undefined) {
	MERGE_TEXTS(r, text_statement(module, 0, stat));
    }

    ADD_SENTENCE_TO_TEXT(r, sentence_tail());

    return(r);
}

text text_graph(), text_control() ;
string control_slabel() ;


void
output_a_graph_view_of_the_unstructured_successors(text r,
                                                   entity module,
                                                   int margin,
                                                   control c)
{                  
   add_one_unformated_printf_to_text(r, "%s %#x\n",
                                     PRETTYPRINT_UNSTRUCTURED_ITEM_MARKER,
                                     (unsigned int) c);

   if (get_bool_property("PRETTYPRINT_UNSTRUCTURED_AS_A_GRAPH_VERBOSE")) {
      add_one_unformated_printf_to_text(r, "C Unstructured node %#x ->",
                                        (unsigned int) c);
      MAP(CONTROL, a_successor,
          {
             add_one_unformated_printf_to_text(r," %#x",
                                               (unsigned int) a_successor);
          },
             control_successors(c));
      add_one_unformated_printf_to_text(r,"\n");
   }

   MERGE_TEXTS(r, text_statement(module,
                                 margin,
                                 control_statement(c)));

   add_one_unformated_printf_to_text(r,
                                     PRETTYPRINT_UNSTRUCTURED_SUCCESSOR_MARKER);
   MAP(CONTROL, a_successor,
       {
          add_one_unformated_printf_to_text(r," %#x",
                                            (unsigned int) a_successor);
       },
          control_successors(c));
   add_one_unformated_printf_to_text(r,"\n");
}


bool
output_a_graph_view_of_the_unstructured_from_a_control(text r,
                                                       entity module,
                                                       int margin,
                                                       control begin_control,
                                                       control exit_control)
{
   bool exit_node_has_been_displayed = FALSE;
   list blocs = NIL;
   
   CONTROL_MAP(c,
               {
                  /* Display the statements of each node followed by
                     the list of its successors if any: */
                  output_a_graph_view_of_the_unstructured_successors(r,
                                                                     module,
                                                                     margin,
                                                                     c);
                  if (c == exit_control)
                     exit_node_has_been_displayed = TRUE;
               },
                  begin_control,
                  blocs);
   gen_free_list(blocs);

   return exit_node_has_been_displayed;
}

void
output_a_graph_view_of_the_unstructured(text r,
                                        entity module,
                                        string label,
                                        int margin,
                                        unstructured u,
                                        int num)
{
   bool exit_node_has_been_displayed = FALSE;
   control begin_control = unstructured_control(u);
   control end_control = unstructured_exit(u);

   add_one_unformated_printf_to_text(r, "%s %#x end: %#x\n",
                                     PRETTYPRINT_UNSTRUCTURED_BEGIN_MARKER,
                                     (unsigned int) begin_control,
                                     (unsigned int) end_control);
   exit_node_has_been_displayed =
      output_a_graph_view_of_the_unstructured_from_a_control(r,
                                                             module,
                                                             margin,
                                                             begin_control,
                                                             end_control);
   
   /* If we have not displayed the exit node, that mean that it is not
      connex with the entry node and so the code is
      unreachable. Anyway, it has to be displayed as for the classical
      Sequential View: */
   if (! exit_node_has_been_displayed) {
      /* Note that since the controlizer adds a dummy successor to the
         exit node, use
         output_a_graph_view_of_the_unstructured_from_a_control()
         instead of
         output_a_graph_view_of_the_unstructured_successors(): */
      output_a_graph_view_of_the_unstructured_from_a_control(r,
                                                             module,
                                                             margin,
                                                             end_control,
                                                             end_control);
      /* Even if the code is unreachable, add the fact that the
         control above is semantically related to the entry node. Add
         a dash arrow from the entry node to the exit node in daVinci,
         for example: */
      add_one_unformated_printf_to_text(r, "%s %#x -> %#x\n",
                                        PRETTYPRINT_UNREACHABLE_EXIT_MARKER,
                                        (unsigned int) begin_control,
                                        (unsigned int) end_control);
   }
   
   add_one_unformated_printf_to_text(r, "%s %#x end: %#x\n",
                                     PRETTYPRINT_UNSTRUCTURED_END_MARKER,
                                     (unsigned int) begin_control,
                                     (unsigned int) end_control);
}


/* TEXT_UNSTRUCTURED prettyprints the control graph CT (with label number
   NUM) from the MODULE at the current LABEL. If CEXIT == CT, then there
   is only one node and the goto+continue can be eliminated. */

text
text_unstructured(entity module,
                  string label,
                  int margin,
                  unstructured u, int num)
{
   text r = make_text(NIL);
   hash_table labels = hash_table_make(hash_pointer, 0) ;
   set trail = set_make(set_pointer) ;
   control previous = control_undefined ;
   cons *blocs = NIL ;
   control cexit = unstructured_exit(u) ;
   control ct = unstructured_control(u) ;

   debug(2, "text_unstructured", "Begin for unstructured %x\n",
	 (unsigned int) u);

   if (get_bool_property("PRETTYPRINT_UNSTRUCTURED_AS_A_GRAPH"))
   {
      output_a_graph_view_of_the_unstructured
	  (r, module, label, margin, u, num);
   }
   else {
       list pbeg, pend;

       if(get_bool_property("PRETTYPRINT_UNSTRUCTURED")) {
	   pbeg = CHAIN_SWORD(NIL, "BEGIN UNSTRUCTURED");
	    
	   u = make_unformatted(strdup("C"), num, margin, pbeg);
	   ADD_SENTENCE_TO_TEXT(r, 
				make_sentence(is_sentence_unformatted, u));
       }

       MERGE_TEXTS(r,
		   text_graph(module, margin, ct,
			      &previous, trail, labels, cexit));

       MERGE_TEXTS(r, text_control(module, margin, cexit,
				   &previous, set_make(set_pointer), labels,
				   cexit)) ;

       if(get_bool_property("PRETTYPRINT_UNSTRUCTURED")) {
	   pend = CHAIN_SWORD(NIL, "END UNSTRUCTURED");
	    
	   u = make_unformatted(strdup("C"), num, margin, pend);
	   ADD_SENTENCE_TO_TEXT(r, 
				make_sentence(is_sentence_unformatted, u));
       }
   }
   
   ifdebug(9) {
      fprintf(stderr,"Unstructured %x (%x, %x)\n", 
              (unsigned int) u, (unsigned int) ct, (unsigned int) cexit ) ;
      CONTROL_MAP( n, {
         statement st = control_statement(n) ;

         fprintf(stderr, "\n%*sNode %x (%s)\n--\n", margin, "", 
                 (unsigned int) n, control_slabel(module, n, labels)) ;
         print_text(stderr, text_statement(module,margin,st));
         fprintf(stderr, "--\n%*sPreds:", margin, "");
         MAPL(ps,{fprintf(stderr,"%x ", (unsigned int) CONTROL(CAR(ps)));},
         control_predecessors(n));
         fprintf(stderr, "\n%*sSuccs:", margin, "") ;
         MAPL(ss,{fprintf(stderr,"%x ", (unsigned int) CONTROL(CAR(ss)));},
         control_successors(n));
         fprintf(stderr, "\n\n") ;
      }, ct , blocs) ;
      gen_free_list(blocs);
   }
   hash_table_free(labels) ;
   set_free(trail) ;

   debug(2, "text_unstructured", "End for unstructured %x\n",
	 (unsigned int) u);

   return(r) ;
}

/* CONTROL_SLABEL returns a freshly allocated label name for the control
node C in the module M. H maps controls to label names. Computes a new
label name. */

string control_slabel(m, c, h)
entity m;
control c;
hash_table h;
{
    string l;

    if ((l = hash_get(h, (char *) c)) == HASH_UNDEFINED_VALUE) {
	statement st = control_statement(c) ;
	string label = entity_name( statement_label( st )) ;

	l = empty_label_p( label ) ? new_label_name(m) : label ;
	hash_put(h, (char *) c, strdup(l)) ;
    }
    pips_assert("control_slabel", strcmp(local_name(l), LABEL_PREFIX) != 0) ;
    pips_assert("control_slabel", strcmp(local_name(l), "") != 0) ;
    pips_assert("control_slabel", strcmp(local_name(l), "=") != 0) ;
    return(strdup(l));
}

/* ADD_CONTROL_GOTO adds to the text R a goto statement to the control
node SUCC from the current one OBJ in the MODULE and with a MARGIN.
LABELS maps control nodes to label names and SEENS (that links the
already prettyprinted node) is used to see whether a simple fall-through
wouldn't do. */

static void add_control_goto(module, margin, r, obj, 
			     succ, labels, seens, cexit )
entity module;
int margin;
text r ;
control obj, succ, cexit ;
hash_table labels;
set seens ;
{
    string label ;

    if( succ == (control)NULL ) {
	return ;
    }
    label = local_name(control_slabel(module, succ, labels))+
	    strlen(LABEL_PREFIX);

    if (strcmp(label, RETURN_LABEL_NAME) == 0 ||
	seens == (set)NULL || 
	(get_bool_property("PRETTYPRINT_INTERNAL_RETURN") && succ == cexit) ||
	set_belong_p(seens, (char *)succ)) {
	ADD_SENTENCE_TO_TEXT(r, sentence_goto_label(module, margin, label));
    }
}

/* TEXT_CONTROL prettyprints the control node OBJ in the MODULE with a
MARGIN. SEENS is a trail that keeps track of already printed nodes and
LABELS maps control nodes to label names. The previously printed control
is in PREVIOUS. */

text text_control(module, margin, obj, previous, seens, labels, cexit)
entity module;
int margin;
control obj, *previous, cexit ;
set seens ;
hash_table labels;
{
    text r = make_text(NIL);
    sentence s;
    unformatted u ;
    statement st = control_statement(obj) ;
    cons *succs, *preds ;
    cons *pc;
    string label;
    string label_name ;
    string comments = statement_comments(st);

    debug(2, "text_control", "Begin for statement %s\n",
	  statement_identification(st));

    label = control_slabel(module, obj, labels);
    label_name = strdup(local_name(label)+strlen(LABEL_PREFIX)) ;

    switch(gen_length(preds=control_predecessors(obj))) {
    case 0:
	break ;
    case 1: 
	if (*previous == CONTROL(CAR(preds)) &&
	    (obj != cexit || 
	     !get_bool_property("PRETTYPRINT_INTERNAL_RETURN"))) {
	    break ;
	}
    default:
	if( empty_label_p( entity_name( statement_label( st )))) {
	    pc = CHAIN_SWORD(NIL,"CONTINUE") ;
	    s = make_sentence(is_sentence_unformatted,
			      make_unformatted(NULL, 0, margin, pc)) ;
	    unformatted_label(sentence_unformatted(s)) = label_name ;
	    ADD_SENTENCE_TO_TEXT(r, s);    
	}
    }
    switch(gen_length(succs=control_successors(obj))) {
    case 0:
	MERGE_TEXTS(r, text_statement(module, margin, st));
	add_control_goto(module, margin, r, obj, 
			 cexit, labels, seens, (control)NULL ) ;
	break ;
    case 1:
	MERGE_TEXTS(r, text_statement(module, margin, st));
	add_control_goto(module, margin, r, obj, 
			 CONTROL(CAR(succs)), labels, seens, cexit) ;
	break;
    case 2: {
	instruction i = statement_instruction(st);
	test t;

	assert(instruction_test_p(i));

	MERGE_TEXTS(r, init_text_statement(module, margin, st)) ;
	if (! string_undefined_p(comments)) {
	    ADD_SENTENCE_TO_TEXT(r, make_sentence(is_sentence_formatted, 
						  comments));
	}
	pc = CHAIN_SWORD(NIL, "IF (");
	t = instruction_test(i);
	pc = gen_nconc(pc, words_expression(test_condition(t)));
	pc = CHAIN_SWORD(pc, ") THEN");
	u = make_unformatted(NULL, statement_number(st), margin, pc) ;

	if( !empty_label_p( entity_name( statement_label( st )))) {
	    unformatted_label(u) = strdup(label_name) ;
	}
	s = make_sentence(is_sentence_unformatted,u) ;
	ADD_SENTENCE_TO_TEXT(r, s);
	add_control_goto(module, margin+INDENTATION, r, obj, 
			 CONTROL(CAR(succs)), labels, seens, cexit) ;
	ADD_SENTENCE_TO_TEXT(r, MAKE_ONE_WORD_SENTENCE(margin,"ELSE"));
	add_control_goto(module, margin+INDENTATION, r, obj, 
			 CONTROL(CAR(CDR(succs))), labels, (set)NULL, cexit) ;
	ADD_SENTENCE_TO_TEXT(r, MAKE_ONE_WORD_SENTENCE(margin,"ENDIF"));
	break;
    }
    default:
	pips_error("text_graph", "incorrect number of successors\n");
    }

    debug(2, "text_control", "End for statement %s\n",
	  statement_identification(st));

    return( r ) ;
}

/* TEXT_GRAPH prettyprints the control graph OBJ in the MODULE with a
MARGIN. SEENS is a trail that keeps track of already printed nodes and
LABELS maps control nodes to label names. The previously printed control
is in PREVIOUS. CEXIT is not printed, done latter. */

text text_graph(module, margin, obj, previous, seens, labels, cexit)
entity module;
int margin;
control obj, *previous, cexit ;
set seens ;
hash_table labels;
{
    text r ;

    if(set_belong_p(seens, (char *)obj) || obj == cexit ) {
	return( make_text( NIL )) ;
    }
    set_add_element(seens, seens, (char *)obj) ;
    r = text_control(module, margin, obj, previous, seens, labels, cexit);
     *previous = obj ;

    MAPL(ss, {
	MERGE_TEXTS(r, text_graph(module, margin, CONTROL(CAR(ss)), 
				  previous, seens, labels, cexit));
    }, control_successors(obj));

    return( r );
}

cons *words_parameters(e)
entity e;
{
    cons *pc = NIL;
    type te = entity_type(e);
    functional fe;
    int nparams, i;

    pips_assert("words_parameters", type_functional_p(te));

    fe = type_functional(te);
    nparams = gen_length(functional_parameters(fe));

    for (i = 1; i <= nparams; i++) {
	entity param = find_ith_parameter(e, i);

	if (pc != NIL) {
	    pc = CHAIN_SWORD(pc, ",");
	}

	pc = CHAIN_SWORD(pc, entity_local_name(param));
    }

    return(pc);
}


/* This function is added by LZ
 * 21/10/91
 * extended to cope with PRETTYPRINT_HPFC 
 */

/* some compilers don't like dimensions that are declared twice.
 * this is the case of g77 used after hpfc. thus I added a
 * flag not to prettyprint again the dimensions of common variables. FC.
 *
 * It is in the standard that dimensions cannot be declared twice in a 
 * single module. BC.
 */
list words_declaration(
    entity e,
    bool prettyprint_common_variable_dimensions_p)
{
    list pl = NIL;
    pl = CHAIN_SWORD(pl, entity_local_name(e));

    if (type_variable_p(entity_type(e)))
    {
	if (!((variable_in_common_p(e) || variable_static_p(e)) &&
	      !prettyprint_common_variable_dimensions_p))
	{
	    if (variable_dimensions(type_variable(entity_type(e))) != NIL) 
	    {
		list dims = variable_dimensions(type_variable(entity_type(e)));
	
		pl = CHAIN_SWORD(pl, "(");

		MAPL(pd, 
		     {
			 pl = gen_nconc(pl, words_dimension(DIMENSION(CAR(pd))));
			 if (CDR(pd) != NIL) pl = CHAIN_SWORD(pl, ",");
		     }, 
		     dims);
	
		pl = CHAIN_SWORD(pl, ")");
	    }
	}
    }
    
    attach_declaration_to_words(pl, e);

    return(pl);
}

cons *words_basic(obj)
basic obj;
{
    cons *pc = NIL;

    if (basic_int_p(obj)) {
	pc = CHAIN_SWORD(pc,"INTEGER*");
	pc = CHAIN_IWORD(pc,basic_int(obj));
    }
    else if (basic_float_p(obj)) {
	pc = CHAIN_SWORD(pc,"REAL*");
	pc = CHAIN_IWORD(pc,basic_float(obj));
    }
    else if (basic_logical_p(obj)) {
	pc = CHAIN_SWORD(pc,"LOGICAL*");
	pc = CHAIN_IWORD(pc,basic_logical(obj));
    }
    else if (basic_overloaded_p(obj)) {
	pc = CHAIN_SWORD(pc,"OVERLOADED");
    }
    else if (basic_complex_p(obj)) {
	pc = CHAIN_SWORD(pc,"COMPLEX*");
	pc = CHAIN_IWORD(pc,basic_complex(obj));
    }
    else if (basic_string_p(obj)) {
	pc = CHAIN_SWORD(pc,"STRING*(");
	pc = gen_nconc(pc, words_value(basic_string(obj)));
	pc = CHAIN_SWORD(pc,")");
    }
    else {
	pips_error("words_basic", "unexpected tag");
    }

    return(pc);
}

cons *words_value(obj)
value obj;
{
    cons *pc;

    if (value_symbolic_p(obj)) {
	pc = words_constant(symbolic_constant(value_symbolic(obj)));
    }
    else if (value_constant(obj)) {
	pc = words_constant(value_constant(obj));
    }
    else {
	pips_error("words_value", "unexpected tag");
	pc = NIL;
    }

    return(pc);
}

cons *words_constant(obj)
constant obj;
{
    cons *pc;

    pc=NIL;

    if (constant_int_p(obj)) {
	pc = CHAIN_IWORD(pc,constant_int(obj));
    }
    else {
	pips_error("words_constant", "unexpected tag");
    }

    return(pc);
}

cons *words_dimension(obj)
dimension obj;
{
    cons *pc;

    pc = words_expression(dimension_lower(obj));
    pc = CHAIN_SWORD(pc,":");
    pc = gen_nconc(pc, words_expression(dimension_upper(obj)));

    return(pc);
}

cons *words_expression(obj)
expression obj;
{
    cons *pc;

    pc = words_syntax(expression_syntax(obj));

    return(pc);
}

cons *words_subexpression(obj, precedence)
expression obj;
int precedence;
{
    cons *pc;

    if ( expression_call_p(obj) )
	pc = words_call(syntax_call(expression_syntax(obj)), precedence);
    else 
	pc = words_syntax(expression_syntax(obj));

     return(pc);
}

cons *words_loop_range(obj)
range obj;
{
    cons *pc;
    call c = syntax_call(expression_syntax(range_increment(obj)));

    pc = words_subexpression(range_lower(obj), 0);
    pc = CHAIN_SWORD(pc,", ");
    pc = gen_nconc(pc, words_subexpression(range_upper(obj), 0));
    if (/*  expression_constant_p(range_increment(obj)) && */
	 strcmp( entity_local_name(call_function(c)), "1") == 0 )
	return(pc);
    pc = CHAIN_SWORD(pc,", ");
    pc = gen_nconc(pc, words_expression(range_increment(obj)));

    return(pc);
}

list /* of string */ words_range(range obj)
{
    cons *pc = NIL ;

    /* if undefined I print a star, why not!? */
    if (expression_undefined_p(range_lower(obj)))
	return CONS(STRING, MAKE_SWORD("*"), NIL);
    /* else */
    pc = CHAIN_SWORD(pc,"(/I,I=");
    pc = gen_nconc(pc, words_expression(range_lower(obj)));
    pc = CHAIN_SWORD(pc,",");
    pc = gen_nconc(pc, words_expression(range_upper(obj)));
    pc = CHAIN_SWORD(pc,",");
    pc = gen_nconc(pc, words_expression(range_increment(obj)));
    pc = CHAIN_SWORD(pc,"/)") ;

    return(pc);
}

cons *words_reference(obj)
reference obj;
{
    cons *pc = NIL;
    string begin_attachment;
    
    entity e = reference_variable(obj);

    pc = CHAIN_SWORD(pc, entity_local_name(e));
    begin_attachment = STRING(CAR(gen_last(pc)));
    
    if (reference_indices(obj) != NIL) {
	pc = CHAIN_SWORD(pc,"(");
	MAPL(pi, {
	    pc = gen_nconc(pc, words_subexpression(EXPRESSION(CAR(pi)), 0));
	    if (CDR(pi) != NIL)
		pc = CHAIN_SWORD(pc,",");
	}, reference_indices(obj));
	pc = CHAIN_SWORD(pc,")");
    }
    attach_reference_to_word_list(begin_attachment, STRING(CAR(gen_last(pc))),
				  obj);

    return(pc);
}

cons *words_label_name(s)
string s ;
{
    return(CHAIN_SWORD(NIL, local_name(s)+strlen(LABEL_PREFIX))) ;
}

cons *words_syntax(obj)
syntax obj;
{
    cons *pc;

    if (syntax_reference_p(obj)) {
	pc = words_reference(syntax_reference(obj));
    }
    else if (syntax_range_p(obj)) {
	pc = words_range(syntax_range(obj));
    }
    else if (syntax_call_p(obj)) {
	pc = words_call(syntax_call(obj), 0);
    }
    else {
	pips_error("words_syntax", "tag inconnu");
	pc = NIL;
    }

    return(pc);
}

cons *words_call(obj, precedence)
call obj;
int precedence;
{
    cons *pc;

    entity f = call_function(obj);
    value i = entity_initial(f);
    
    pc = (value_intrinsic_p(i)) ? words_intrinsic_call(obj, precedence) : 
	                          words_regular_call(obj);

    return(pc);
}

cons *words_regular_call(obj)
call obj;
{
    cons *pc = NIL;

    entity f = call_function(obj);
    value i = entity_initial(f);
    type t = entity_type(f);
    
    if (call_arguments(obj) == NIL) {
	if (type_statement_p(t))
	    return(CHAIN_SWORD(pc, entity_local_name(f)+strlen(LABEL_PREFIX)));
	if (value_constant_p(i)||value_symbolic_p(i))
	    return(CHAIN_SWORD(pc, entity_local_name(f)));
    }

    if (type_void_p(functional_result(type_functional(t)))) {
	pc = CHAIN_SWORD(pc, "CALL ");
    }

    pc = CHAIN_SWORD(pc, entity_local_name(f));

    if( !ENDP( call_arguments(obj))) {
	pc = CHAIN_SWORD(pc, "(");
	MAPL(pa, {
	    pc = gen_nconc(pc, words_expression(EXPRESSION(CAR(pa))));
	    if (CDR(pa) != NIL)
		    pc = CHAIN_SWORD(pc, ", ");
	}, call_arguments(obj));
	pc = CHAIN_SWORD(pc, ")");
    }
    else if(!type_void_p(functional_result(type_functional(t)))) {
	pc = CHAIN_SWORD(pc, "()");
    }
    return(pc);
}

cons *words_prefix_unary_op(obj, precedence)
call obj;
int precedence;
{
    cons *pc = NIL;
    expression e = EXPRESSION(CAR(call_arguments(obj)));
    int prec = words_intrinsic_precedence(obj);

    pc = CHAIN_SWORD(pc, entity_local_name(call_function(obj)));
    pc = gen_nconc(pc, words_subexpression(e, prec));

    return(pc);
}

cons *words_unary_minus(obj, precedence)
call obj;
int precedence;
{
    cons *pc = NIL;
    expression e = EXPRESSION(CAR(call_arguments(obj)));
    int prec = words_intrinsic_precedence(obj);

    if ( prec < precedence )
	pc = CHAIN_SWORD(pc, "(");
    pc = CHAIN_SWORD(pc, "-");
    pc = gen_nconc(pc, words_subexpression(e, prec));
    if ( prec < precedence )
	pc = CHAIN_SWORD(pc, ")");

    return(pc);
}

/* 
 * If the infix operator is either "-" or "/", I perfer not to delete 
 * the parentheses of the second expression.
 * Ex: T = X - ( Y - Z ) and T = X / (Y*Z)
 *
 * Lei ZHOU       Nov. 4 , 1991
 */
cons *words_infix_binary_op(obj, precedence)
call obj;
int precedence;
{
    cons *pc = NIL;
    cons *args = call_arguments(obj);
    int prec = words_intrinsic_precedence(obj);
    cons *we1 = words_subexpression(EXPRESSION(CAR(args)), prec);
    cons *we2;

    if ( strcmp(entity_local_name(call_function(obj)), "/") == 0 )
	we2 = words_subexpression(EXPRESSION(CAR(CDR(args))), 100);
    else if ( strcmp(entity_local_name(call_function(obj)), "-") == 0 ) {
	expression exp = EXPRESSION(CAR(CDR(args)));
	if ( expression_call_p(exp) &&
	     words_intrinsic_precedence(syntax_call(expression_syntax(exp))) >= 
	     intrinsic_precedence("*") )
	    /* precedence is greter than * or / */
	    we2 = words_subexpression(exp, prec);
	else
	    we2 = words_subexpression(exp, 100);
    }
    else
	we2 = words_subexpression(EXPRESSION(CAR(CDR(args))), prec);

    
    if ( prec < precedence )
	pc = CHAIN_SWORD(pc, "(");
    pc = gen_nconc(pc, we1);
    pc = CHAIN_SWORD(pc, entity_local_name(call_function(obj)));
    pc = gen_nconc(pc, we2);
    if ( prec < precedence )
	pc = CHAIN_SWORD(pc, ")");

    return(pc);
}

cons *words_assign_op(obj, precedence)
call obj;
int precedence;
{
    cons *pc = NIL;
    cons *args = call_arguments(obj);
    int prec = words_intrinsic_precedence(obj);

    pc = gen_nconc(pc, words_subexpression(EXPRESSION(CAR(args)),  prec));
    pc = CHAIN_SWORD(pc, " = ");
    pc = gen_nconc(pc, words_subexpression(EXPRESSION(CAR(CDR(args))), prec));

    return(pc);
}

cons *words_nullary_op(obj, precedence)
call obj;
int precedence;
{
    cons *pc = NIL;

    pc = CHAIN_SWORD(pc, entity_local_name(call_function(obj)));

    return(pc);
}

cons *words_io_control(iol, precedence)
cons **iol;
int precedence;
{
    cons *pc = NIL;
    cons *pio = *iol;

    while (pio != NIL) {
	syntax s = expression_syntax(EXPRESSION(CAR(pio)));
	call c;

	if (! syntax_call_p(s)) {
	    pips_error("words_io_control", "call expected");
	}

	c = syntax_call(s);

	if (strcmp(entity_local_name(call_function(c)), "IOLIST=") == 0) {
	    *iol = CDR(pio);
	    return(pc);
	}

	if (pc != NIL)
	    pc = CHAIN_SWORD(pc, ",");
	
	pc = CHAIN_SWORD(pc, entity_local_name(call_function(c)));
	pc = gen_nconc(pc, words_expression(EXPRESSION(CAR(CDR(pio)))));

	pio = CDR(CDR(pio));
    }

    if (pio != NIL)
	    pips_error("words_io_control", "bad format");

    *iol = NIL;

    return(pc);
}

cons *words_implied_do(obj, precedence)
call obj;
int precedence;
{
    cons *pc = NIL;

    cons *pcc;
    expression index;
    syntax s;
    range r;

    pcc = call_arguments(obj);
    index = EXPRESSION(CAR(pcc));

    pcc = CDR(pcc);
    s = expression_syntax(EXPRESSION(CAR(pcc)));
    if (! syntax_range_p(s)) {
	pips_error("words_implied_do", "range expected");
    }
    r = syntax_range(s);

    pc = CHAIN_SWORD(pc, "(");
    MAPL(pcp, {
	pc = gen_nconc(pc, words_expression(EXPRESSION(CAR(pcp))));
	if (CDR(pcp) != NIL)
	    pc = CHAIN_SWORD(pc, ",");
    }, CDR(pcc));
    pc = CHAIN_SWORD(pc, ", ");

    pc = gen_nconc(pc, words_expression(index));
    pc = CHAIN_SWORD(pc, " = ");
    pc = gen_nconc(pc, words_loop_range(r));
    pc = CHAIN_SWORD(pc, ")");

    return(pc);
}

cons *words_unbounded_dimension(obj, precedence)
call obj;
int precedence;
{
    cons *pc = NIL;

    pc = CHAIN_SWORD(pc, "*");

    return(pc);
}

cons *words_list_directed(obj, precedence)
call obj;
int precedence;
{
    cons *pc = NIL;

    pc = CHAIN_SWORD(pc, "*");

    return(pc);
}

cons *words_io_inst(obj, precedence)
call obj;
int precedence;
{
    cons *pc = NIL;
    cons *pcio = call_arguments(obj);
    cons *pio_write;
    boolean good_fmt, good_unit, iolist_reached;

    /* AP: I try to convert WRITE to PRINT. Three conditions must be
       fullfilled. The first, and obvious, one, is that the function has
       to be WRITE. Secondly, "FMT" has to be equal to "*". Finally,
       "UNIT" has to be equal either to "*" or "6".  In such case,
       "WRITE(*,*)" is replaced by "PRINT *,". */
    /* GO: Not anymore for UNIT=6 leave it ... */
    good_fmt = FALSE;
    good_unit = FALSE;
    pio_write = pcio;

    if (strcmp(entity_local_name(call_function(obj)), "WRITE") == 0) {
      iolist_reached = FALSE;
      while ((pio_write != NIL) && (!iolist_reached)) {
	syntax s = expression_syntax(EXPRESSION(CAR(pio_write)));
	call c;
	expression arg = EXPRESSION(CAR(CDR(pio_write)));

	if (! syntax_call_p(s)) {
	    pips_error("words_io_inst", "call expected");
	}

	c = syntax_call(s);

	if ((strcmp(entity_local_name(call_function(c)), "FMT=") == 0) &&
	    (strcmp(words_to_string(words_expression(arg)), "*") == 0))
	    good_fmt= TRUE;

	if ((strcmp(entity_local_name(call_function(c)), "UNIT=") == 0) &&
	    (strcmp(words_to_string(words_expression(arg)), "*") == 0))
	    good_unit = TRUE;

	if (strcmp(entity_local_name(call_function(c)), "IOLIST=") == 0) {
	  iolist_reached = TRUE;
	  pio_write = CDR(pio_write);
	}
	else
	  pio_write = CDR(CDR(pio_write));
      }
    }

    if (good_fmt && good_unit) {
      /* AP: Allright for the substitution of WRITE by PRINT. For the
         IOLIST prettyprint, we skip everything but elements following the
         first "IOLIST=" keyword. */
      pc = CHAIN_SWORD(pc, "PRINT *,");
      pcio = pio_write;
    }
    else {
      pc = CHAIN_SWORD(pc, entity_local_name(call_function(obj)));
      pc = CHAIN_SWORD(pc, " (");
      /* FI: missing argument; I use "precedence" because I've no clue;
         see LZ */
      pc = gen_nconc(pc, words_io_control(&pcio, precedence));
      pc = CHAIN_SWORD(pc, ") ");
    }

    /* because the "IOLIST=" keyword is embedded in the list
       and because the first IOLIST= has already been skipped,
       only odd elements are printed */
    MAPL(pp, {
	pc = gen_nconc(pc, words_expression(EXPRESSION(CAR(pp))));

	if (CDR(pp) != NIL) {
	    POP(pp);
	    if(pp==NIL) 
		pips_error("words_io_inst","missing element in IO list");
	    pc = CHAIN_SWORD(pc, ",");
	}
    }, pcio);
    return(pc) ;
}

cons *null(obj, precedence)
call obj;
int precedence;
{
    return(NIL);
}

/* precedence needed here
 * According to the Precedence of Operators 
 * Arithmetic > Character > Relational > Logical
 * Added by Lei ZHOU    Nov. 4,91
 */
struct intrinsic_handler {
    char * name;
    cons *(*f)();
    int prec;
} tab_intrinsic_handler[] = {
    {"**", words_infix_binary_op, 30},

    {"//", words_infix_binary_op, 30},

    {"--", words_unary_minus, 25},

    {"*", words_infix_binary_op, 21},
    {"/", words_infix_binary_op, 21},

    {"+", words_infix_binary_op, 20},
    {"-", words_infix_binary_op, 20},


    {".LT.", words_infix_binary_op, 15},
    {".GT.", words_infix_binary_op, 15},
    {".LE.", words_infix_binary_op, 15},
    {".GE.", words_infix_binary_op, 15},
    {".EQ.", words_infix_binary_op, 15},
    {".NE.", words_infix_binary_op, 15},

    {".NOT.", words_prefix_unary_op, 9},

    {".AND.", words_infix_binary_op, 8},

    {".OR.", words_infix_binary_op, 6},

    {".EQV.", words_infix_binary_op, 3},
    {".NEQV.", words_infix_binary_op, 3},

    {"=", words_assign_op, 1},

    {"WRITE", words_io_inst, 0},
    {"READ", words_io_inst, 0},
    {"PRINT", words_io_inst, 0},
    {"OPEN", words_io_inst, 0},
    {"CLOSE", words_io_inst, 0},
    {"INQUIRE", words_io_inst, 0},
    {"BACKSPACE", words_io_inst, 0},
    {"REWIND", words_io_inst, 0},
    {"ENDFILE", words_io_inst, 0},
    {"IMPLIED-DO", words_implied_do, 0},

    {RETURN_FUNCTION_NAME, words_nullary_op, 0},
    {"PAUSE", words_nullary_op, 0},
    {"STOP", words_nullary_op, 0},
    {"CONTINUE", words_nullary_op, 0},
    {"END", words_nullary_op, 0},
    {"FORMAT", words_prefix_unary_op, 0},
    {UNBOUNDED_DIMENSION_NAME, words_unbounded_dimension, 0},
    {LIST_DIRECTED_FORMAT_NAME, words_list_directed, 0},

    {NULL, null, 0}
};

cons *words_intrinsic_call(obj, precedence)
call obj;
int precedence;
{
    struct intrinsic_handler *p = tab_intrinsic_handler;
    char *n = entity_local_name(call_function(obj));

    while (p->name != NULL) {
	if (strcmp(p->name, n) == 0) {
	    return((*(p->f))(obj, precedence));
	}
	p++;
    }

    return(words_regular_call(obj));
}

int words_intrinsic_precedence(obj)
call obj;
{
    char *n = entity_local_name(call_function(obj));

    return(intrinsic_precedence(n));
}

int intrinsic_precedence(n)
string n;
{
    struct intrinsic_handler *p = tab_intrinsic_handler;

    while (p->name != NULL) {
	if (strcmp(p->name, n) == 0) {
	    return(p->prec);
	}
	p++;
    }

    return(0);
}
