 /* package plint */

#include <stdio.h>
#include "boolean.h"
#include "arithmetique.h"
#include "vecteur.h"
#include "contrainte.h"
#include "sc.h"

#include "sommet.h"

/* pour recuperer les declarations des fonctions de conversion de
 * sc en liste de sommets et reciproquement, bien que ca casse le
 * DAG des types de donnees
 */
#include "ray_dte.h"
#include "polyedre.h"
#include "matrix.h"

#include "plint.h"

/* Psysteme plreal(Psysteme first_sys, Psommet fonct):
 * Resolution d'un systeme lineaire en nombres reels positifs
 *
 *
 *  resultat retourne par la fonction :
 *
 *  Psysteme       : systeme lineaire 
 *
 *  Les parametres de la fonction :
 *
 *  Psysteme first_syst  : systeme lineaire initial
 *  Psommet fonct       : fonction economique du  programme lineaire
 */
Psysteme plreal(first_sys,fonct)
Psysteme first_sys;
Psommet fonct;
{
    Psommet sys1 = NULL;
    Psysteme syst_res = NULL;
    Pvecteur lvbase = NULL;
    int nb_som = 0;
    int nbvars;
    Pbase b =BASE_NULLE;

    if (first_sys) {
	nbvars = first_sys->dimension;
	b = base_dup(first_sys->base);

	if ((first_sys->egalites != NULL) 
	    || (first_sys->inegalites != NULL)) {
	    /* conversion du systeme lineaire en sommet    */
	    sys1 = sys_som_conv(first_sys,&nb_som);
	    /* ajout des variables d'ecart   */
	    if ((sys1 = eq_in_ineq(&sys1,&nb_som,&lvbase)) != NULL) {
		sys1 = var_ecart_sup(sys1,nb_som,&lvbase,&nbvars,&b);
		sys1 = primal_pivot(sys1,&lvbase,nb_som,fonct);
	    }

	    /* calcul de la solution reelle positive  */
	    while ((sys1 != NULL) && 
		   (dual_pivot_pas(&sys1,&lvbase,nb_som,fonct,&nbvars,&b) == TRUE));

	    /* si le systeme est non faisable,
	       liberation de l'espace memoire ocupe par la fonction 
	       economique  */
	    if (sys1 == NULL && fonct != NULL) {
		vect_rm(fonct->vecteur);
		fonct->vecteur = NULL;
	    }
	    /* conversion du sommet en systeme lineaire  */
	    syst_res = som_sys_conv(sys1);
	    /* si le systeme est non faisable,
	       liberation de l'espace memoire occupe par les variables */
	    if (syst_res != NULL) {
		syst_res->base = b;
		syst_res->dimension = nbvars;
	    }
	}
    } 
    sommets_rm(sys1);
    vect_rm(lvbase);

    return(syst_res);
}
