/* %A% ($Date: 1996/09/21 12:02:39 $, ) 
    version $Revision$, got on %D%, %T% [%P%].
   Copyright (c) - �cole des Mines de Paris Proprietary.  */

#ifndef lint
char top_level_newgen_c_vcid[] = "%A% ($Date: 1996/09/21 12:02:39 $, ) version $Revision$, got on %D%, %T% [%P%].\n Copyright (c) �cole des Mines de Paris Proprietary.";
#endif /* lint */
#include <stdio.h>

#include "genC.h"
#include "specs.h"
#include "all_newgen_headers.h"

#include "ri-util.h"

void initialize_newgen()
{
    extern void sg_rm();
    extern sg_dup();

    /* Read NewGen specification file
     */
    gen_read_spec(ALL_SPECS);
      
    /* Initialise external functions...
     * re-entry in newgen because of the graph stuff...
     */  
    gen_init_external(ARC_LABEL_NEWGEN_EXTERNAL, 
		      (char *(*)()) gen_read, 
		      (void (*)()) gen_write,
		      (void (*)()) gen_free, 
		      (char *(*)()) gen_copy_tree_with_sharing,
		      (int (*)()) gen_allocated_memory);

    gen_init_external(VERTEX_LABEL_NEWGEN_EXTERNAL, 
		      (char *(*)()) gen_read, 
		      (void (*)()) gen_write,
		      (void (*)()) gen_free, 
		      (char *(*)()) gen_copy_tree_with_sharing,
		      (int (*)()) gen_allocated_memory);

    gen_init_external(PPOLYNOME_NEWGEN_EXTERNAL, 
		      (char *(*)()) polynome_gen_read, 
		      (void (*)()) polynome_gen_write, 
		      (void (*)()) polynome_gen_free,
		      (char *(*)()) polynome_gen_copy_tree,
		      (int (*)()) polynome_gen_allocated_memory);

    gen_init_external(PVECTEUR_NEWGEN_EXTERNAL, 
		      (char *(*)()) vect_gen_read, 
		      (void (*)()) vect_gen_write, 
		      (void (*)()) vect_gen_free,
		      (char *(*)()) vect_gen_copy_tree,
		      (int (*)()) vect_gen_allocated_memory);

    gen_init_external(PSYSTEME_NEWGEN_EXTERNAL, 
		      (char *(*)()) sc_gen_read, 
		      (void (*)()) sc_gen_write, 
		      (void (*)()) sc_gen_free,
		      (char *(*)()) sc_gen_copy_tree,
		      (int (*)()) sc_gen_allocated_memory);

    gen_init_external(MATRICE_NEWGEN_EXTERNAL, 
		      (char *(*)()) gen_core,
		      (void (*)()) gen_core, 
		      (void (*)()) free,
		      (char *(*)()) gen_core,
		      (int (*)()) NULL); /* can't get the size... FC */

    gen_init_external(PTSG_NEWGEN_EXTERNAL,
		      (char *(*)()) gen_core,
		      (void (*)()) gen_core, 
		      (void (*)()) sg_rm,
		      (char *(*)()) sg_dup,
		      (int (*)()) NULL); /* can't get the size... FC */
}
