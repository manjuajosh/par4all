/* Basic functions about control flow graphs (CFG), aka unstructured data structure
 *
 * Francois Irigoin, 1997.
 */

#include <stdlib.h> 
#include <stdio.h> 

#include "genC.h"
#include "misc.h"
#include "ri.h"
#include "ri-util.h"
#include "control.h"

/* Check some nice properties expected to be true after a powerful
 * enough controlizer phase.
 *
 * The calls to pips_assert could be replaced by calls to user_warning,
 * the control graph could be printed out before core dumping,...
 *
 * See also check_control_coherency().
 */

bool
unstructured_consistency_p(unstructured u, bool assert_p)
{
    control entry_node = unstructured_control(u);
    control exit_node = unstructured_exit(u);
    bool consistency_p = TRUE;
    list nodes = NIL;
    
    forward_control_map_get_blocs(entry_node, &nodes);

    /* the exit and entry nodes must be different or the
     * unstructured would have been reduced to one structured statement
     */
    if(exit_node==entry_node) {
	consistency_p = FALSE;
	if(assert_p) pips_assert("Entry and exit nodes are different", FALSE);
    }

    /* The entry node must have at least one predecessor:
     * else it should have been moved out of the unstructured.
     * Note that it has in fact TWO predecessors since it is the
     * unstructured entry point.
     */
    if(consistency_p && ENDP(control_predecessors(entry_node))) {
	consistency_p = FALSE;
	if(assert_p) pips_assert("Entry node has at least one predecessor", FALSE);
    }

    /* The exit node should have no successors: its real successor
     * is the statement following the unstructured
     */
    if(consistency_p && !ENDP(control_successors(exit_node))) {
	consistency_p = FALSE;
	if(assert_p) pips_assert("Exit node has no successor", FALSE);
    }

    /* The exit node should not be a test:
     * else one of its successors should be control_undefined 
     * since it would be outside of the unstructured, or the
     * exit condition would be unknown.
     */
    if(consistency_p) {
	statement exit_s = control_statement(exit_node);
	if(statement_test_p(exit_s)) {
	    consistency_p = FALSE;
	    if(assert_p) pips_assert("Exit node is not a test", FALSE);
	}
    }

    /* The exit node may have no predecessor because an infinite loop
     * appears in the code. Or it is a successor of the entry node.
     */
    if(consistency_p && !ENDP(control_predecessors(exit_node))) {
	if(gen_find_eq(exit_node, nodes)!=exit_node) {
	    consistency_p = FALSE;
	    if(assert_p) pips_assert("Exit node has predecessors but is unreachable", FALSE);
	}
    }

    /* RK: ??? If the exit node is unreachable, it should be a CONTINUE
     * Since a CONTINUE has no effects, it does not perturb analyses even
     * if they do not check reachability.
     * RK: The NOP might be the empty sequence instead of a CONTINUE?
     */
    if(consistency_p && ENDP(control_predecessors(exit_node))) {
	statement exit_s = control_statement(exit_node);

	if(!statement_continue_p(exit_s)) {
	    consistency_p = FALSE;
	    if(assert_p) pips_assert("Unreachable exit node is a CONTINUE statement", FALSE);
	}
    }

    /* If the exit node has only one predecessor, it should
     * be a test or the exit node would have been moved outside of
     * the unstructured.
     */
    if(consistency_p && gen_length(control_predecessors(exit_node))==1) {
	control c = CONTROL(CAR(control_predecessors(exit_node)));
	statement s = control_statement(c);

	if(!statement_test_p(s)) {
	    consistency_p = FALSE;
	    if(assert_p) pips_assert("Unique predecessor of exit node is a test", FALSE);
	}
    }

    /* Since sequences are merged a unique node, if a node c1 has only one
     * successor c2, and if c2 has only one predecessor c3, c3 must be c1
     * and either c1 is the exit node which is impossible because there
     * would be no exit condition and the exit node would have a
     * successor, c2, or c2 is the entry node. So c2 must be the entry node.
     *
     * Well, this is not true because only tests can have two successors.
     * hence, a test node cannot be merged.
     */
    MAP(CONTROL, c1, {
	if(gen_length(control_successors(c1))==1) {
	    control c2 = CONTROL(CAR(control_successors(c1)));
	    statement s2 = control_statement(c2);

	    if(gen_length(control_predecessors(c2))==1) {
		control c3 = CONTROL(CAR(control_successors(c2)));
		if(c1!=c3) {
		    consistency_p = FALSE;
		    if(assert_p) 
			pips_assert("c3, the unique predecessor of the unique successor of c1, is c1",
				    FALSE);
		}
		else if(c2!=entry_node && !statement_test_p(s2)) {
		    consistency_p = FALSE;
		    (void) fprintf(stderr,
				   "\nEntry node: %p\nExit node: %p\nc1 node: %p\nc2 node: %p\n\n",
				   entry_node, exit_node, c1, c2);
		    display_linked_control_nodes(entry_node);
		    if(assert_p) 
			pips_assert("c2 is the unique successor of c1. "
				    "c2 has c1 as unique predecessor. c2 must be the entry node",
				    FALSE);
		}
	    }

	}
	}, nodes)

    gen_free_list(nodes);

    return consistency_p;
}


bool
unstructured_while_p(unstructured u)
{
    control entry_node = unstructured_control(u);
    control exit_node = unstructured_exit(u);
    bool while_p = FALSE;

    /* Make sure that the unstructured has been "normalized" */
    while_p = unstructured_consistency_p(u, FALSE);

    /* An unstructured is a while loop if and only if the exit node
     * has only one predecessor and if the entry node is a successor
     * of the predecessor of the exit node.
     *
     * A loop with two exit conditions is not recognized as a while loop.
     *
     * A loop with an exit test on entry, a final exit test (do...while) or 
     * an exit test anywhere is recognized as a while loop.
     */

    if(while_p && gen_length(control_predecessors(exit_node))==1) {
	control pred = CONTROL(CAR(control_predecessors(exit_node)));
	list succs = NIL;

	forward_control_map_get_blocs(pred, &succs);

	if(gen_find_eq(entry_node, succs)==entry_node) {
	    while_p = TRUE;
	}
	gen_free_list(succs);
    }
    return while_p;
}
