 /* package plint */

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>

#include "boolean.h"
#include "arithmetique.h"
#include "vecteur.h"
#include "contrainte.h"
#include "sc.h"

#include "sommet.h"

#define MALLOC(s,t,f) malloc(s)
#define FREE(s,t,f) free(s)


/* Psommet sommets_dup(Psommet som):
 * copie d'une liste de sommets tout en respectant le meme ordre 
 */
Psommet sommets_dup(som)
Psommet som;
{

    Psommet som1,som2;
    /* pointeur vers le dernier element de la liste*/
    Psommet pred;
    /* pointeur vers le premier element de la liste*/
    Psommet debut = NULL;


#ifdef TRACE
    printf(" ***** duplication du systeme \n");
#endif
    if (som)
    {
	som2 = sommet_dup(som);
	debut = som2;
	pred = som2;
	for (som1 = som->succ; som1 != NULL; som1 = som1->succ)
	{

	    som2 = sommet_dup(som1);
	    pred->succ = som2;
	    pred = som2;
	};
    };
    return (debut);
}

/* void sommets_rm(Psommet ps):
 * liberation de l'espace memoire alloue a une liste de sommets
 *
 */
void sommets_rm(ps)
Psommet ps;
{
    Psommet p1,p2;

#ifdef TRACE2
    printf(" ***** erase systeme \n");
#endif	

    for (p2 = ps;p2 != NULL;)
    {
	p1 = p2->succ;
	FREE((int *)p2->eq_sat,INTEGER,"sommets_rm");
	vect_rm(p2->vecteur);
	FREE((Psommet)p2,SOMMET,"sommets_rm");
	p2 = p1;
    }

}

/* void sommets_normalize(som)
 * Normalisation des elements d'une liste de sommets i.e. division par le pgcd de tous
 * les elements.
 *
 */
void sommets_normalize(som)
Psommet som;
{
    Psommet ps;
    for (ps = som;ps!= NULL; ps= ps->succ)
	sommet_normalize(ps);
}

