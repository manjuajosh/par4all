/* $RCSfile: sc_simplexe_feasibility.c,v $ (version $Revision$)
 * $Date: 1996/07/24 11:23:10 $, 
 */

/* test du simplex : ce test s'appelle par :
 *  programme fichier1.data fichier2.data ... fichiern.data
 * ou bien : programme<fichier.data
 * Si on compile grace a` "make sim" dans le directory
 *  /home/users/pips/C3/Linear/Development/polyedre.dir/test.dir
 * alors on peut tester l'execution dans le meme directory
 * en faisant : tests|more
 */

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <limits.h>
#include <setjmp.h>

#include "boolean.h"
#include "arithmetique.h"
#include "assert.h"
#include "vecteur.h"
#include "contrainte.h"
#include "sc.h"

extern jmp_buf overflow_error;

/* To replace #define NB_EQ and #define NB_INEQ - BC, 2/4/96 - 
 * NB_EQ and NB_INEQ are initialized at the beginning of the main subroutine.
 * they represent the number of non-NULL constraints in sc. This is useful
 * to allocate the minimum amount of memory necessary.  */
static int NB_EQ = 0;
static int NB_INEQ = 0;

#define DEBUG(code)  /* nothing, could be "code" */
#define DEBUG1(code) /* idem */
#define DEBUG2(code) /* idem */

#define PTR_NIL ((char*)0xdeadbeef)
#define INFINI VALUE_MAX
#define MAX_VAR 1971 /* nombre max de variables */

/* seuil au dela duquel on se mefie d'un overflow
 */
#if defined(LINEAR_VALUE_IS_LONGLONG)
#define MAXVAL 576
#else
#define MAXVAL 24  
#endif

#define DIMENSION sc->dimension
#define NUMERO hashtable[h].numero
#define SOLUBLE(N) soluble=N;goto FINSIMPLEX ;
#define CREVARVISIBLE variables[compteur-3]=compteur-2;
#define CREVARCACHEE { variablescachees[nbvariables]=nbvariables + MAX_VAR ; \
			 if (nbvariables ++ >= MAX_VAR) abort(); }

/* for tracing macros after expansion: 
 */
#define tag(x) /* printf(x); */

/*************************************************** MACROS FOR FRACTIONS */
/* maybe most of them should be functions?
 */

/* SIMPL normalizes rational a/b (b<>0):
 *   divides by gcd(a,b) and returns with b>0
 * note that there should be no arithmetic exceptions within this macro:
 * (well, only uminus may have trouble for VALUE_MIN...)
 */
#define SIMPL(a,b) 						\
{tag("SIMPL")							\
    if (value_notone_p(a) && value_notone_p(b))			\
    {								\
	Value i=a, j=b, k;					\
	while (value_notzero_p(k=value_mod(i,j)))		\
	{							\
	    i=j, j=k, value_division(a,k), value_division(b,k);	\
	}							\
	if (value_neg_p(b))					\
	    value_oppose(a), value_oppose(b);			\
    }								\
}

/* G computes j=gcd(a,b) assuming b>0 and better with a>b.
 * there can be no artihmetic errors.
 */
#define G(j,a,b)					\
{tag("G")						\
    j=b;						\
    if (value_gt(b,VALUE_ONE))				\
    {							\
	Value i=a, k;					\
	while(value_notzero_p(k=value_mod(i,j)))	\
	    i=j, j=k;					\
    }							\
    if (value_neg_p(j))					\
	value_oppose(j);				\
}

#define GCD(j,a,b)				\
{tag("GCD")					\
    if (value_gt(a,b)) 				\
      { G(j,a,b); } 				\
    else 					\
      { G(j,b,a); }				\
}

/* SIMPLIFIE normalizes fraction f
 */
#define SIMPLIFIE(f) { tag("SIMPLIFIE") SIMPL(f.num,f.den)}

#define AFF(x,y) {x.num=y.num; x.den=y.den;} /* x=y should be ok:-) */
#define INV(x,y) {x.num=y.den; x.den=y.num;} /* x=1/y */

#define METINFINI(f) {f.num=VALUE_MAX;  f.den=VALUE_ONE;}
#define MET_ZERO(f)  {f.num=VALUE_ZERO; f.den=VALUE_ONE;}
#define MET_UN(f)    {f.num=VALUE_ONE;  f.den=VALUE_ONE;}

#define EGAL1(x) (value_eq(x.num,x.den))
#define EGAL0(x) (value_zero_p(x.num))
#define NUL(x) (value_zero_p(x.num))

#define NEGATIF(x)					\
  ((value_neg_p(x.num) && value_pos_p(x.den)) || 	\
   (value_pos_p(x.num) && value_neg_p(x.den)))

#define POSITIF(x)					\
  ((value_pos_p(x.num) && value_pos_p(x.den)) || 	\
   (value_neg_p(x.num) && value_neg_p(x.den)))

#define SUP1(x)								   \
  ((value_pos_p(x.num) && value_pos_p(x.den) &&  value_gt(x.num,x.den)) || \
   (value_neg_p(x.num) && value_neg_p(x.den) &&  value_gt(x.den,x.num)))

#define EGAL_MACRO(x,y,mult)					\
  ((value_zero_p(x.num) && value_zero_p(y.num)) || 		\
   (value_notzero_p(x.den) && value_notzero_p(y.den) && 	\
    value_eq(mult(x.num,y.den),mult(x.den,y.num))))

#define INF_MACRO(x,y,mult) (value_lt(mult(x.num,y.den),mult(x.den,y.num)))

/* computes x = simplify(y/z)
 */
#define DIV_MACRO(x,y,z,mult)			\
{tag("DIV_MACRO")				\
    if (value_zero_p(y.num))			\
    {						\
	MET_ZERO(x);				\
    }						\
    else					\
    {						\
	x.num=mult(y.num,z.den);		\
	x.den=mult(y.den,z.num);		\
	SIMPLIFIE(x);				\
    }						\
}

/* computes x = simplify(y*z)
 */
#define MUL_MACRO(x,y,z,mult) 				\
{tag("MUL_MACRO")					\
    if(value_zero_p(y.num) || value_zero_p(z.num))	\
	MET_ZERO(x)					\
    else 						\
    {							\
	x.num=mult(y.num,z.num);			\
        x.den=mult(y.den,z.den);			\
	SIMPLIFIE(x);					\
    }							\
}

/* computes X = simplify(A-B)
 */
#define SUB_MACRO(X,A,B,mult)						      \
{ tag("SUB_MACRO")							      \
    if (value_zero_p(A.num))						      \
	X.num = value_uminus(B.num),					      \
	X.den = B.den;							      \
    else if (value_zero_p(B.num))					      \
    { AFF(X, A); }							      \
    else if (value_eq(A.den,B.den))					      \
    {									      \
	X.num = value_minus(A.num,B.num);				      \
	X.den = A.den;							      \
	if (value_notone_p(A.den))					      \
	    { SIMPLIFIE(X);}						      \
    }									      \
    else /* must compute the stuff: */					      \
    {									      \
	Value ad=A.den, bd=B.den, gd, v;				      \
	GCD(gd,ad,bd);							      \
	if (value_notone_p(gd)) value_division(ad,gd), value_division(bd,gd); \
        X.num = mult(A.num,bd);						      \
        v = mult(B.num,ad);						      \
	value_substract(X.num,v);					      \
	X.den = mult(ad,bd);						      \
	SIMPLIFIE(X);							      \
	SIMPL(X.num,gd);						      \
	X.den = mult(X.den,gd);						      \
    }									      \
}

#define pivot_debug_macro(what)						\
  DEBUG2({ printf(what);printfrac(A);printfrac(B);			\
           printfrac(C);printfrac(D);printf(" \n");printfrac(X);})

/* computes X = A - B*C/D, trying to avoid arithmetic exceptions...
 */
#define FULL_PIVOT_MACRO_SIOUX(X,A,B,C,D,mult) 				\
{									\
    frac u,v,w; tag("FULL_PIVOT_SIOUX")					\
    AFF(u,B); AFF(v,C); INV(w,D); /* u*v*w == B*C/D */			\
    SIMPL(u.num,v.den); SIMPL(u.num,w.den);				\
    SIMPL(v.num,u.den); SIMPL(v.num,w.den);				\
    SIMPL(w.num,u.den); SIMPL(w.num,v.den);				\
    u.num = mult(u.num,v.num); /* u*=v */				\
    u.den = mult(u.den,v.den);						\
    u.num = mult(u.num,w.num); /* u*=w */				\
    u.den = mult(u.den,w.den);						\
    SUB_MACRO(X,A,u,mult);						\
    pivot_debug_macro("");						\
}

/* computes X = A - B*C/D, but does not try to avoid arithmetic exceptions
 */
#define FULL_PIVOT_MACRO_DIRECT(X,A,B,C,D,mult)				  \
{									  \
    Value v; tag("FULL_PIVOT_DIRECT")					  \
    X.num = mult(A.num,B.den);						  \
    X.num = mult(X.den,C.den);						  \
    X.num = mult(X.den,D.num);						  \
    v = mult(A.den,B.num);						  \
    v = mult(v,C.num);							  \
    v = mult(v,D.den);							  \
    value_substract(X.num,v);						  \
    X.den = mult(A.den,B.den);						  \
    X.den = mult(X.den,C.den);						  \
    X.den = mult(X.den,D.num);						  \
    SIMPLIFIE(X);							  \
}

#define direct_p(v) (value_lt(v,MAXVAL))

/* computes X = A - B*C/D, with a swtich to use SIOUX or DIRECT
 * thae rationale for the actual condition is quite fuzzy.
 */
#define FULL_PIVOT_MACRO(X,A,B,C,D,mult)		\
{ tag("FULL_PIVOT")					\
    if (direct_p(A.den) && direct_p(B.den) &&		\
	direct_p(C.den) && direct_p(D.num))		\
    {							\
	FULL_PIVOT_MACRO_DIRECT(X,A,B,C,D,mult);	\
    }							\
    else						\
    {							\
	FULL_PIVOT_MACRO_SIOUX(X,A,B,C,D,mult);		\
    }							\
} 

/* idem if A==0
 */
#define PARTIAL_PIVOT_MACRO_SIOUX(X,B,C,D,mult)		\
{ tag("PARTIAL_PIVOT_SIOUX")				\
    frac u;						\
    MUL_MACRO(u,B,C,mult); /* u=simplify(b*c) */	\
    DIV_MACRO(X,u,D,mult); /* x=simplify(u/d) */	\
    value_oppose(X.num);   /* x=-x */			\
    pivot_debug_macro("++ ");				\
}

#define PARTIAL_PIVOT_MACRO_DIRECT(X,B,C,D,mult)	\
{ tag("PARTIAL_PIVOT_DIRECT")				\
    X.num = mult(B.num,C.num);				\
    X.num = mult(X.num,D.den);				\
    value_oppose(X.num);			\
    X.den = mult(B.den,C.den);				\
    X.den = mult(X.den,D.num);				\
    SIMPLIFIE(X);					\
}

#define PARTIAL_PIVOT_MACRO(X,B,C,D,mult)			\
{								\
    if (direct_p(B.den) && direct_p(C.den) && direct_p(D.num))	\
    {								\
	PARTIAL_PIVOT_MACRO_DIRECT(X,B,C,D,mult);		\
    }								\
    else							\
    {								\
	PARTIAL_PIVOT_MACRO_SIOUX(X,B,C,D,mult);		\
    }								\
}

/* Pivot :  x = a - b c / d
 * mult is used for multiplying values.
 * the macro has changed a lot, for indentation and so... FC.
 */
#define PIVOT_MACRO(X,A,B,C,D,mult)					      \
{									      \
   if (value_zero_p(A.num))/* a==0? */					      \
   {									      \
       if (value_zero_p(B.num) || value_zero_p(C.num) || value_zero_p(D.den)) \
	   { MET_ZERO(X);}						      \
       else /* b*c/d != 0, calculons! */				      \
	   { PARTIAL_PIVOT_MACRO(X,B,C,D,mult);}			      \
   }									      \
   else /* a!=0 */							      \
      if (value_zero_p(B.num) || value_zero_p(C.num) || value_zero_p(D.den))  \
	  { AFF(X,A);}							      \
      else /*  b*c/d != 0, calculons! */				      \
	  if (value_one_p(D.num) && value_one_p(A.den) &&		      \
	      value_one_p(B.den) && value_one_p(C.den))			      \
	  { /* no den to compute */					      \
	      Value v = mult(B.num,C.num);				      \
              v = mult(v,D.den);					      \
	      X.num=value_minus(A.num,v);				      \
	      X.den=VALUE_ONE;						      \
	  }								      \
	  else /* well, we must compute the full formula! */		      \
	      { FULL_PIVOT_MACRO(X,A,B,C,D,mult);}			      \
}

/* multiplies two Values of no arithmetic overflow, or throw exception.
 * should be value_mult? FC.
 */
#define value_mult_ae(v,w)					\
((value_notzero_p(v) && value_notzero_p(w) &&			\
  value_lt(value_abs(v),value_div(VALUE_MAX,value_abs(w))))?	\
 value_mult(v,w): (longjmp(overflow_error, 5), VALUE_MAX))

/* Version with and without arithmetic exceptions...
 */
#define MULT(RES,A,B) RES=value_mult(A,B)
#define MULTOFL(RES,A,B) RES=value_mult_ae(A,B)

#define DIV(x,y,z) DIV_MACRO(x,y,z,value_mult)
#define DIVOFL(x,y,z) DIV_MACRO(x,y,z,value_mult_ae)

#define MUL(x,y,z) MUL_MACRO(x,y,z,value_mult)
#define MULOFL(x,y,z) MUL_MACRO(x,y,z,value_mult_ae)

#define SUB(X,A,B) SUB_MACRO(X,A,B,value_mult)
#define SUBOFL(X,A,B) SUB_MACRO(X,A,B,value_mult_ae)

#define PIVOT(X,A,B,C,D) PIVOT_MACRO(X,A,B,C,D,value_mult)
#define PIVOTOFL(X,A,B,C,D) PIVOT_MACRO(X,A,B,C,D,value_mult_ae)

#define EGAL(x,y) EGAL_MACRO(x,y,value_mult)
#define EGALOFL(x,y) EGAL_MACRO(x,y,value_mult_ae)

#define INF(x,y) INF_MACRO(x,y,value_mult)
#define INFOFL(x,y) INF_MACRO(x,y,value_mult_ae)

/* this is already too much...
 */

typedef struct
{
    Variable nom;
    int numero; 
    int hash ;
    Value val ;
    int succ ;
} hashtable_t;

static void
dump_hashtable(hashtable_t hashtable[])
{
    int i;
    for(i=0;i<MAX_VAR;i++) 
	if(hashtable[i].nom != 0) 
	    printf("%s %d ", hashtable[i].nom, hashtable[i].numero),
	    print_Value(hashtable[i].val),
	    printf("\n");
}

/* Le nombre de variables visibles est : compteur-2
 * La i-eme variable visible a le numero : variables[i+1]=i
 *   (0 <= i < compteur-2)
 * Le nombre de variables cachees est : nbvarables
 * La i-eme variable cachee a le numero : variablescachees[i+1]=MAX_VAR+i-1
 *   (0 <= i < nbvariables)
 */
/* utilise'es par dump_tableau ; a rendre local */
static int nbvariables, variablescachees[MAX_VAR], variables[MAX_VAR] ; 
static frac frac0={0,0,0} ;

static void printfrac(frac x) {
    printf(" "); print_Value(x.num);
    printf("/"); print_Value(x.den);
}

static void dump_tableau(tableau *t,int colonnes) {
    int i,j, k, w;
    int max=0;
    for(i=0;i<colonnes;i++) 
      if(t[i].colonne[t[i].taille-1].numero>max)max=t[i].colonne[t[i].taille-1].numero ; 
    printf("Dump du tableau ------ %d colonnes  %d lignes\n",colonnes,max) ;
    printf("%d Variables  visibles :\n",colonnes-2) ;
    for(i=0;i<colonnes-2;i++) printf("%7d",variables[i]) ;
    printf("\n") ;
    printf("%d Variables cachees :\n",nbvariables);
    for(i=0;i<nbvariables;i++) printf("%7d",variablescachees[i]) ;
    printf("\n") ;

    DEBUG(for(i=0;i<colonnes;i++) {
	    if(t[i].existe != 0) {
		printf("Colonne %d Existe=%d Taille=%d\n",i,
		       t[i].existe,t[i].taille) ;
		for(j=0 ; j<t[i].taille ; j++) {
		    printf("ligne %d valeur", t[i].colonne[j].numero);
		    printfrac(t[i].colonne[j]);
		    printf("\n");
		}
	    }
	}) /* DEBUG */
  
    printf("Nb lignes: %d\n", max);
    for(j=0;j<=max;j++) { printf("\nLigne %d ",j) ;
        for(i=0;i<colonnes;i++) {
            w=1 ;
            for(k=0;k<t[i].taille;k++)
                if(t[i].colonne[k].numero==j)
		    printfrac(t[i].colonne[k]) , w=0 ;
            if(w!=0)printfrac(frac0) ;
        }
    }
    printf("\n");
} /* dump_tableau */


/* calcule le hashcode d'un pointeur
   sous forme d'un nombre compris entre 0 et  MAX_VAR */
static int hash(Variable s) 
{ int i ;
  i=(long)s % MAX_VAR ;
  return (i) ;
}

                 
/* fonction de calcul de la faisabilite' d'un systeme
 * d'equations et d'inequations
 * Auteur : Robert Mahl, Date : janvier 1994
 */
/* Retourne : 1 si le systeme est soluble (faisable)
 *  en rationnels,
 * 0 s'il n'y a pas de solution.
 */
/* overflow control :
 *  ofl_ctrl == NO_OFL_CTRL  => no overflow control
 *  ofl_ctrl == FWD_OFL_CTRL  
 *           => overflow control is made (longjmp(overflow_error,5))
 * BC, 13/12/94
 */
boolean 
sc_simplexe_feasibility_ofl_ctrl(
    Psysteme sc, 
    int ofl_ctrl)
{
    Pcontrainte pc, pc_tmp ;
    Pvecteur pv ;
    int premier_hash = (int) PTR_NIL; /* tete de liste des noms de variables */
    /* Necessaire de declarer "hashtable" static 
     *  pour initialiser tout automatiquement a` 0.
     * Necessaire de chainer les enregistrements
     *  pour reinitialiser a 0
     *  en sortie de la procedure.
     */
    static hashtable_t hashtable[MAX_VAR] ;
    jmp_buf overflow_error3;
    tableau *eg = NULL; /* tableau des egalite's  */
    tableau *t = NULL; /* tableau des inegalite's  */
    /* les colonnes 0 et 1 sont reservees au terme const: */
    int compteur = 2 ;
    long i, j, k, h, trouve, hh=0, ligne, i0, i1, jj, ii ;
    Value poidsM, valeur, tmpval;
    long w ;
    int soluble = 1 ; /* valeur retournee par feasible */
    frac *nlle_colonne = NULL, *colo;
    frac objectif[2] ; /* objectif de max pour simplex : 
			  somme des (b2,c2) termes constants "inferieurs" */
    frac rapport1, rapport2, min1, min2, pivot, cc ;
    
    /* Allocation a priori du tableau des egalites.
     * "eg" : tableau a "nb_eq" lignes et "dimension"+2 colonnes.
     */
    
    /* the input Psysteme must be consistent; this is not the best way to
     * do this; array bound checks should be added instead in proper places;
     * no time to do it properly for the moment. BC.
     */
    assert(sc_weak_consistent_p(sc));

    /* Do not allocate place for NULL constraints */
    NB_EQ = 0;
    NB_INEQ = 0;
    for(pc_tmp = sc->egalites; pc_tmp!= NULL; pc_tmp=pc_tmp->succ) 
    {
	if (pc_tmp->vecteur != NULL)
	    NB_EQ++;
    }
    for(pc_tmp = sc->inegalites; pc_tmp!= NULL; pc_tmp=pc_tmp->succ) 
    {
	if (pc_tmp->vecteur != NULL)
	    NB_INEQ++;
    }
    
    if (setjmp(overflow_error3))
    {
	for(i=premier_hash ; i!=(int)PTR_NIL; i=hashtable[i].succ)
	    hashtable[i].nom = 0 ;
	if(NB_EQ > 0) {
	    for(i=0 ; i<(3+DIMENSION) ; i++)
		free(eg[i].colonne);
	    free(eg);
	}

	/* I have noticed that when pips core dumps here, it is because
	 * a setjmp(overflow_error) has been forgotten. bc.
	 */
	for(i=0;i<(3 + NB_INEQ + NB_EQ + DIMENSION); i++)  
	    free(t[i].colonne); 
	free(t); 
	free(nlle_colonne); 
	if (ofl_ctrl == FWD_OFL_CTRL) 
	    longjmp(overflow_error,5);
	return TRUE; 
    }
    else
    {
	if(NB_EQ != 0)
	{
	    eg=(tableau*)malloc((3+DIMENSION)*sizeof(tableau)) ;
	    for(i=0 ; i<(3+DIMENSION) ; i++)
	    {
		eg[i].colonne=(frac*)malloc(NB_EQ*sizeof(frac)) ;
		eg[i].existe = 0 ;
		eg[i].taille = 0 ;
	    }
	}
    /* Determination d'un numero pour chaque variable */
    
    for(pc=sc->egalites, ligne=1 ; pc!=0; pc=pc->succ, ligne++)
    {
	pv=pc->vecteur;
	if (pv!=NULL) /* skip if empty */
	{
	    j=0 ; /* compteur du nb de variables de l'equation */
	    valeur=0 ; /* le terme cst vaut 0 par defaut */
	    for(; pv !=0 ; pv=pv->succ) {
		if(vect_coeff(pv->var,sc_base(sc))) {
		    j++ ;
		    h = hash((Variable) pv->var) ; trouve=0 ;
		    while (hashtable[h].nom != 0)  {
			if (hashtable[h].nom==pv->var) {
			    trouve=1 ;
			    break ;
			}
			else { h = (h+1) % MAX_VAR ; }
		    }
		    if(!trouve) {
			hashtable[h].succ=premier_hash ;
			premier_hash = h ;
			hashtable[h].val = VALUE_NAN ;
			hashtable[h].numero=compteur++ ;
			CREVARVISIBLE;
			hashtable[h].nom=pv->var ;
		    }
		    hh = h ;
		    assert((NUMERO) < (3+DIMENSION));
		    eg[NUMERO].existe = 1 ;
		    eg[NUMERO].colonne[eg[NUMERO].taille].numero=ligne ;
		    eg[NUMERO].colonne[eg[NUMERO].taille].num = pv->val ;
		    eg[NUMERO].colonne[eg[NUMERO].taille].den = VALUE_ONE ;
		    eg[NUMERO].taille++ ;
		} 
		else { valeur = value_uminus(pv->val); 
		       eg[0].existe = 1 ;
		       eg[0].colonne[eg[0].taille].numero=ligne ;
		       eg[0].colonne[eg[0].taille].num = valeur ;
		       eg[0].colonne[eg[0].taille].den = VALUE_ONE ;
		       eg[0].taille++ ;
		   }
	    }
	    /* Cas ou` valeur de variable est connue : */
	    if(j==1) hashtable[hh].val = valeur ;
	    DEBUG1(if(sc->egalites!=0)dump_tableau(eg,compteur));
	}
	else
	    ligne--;
    }
    
    /* Allocation a priori du tableau du simplex "t" par
     * colonnes. Soit
     * "dimension" le nombre de variables, la taille maximum
     * du tableau est de (1 + nb_ineq) lignes
     * et de (2 + dimension + nb_ineq + nb_eq) colonnes
     * On y ajoute en fait le double du nombre d'egalite's.
     * Ce tableau sera rempli de la facon suivante :
     * - ligne 0 : critere d'optimisation
     * - lignes 1 a nb_ineq : les inequations
     * - colonne 0 : le terme constant (composante de poids 1)
     * - colonne 1 : le terme constant (composante de poids M)
     * - colonnes 2 et suivantes : les elements initiaux
     *   et les termes d'ecart
     * Le tableau a une derniere colonne temporaire pour 
     *  pivoter un vecteur unitaire.
     */
    
    t = (tableau*)malloc((3 + NB_INEQ + NB_EQ + DIMENSION)*sizeof(tableau));
    for(i=0;i<(3 + NB_INEQ + NB_EQ + DIMENSION); i++) {
        t[i].colonne= (frac*) malloc((4 + 2*NB_EQ + NB_INEQ)*sizeof(frac)) ;
        t[i].existe = 0 ;
        t[i].taille = 1 ;
        t[i].colonne[0].numero = 0 ;
        t[i].colonne[0].num = VALUE_ZERO ;
    }
    nbvariables= 0 ;
    /* Initialisation de l'objectif */

    for(i=0;i<=1;i++) 
	objectif[i].num=VALUE_ZERO, objectif[i].den=VALUE_ONE;
	
    DEBUG(dump_hashtable(hashtable);)
	
    /* Entree des inegalites dans la table */
    
    for(pc=sc->inegalites, ligne=1; pc!=0; pc=pc->succ, ligne++) 
    {
	pv=pc->vecteur;
	if (pv!=NULL) /* skip if empty */
	{
	    valeur = VALUE_ZERO ;
	    poidsM = VALUE_ZERO ;
	    for(; pv !=0 ; pv=pv->succ) 
		if(vect_coeff(pv->var,sc_base(sc)))
		    value_addto(poidsM,pv->val) ;
		else valeur = value_uminus(pv->val) ; /* val terme const */

	    for(pv=pc->vecteur ; pv !=0 ; pv=pv->succ) {
		if(vect_coeff(pv->var,sc_base(sc))) {
		    h = hash((Variable)  pv->var) ; trouve=0 ;
		    while (hashtable[h].nom != 0)  {
			if (hashtable[h].nom==pv->var) {
			    trouve=1 ;
			    break ;
			}
			else { h = (h+1) % MAX_VAR ; }
		    }
		    if(!trouve) {
			hashtable[h].succ=premier_hash ;
			premier_hash = h ;
			hashtable[h].val = VALUE_NAN ;
			hashtable[h].numero=compteur++ ;
			hashtable[h].nom=pv->var ;
			CREVARVISIBLE ;
		    }
		    assert((NUMERO) < (3 + NB_INEQ + NB_EQ + DIMENSION));
		    if(value_neg_p(poidsM) || 
		       (value_zero_p(poidsM) && value_neg_p(valeur)))
			value_addto(t[NUMERO].colonne[0].num,pv->val),
			t[NUMERO].colonne[0].den = VALUE_ONE ;
		    t[NUMERO].existe = 1 ;
		    t[NUMERO].colonne[t[NUMERO].taille].numero=ligne ;
		    if(value_neg_p(poidsM) || 
		       (value_zero_p(poidsM) && value_neg_p(valeur)))
			tmpval = value_uminus(pv->val) ; 
		    else tmpval = pv->val ;
		    t[NUMERO].colonne[t[NUMERO].taille].num = tmpval ;
		    t[NUMERO].colonne[t[NUMERO].taille].den = VALUE_ONE ;
		    t[NUMERO].taille++ ;
		}
	    }
	    /* Creation de variable d'ecart ? */
	    if(value_neg_p(poidsM) ||
	       (value_zero_p(poidsM) && value_neg_p(valeur))) {
		DEBUG(dump_tableau(t, compteur);)
		i=compteur++ ;
		CREVARVISIBLE ;
		t[i].existe = 1 ; t[i].taille = 2 ;
		t[i].colonne[0].num = VALUE_ONE ;
		t[i].colonne[0].den = VALUE_ONE ;
		DEBUG(printf("ligne ecart = %ld, colonne %ld\n",ligne,i);)
		t[i].colonne[1].numero = ligne ;
		t[i].colonne[1].num = VALUE_MONE ;
		t[i].colonne[1].den = VALUE_ONE ;
		value_oppose(poidsM), 
		value_oppose(valeur) ;
		value_addto(objectif[0].num,valeur) ; 
		value_addto(objectif[1].num,poidsM) ;
	    }
	    /* Mise a jour des colonnes 0 et 1 */
	    t[0].colonne[t[0].taille].numero = ligne ;
	    t[0].colonne[t[0].taille].den = VALUE_ONE ;
	    t[0].colonne[t[0].taille].num = valeur ;
	    t[0].existe = 1 ;
	    t[0].taille++ ;
	    /* Element de poids M en 1ere colonne */
	    t[1].colonne[t[1].taille].numero = ligne ;
	    t[1].colonne[t[1].taille].num = poidsM ;
	    t[1].colonne[t[1].taille].den = VALUE_ONE ;
	    t[1].existe = 1 ;
	    t[1].taille++ ;
	    /* Creation d'une colonne cachee */
	    CREVARCACHEE ;
	    DEBUG(dump_tableau(t, compteur);)
	}
	else
	    ligne--;
    }

    DEBUG(dump_hashtable(hashtable));
    DEBUG1(dump_tableau(t, compteur));
    
    /* NON IMPLEMENTE' */
    
    /* Elimination de Gauss-Jordan dans le tableau "eg"
     *  Chaque variable a` eliminer est marquee
     *  eg[ ].existe = 2
     *  Si le processus d'elimination ne revele pas
     *  d'impossibilite', il est suivi du processus
     *  d'elimination dans les inegalites.
     */
    /* FIN DE NON IMPLEMENTE' */
    
    /* SOLUTION PROVISOIRE
     *  Pour chaque egalite on introduit
     *  2 inequations complementaires
     */
    
    for(pc=sc->egalites ; pc!=0; pc=pc->succ, ligne++)
    {
	/* Added by bc: do nothing for nul equalities */
	if (pc->vecteur != NULL)
	{
        valeur = VALUE_ZERO ;
        poidsM = VALUE_ZERO ;
        for(pv=pc->vecteur ; pv !=0 ; pv=pv->succ)
            if(vect_coeff(pv->var,sc_base(sc)))
                value_addto(poidsM,pv->val) ;
            else valeur = value_uminus(pv->val); /* val terme const */
        for(pv=pc->vecteur ; pv !=0 ; pv=pv->succ) {
            if(vect_coeff(pv->var,sc_base(sc))) {
                h = hash((Variable) pv->var) ; trouve=0 ;
                while (hashtable[h].nom != 0)  {
                    if (hashtable[h].nom==pv->var) {
                        trouve=1 ;
                        break ;
                    }
                    else { h = (h+1) % MAX_VAR ; }
                }
                if(!trouve) {
                    hashtable[h].succ=premier_hash ;
                    premier_hash = h ;
                    hashtable[h].val = VALUE_NAN ;
                    hashtable[h].numero=compteur++ ;
                    CREVARVISIBLE ;
                    hashtable[h].nom=pv->var ;
                }
		assert((NUMERO) < (3 + NB_INEQ + NB_EQ + DIMENSION));
                if(value_neg_p(poidsM) || 
		   (value_zero_p(poidsM) && value_neg_p(valeur)))
                    value_addto(t[NUMERO].colonne[0].num,pv->val),
                    t[NUMERO].colonne[0].den = VALUE_ONE ;
                t[NUMERO].existe = 1 ;
                t[NUMERO].colonne[t[NUMERO].taille].numero=ligne ;
                if(value_neg_p(poidsM) || 
		   (value_zero_p(poidsM) && value_neg_p(valeur)))
                    tmpval = value_uminus(pv->val); 
		else tmpval = pv->val ;
                t[NUMERO].colonne[t[NUMERO].taille].num = tmpval ;
                t[NUMERO].colonne[t[NUMERO].taille].den = VALUE_ONE ;
                t[NUMERO].taille++ ;
            }
        }
	/* Creation de variable d'ecart ? */
        if(value_neg_p(poidsM) || 
	   (value_zero_p(poidsM) && value_neg_p(valeur))) {
            i=compteur++ ;
            CREVARVISIBLE ;
            t[i].existe = 1 ; t[i].taille = 2 ;
            t[i].colonne[0].num = VALUE_ONE ;
            t[i].colonne[0].den = VALUE_ONE ;
            t[i].colonne[1].numero = ligne ;
            t[i].colonne[1].num = VALUE_MONE ;
            t[i].colonne[1].den = VALUE_ONE ;
            value_oppose(poidsM), 
	    value_oppose(valeur);
            value_addto(objectif[0].num,valeur) ;
            value_addto(objectif[1].num,poidsM) ;
        }
	/* Mise a jour des colonnes 0 et 1 */
        t[0].colonne[t[0].taille].numero = ligne ;
        t[0].colonne[t[0].taille].num = valeur ;
        t[0].colonne[t[0].taille].den = VALUE_ONE ;
        t[0].existe = 1 ;
        t[0].taille++ ;
	/* Element de poids M en 1ere colonne */
        t[1].colonne[t[1].taille].numero = ligne ;
        t[1].colonne[t[1].taille].num = poidsM ;
        t[1].colonne[t[1].taille].den = VALUE_ONE ;
        t[1].existe = 1 ;
        t[1].taille++ ;
	/* Creation d'une colonne cachee */
        CREVARCACHEE ;
	DEBUG(dump_tableau(t, compteur);)
        }
    }
    
    for(pc=sc->egalites ; pc!=0; pc=pc->succ, ligne++)
    {
	/* Added by bc: do nothing for nul equalities */
	if (pc->vecteur != NULL)
	{
        valeur = VALUE_ZERO ;
        poidsM = VALUE_ZERO ;
        for(pv=pc->vecteur ; pv !=0 ; pv=pv->succ)
            if(vect_coeff(pv->var,sc_base(sc)))
                value_substract(poidsM, pv->val) ;
            else valeur = pv->val ; /* val terme const */
        for(pv=pc->vecteur ; pv !=0 ; pv=pv->succ) {
            if (vect_coeff(pv->var,sc_base(sc))) {
                h = hash((Variable) pv->var) ; trouve=0 ;
                while (hashtable[h].nom != 0)  {
                    if (hashtable[h].nom==pv->var) {
                        trouve=1 ;
                        break ;
                    }
                    else { h = (h+1) % MAX_VAR ; }
                }
                if(!trouve) {
                    hashtable[h].succ=premier_hash ;
                    premier_hash = h ;
                    hashtable[h].val = VALUE_NAN ;
                    hashtable[h].numero=compteur++ ;
                    hashtable[h].nom=pv->var ;
                    CREVARVISIBLE ;
                }
		assert((NUMERO) < (3 + NB_INEQ + NB_EQ + DIMENSION));
                if(value_neg_p(poidsM) || 
		   (value_zero_p(poidsM) && value_neg_p(valeur)))
                    value_substract(t[NUMERO].colonne[0].num,pv->val),
                    t[NUMERO].colonne[0].den = VALUE_ONE ;
                t[NUMERO].existe = 1 ;
                t[NUMERO].colonne[t[NUMERO].taille].numero=ligne ;
                if(value_neg_p(poidsM) || 
		   (value_zero_p(poidsM) && value_neg_p(valeur)))
                    tmpval = pv->val ; 
		else tmpval = value_uminus(pv->val) ;
                t[NUMERO].colonne[t[NUMERO].taille].num = tmpval ;
                t[NUMERO].colonne[t[NUMERO].taille].den = VALUE_ONE ;
                t[NUMERO].taille++ ;
            }
        }
	/* Creation de variable d'ecart ? */
        if(value_neg_p(poidsM) || 
	   (value_zero_p(poidsM) && value_neg_p(valeur))) {
            i=compteur++ ;
            CREVARVISIBLE ;
            t[i].existe = 1 ; t[i].taille = 2 ;
            t[i].colonne[0].num = VALUE_ONE ;
            t[i].colonne[0].den = VALUE_ONE ;
            t[i].colonne[1].numero = ligne ;
            t[i].colonne[1].num = VALUE_MONE ;
            t[i].colonne[1].den = VALUE_ONE ;
            value_oppose(poidsM), 
	    value_oppose(valeur);
            value_addto(objectif[0].num,valeur) ;
            value_addto(objectif[1].num,poidsM) ;
        }
	/* Mise a jour des colonnes 0 et 1 */
        t[0].colonne[t[0].taille].numero = ligne ;
        t[0].colonne[t[0].taille].num = valeur ;
        t[0].colonne[t[0].taille].den = VALUE_ONE ;
        t[0].existe = 1 ;
        t[0].taille++ ;
	/* Element de poids M en 1ere colonne */
        t[1].colonne[t[1].taille].numero = ligne ;
        t[1].colonne[t[1].taille].num = poidsM ;
        t[1].colonne[t[1].taille].den = VALUE_ONE ;
        t[1].existe = 1 ;
        t[1].taille++ ;
	/* Creation d'une colonne cachee */
        CREVARCACHEE ;
	DEBUG(dump_tableau(t, compteur);)
        }
    }
    
    /* FIN DE SOLUTION PROVISOIRE */
    
    /* Algorithme du simplexe - methode primale simple.
     * L'objectif est d'etudier la faisabilite' d'un systeme
     * de contraintes sans trouver l'optimum.
     *   Les contraintes ont la forme : a x <= b
     *      et  d x = e
     * La methode de resolution procede comme suit :
     *
     *  1) Creer un tableau
     *       a  b
     *       d  e
     *     Eliminer autant de variables que posible par
     *    Gauss-Jordan
     *
     *  2) Travailler sur les inegalites seulement.
     *      Poser  x = x' - M 1
     *    ou` 1 est le vecteur de chiffres 1.
     *     Les inequations prennent alors la forme :
     *      a1 x <= b1 + M c1
     *      a2 x >= b2 + M c2
     *    avec c1 et c2 positifs
     *     On introduit les variables d'ecart y (autant que 
     *    d'inequations du 2eme type) et on cherche
     *      max{1(a2 x - y) | x,y >= 0 ; a1 x <= b1 + M c1 ;
     *                                a2 x - y <= b2 + M c2}
     *     On cree donc le tableau :
     *        0  0  1 a2     1  0  0
     *        b1 c1  a1      0  I  0
     *        b2 c2  a2     -I  0  I
     *
     *     On applique ensuite l'algorithme du simplex en
     *    se souvenant que c1 et c2 sont a multiplier par un
     *    infiniment grand.
     *     Si l'optimum est egal a (1 b2 , 1 c2), il y a une
     *    solution.
     *
     * Structures de donnees : on travaille sur des tableaux
     * de fractions rationnelles.
     */
    nlle_colonne=(frac *) malloc((4 + 2*NB_EQ + NB_INEQ)*sizeof(frac)) ;
    while(1) {

        /*  Recherche d'un nombre negatif 1ere ligne  */
        for(j=2, jj= -1 ;j<compteur;j++)
            if(t[j].existe && NEGATIF(t[j].colonne[0]))
            {  jj=j ; break ;
	   }
        /*  Terminaison  */
        if(jj == -1) { 
	    boolean cond;
            DEBUG1({
		printf ("solution :\n") ;
		dump_tableau(t, compteur) ;
		printf("objectif : "); printfrac(objectif[0]) ; 
			printfrac(objectif[1]) ; printf("\n") ;
	    });
	    
	    if (ofl_ctrl == FWD_OFL_CTRL)
		cond = EGALOFL(objectif[0],t[0].colonne[0]) &&
		    EGALOFL(objectif[1],t[1].colonne[0]);
	    else
		cond = EGAL(objectif[0],t[0].colonne[0]) &&
		    EGAL(objectif[1],t[1].colonne[0]);
	    
	    if(cond)
	    {
                DEBUG1(printf("Systeme soluble (faisable) en rationnels\n"));
		SOLUBLE(1)
	    }
	    else 
	    {
		DEBUG1(printf("Systeme insoluble (infaisable)\n"));
		SOLUBLE(0)
	    }
	    DEBUG1(printf("fin\n"));
        }
	DEBUG(printf("1 : jj= %ld\n",jj));
	DEBUG2(dump_tableau(t, compteur));
        /*  Recherche de la ligne de pivot  */
        METINFINI(min1) ; 
	METINFINI(min2) ;

        for(i=1, i0=1, i1=1, ii=-1 ; i<t[jj].taille ; i++)
        {
	    boolean cond;

            if(((i0<t[0].taille && t[jj].colonne[i].numero <= 
		 t[0].colonne[i0].numero)  || i0>=t[0].taille)
	       && ((i1<t[1].taille && t[jj].colonne[i].numero <=
		    t[1].colonne[i1].numero) || i1>=t[1].taille)) {
		if( POSITIF(t[jj].colonne[i])) {
		    frac f1 = 
			(i0<t[0].taille &&
			 t[jj].colonne[i].numero==t[0].colonne[i0].numero)?
			     t[0].colonne[i0]:frac0;
		    frac f2 = t[jj].colonne[i];
		    frac f3 =
			(i1<t[1].taille && 
			 t[jj].colonne[i].numero==t[1].colonne[i1].numero)?
			     t[1].colonne[i1]:frac0;
			
		    if (ofl_ctrl == FWD_OFL_CTRL) {
			DIVOFL(rapport1,f1,f2);
			DIVOFL(rapport2,f3,f2);
		    }
		    else {
			DIV(rapport1,f1,f2);
			DIV(rapport2,f1,f2);
		    }
    
		    if (ofl_ctrl == FWD_OFL_CTRL)
			cond = INFOFL(rapport2,min2) ||
			    (EGALOFL(rapport2,min2) && INFOFL(rapport1,min1));
		    else
			cond = INF(rapport2,min2) || 
			    (EGAL(rapport2,min2) && INF(rapport1,min1));
		    
		    if (cond) {
			AFF(min1,rapport1) ;
			AFF(min2,rapport2) ;
			AFF(pivot,t[jj].colonne[i]) ;
			ii=t[jj].colonne[i].numero ;
		    }
		} /* POSITIF(t[jj].colonne[i])) */
	    }
	    else {
		if(i0<t[0].taille && 
		   t[jj].colonne[i].numero> t[0].colonne[i0].numero) i0++ ;
		if(i1<t[1].taille && 
		   t[jj].colonne[i].numero > t[1].colonne[i1].numero) i1++ ;
	    }
	    
	    DEBUG(printf("i=%ld i0=%ld i1=%ld   %d %d %d\n",
			 i,i0,i1,
			 t[jj].colonne[i].numero,
			 t[0].colonne[i0].numero,
			 t[1].colonne[i1].numero));
        }
        /* Cas d'impossibilite'  */
	if(ii==-1) {
	    DEBUG1(dump_tableau(t, compteur);printf("Solution infinie\n"););
	    SOLUBLE(1)
	}

	/* Modification des numeros des variables */

        j = variables[jj-2];
	k = variablescachees[ii-1];
        variables[jj-2] = k;
	variablescachees[ii-1] = j;

        DEBUG2({
	    printf("Visibles :");
	    for(j=0;j<compteur-2;j++)
		printf(" %d",variables[j]);
	    printf("\nCachees :");
	    for(j=0;j<nbvariables;j++)
		printf(" %d",variablescachees[j]);
	    printf("\n");
	});

        /*  Pivot autour de la ligne ii / colonne jj
         * Dans ce qui suit, j = colonne courante,
         *  k = numero element dans la nouvelle colonne
         *     qui remplacera la colonne j,
         *  cc = element (colonne j, ligne ii)
         */
	DEBUG2(printf("Pivoter %ld %ld\n",ii,jj) ;);
        
	/* Remplir la derniere colonne temporaire de t
	 *   qui contient un 1 en position ligne ii
	 */
        t[compteur].taille = 2 ;
        t[compteur].colonne[0].num = VALUE_ZERO ;
        t[compteur].colonne[0].numero = 0 ;
        t[compteur].colonne[1].numero = ii;
        t[compteur].colonne[1].num = VALUE_ONE ;
        t[compteur].colonne[1].den = VALUE_ONE ;
        t[compteur].existe = 1 ;

        for(j=0 ; j<=compteur ; j=(j==(jj-1))?(j+2):(j+1)) {
	    if(t[j].existe)
	    {
		k=0 ;
		cc.num= VALUE_ZERO ; 
		cc.den= VALUE_ONE ;
		for(i=1;i<t[j].taille;i++)
		    if(t[j].colonne[i].numero==ii)
                    { AFF(cc,t[j].colonne[i]); break ; }
		    else if(t[j].colonne[i].numero>ii)
                    {cc.num= VALUE_ZERO ; cc.den=VALUE_ONE ; break ; }
		for(i=0,i1=0;i<t[j].taille || i1<t[jj].taille ;) {
		    if(i<t[j].taille &&  
		       i1<t[jj].taille && 
		       t[j].colonne[i].numero == t[jj].colonne[i1].numero) 
		    {   
			DEBUG(printf("k=%ld, j=%ld, i=%ld i1=%ld\n",k,j,i,i1);
			      printfrac(t[j].colonne[i]) ;
			      printfrac(t[jj].colonne[i1]) ;
			      printfrac(cc);
			      printfrac(pivot););

			if(t[j].colonne[i].numero == ii) {
			    AFF(nlle_colonne[k],t[j].colonne[i]);
			} else {
			    frac *n = &nlle_colonne[k],
                                 *a = &t[j].colonne[i],
                                 *b = &t[jj].colonne[i1];
			    if (ofl_ctrl == FWD_OFL_CTRL) {
				PIVOTOFL((*n), (*a), (*b), cc, pivot);
			    }
			    else {
				PIVOT((*n), (*a), (*b), cc, pivot);
			    }
			}
			if(i==0||nlle_colonne[k].num!=0) {
			    nlle_colonne[k].numero = t[j].colonne[i].numero ;
			    DEBUG(printfrac(nlle_colonne[k]) ;
				  printf(" ligne numero %d ", 
					 nlle_colonne[k].numero) ;
				  printf("\n") ;);
			    k++ ;
			}
			i++ ; i1++ ;
		    }
		    else
			if(i>=t[j].taille || 
			   (i1<t[jj].taille && 
			    t[j].colonne[i].numero > t[jj].colonne[i1].numero))
			{  
			    DEBUG(printf("t[j].colonne[i].numero >"
					 "t[jj].colonne[i1].numero ,"
					 "k=%ld, j=%ld, i=%ld i1=%ld\n",
					 k,j,i,i1);
				  printf("j = %ld  t[j].taille=%d ,"
					 "t[jj].taille=%d\n",
					 j,t[j].taille,t[jj].taille);
				  printf("t[j].colonne[i].numero=%d ,"
					 "t[jj].colonne[i1].numero=%d\n",
					  t[j].colonne[i].numero,
					 t[jj].colonne[i1].numero););

                        /* 0 en colonne j  ligne t[jj].colonne[i1].numero */
			    if(t[jj].colonne[i1].numero == ii) {
				AFF(nlle_colonne[k],frac0)
			    } else {
				frac *n = &(nlle_colonne[k]),
				     *b = &(t[jj].colonne[i1]);
				if(ofl_ctrl == FWD_OFL_CTRL)
				{
				    PIVOTOFL((*n),frac0,(*b),cc,pivot) ;
				}
				else 
				{
				    PIVOT((*n),frac0,(*b),cc,pivot) ;
				}
			    }

			    if(i==0||nlle_colonne[k].num!=0)
			    {
				nlle_colonne[k].numero = 
				    t[jj].colonne[i1].numero ;
				DEBUG(printfrac(nlle_colonne[k]);
				      printf(" ligne numero %d ",
					     nlle_colonne[k].numero);
				      printf("\n"););
				k++ ;
			    }
			    if(i1<t[jj].taille) i1++ ; else i++ ;
			}
			else
			    if(i1>=t[jj].taille || 
			       t[j].colonne[i].numero < 
			       t[jj].colonne[i1].numero)
			    {
				/* 0 en col jj, ligne t[j].colonne[i].numero */
				DEBUG(printf("t[j].colonne[i].numero <"
					     "t[jj].colonne[i1].numero ,"
					     "k=%ld, j=%ld, i=%ld i1=%ld\n",
					     k,j,i,i1);
				      printf("j = %ld  t[j].taille=%d ,"
					     "t[jj].taille=%d\n",
					     j,t[j].taille,t[jj].taille););
				AFF(nlle_colonne[k],t[j].colonne[i]) ;
				DEBUG(printfrac(nlle_colonne[k]));
				if(i==0||nlle_colonne[k].num!=0) {
				    nlle_colonne[k].numero = 
					t[j].colonne[i].numero ;
				    DEBUG(printf(" ligne numero %d \n", 
						 nlle_colonne[k].numero));
				    k++ ;
				}
				if(i<t[j].taille) i++ ; else i1++ ;
			    }

		}
		if(j==compteur) w = jj ; else w = j ;
		colo = t[w].colonne ;
		t[w].colonne=nlle_colonne ;
		nlle_colonne = colo ;
		t[w].taille=k ;
		DEBUG(printf("w = %ld  t[w].taille=%d \n",w,t[w].taille);
		      dump_tableau(t, compteur););
	    }
        }
    }

    /* Restauration des entrees vides de la table hashee  */
    FINSIMPLEX :
	for(i=premier_hash ; i!=(int)PTR_NIL; i=hashtable[i].succ)
	    hashtable[i].nom = 0 ;
    DEBUG2(dump_tableau(t, compteur));

	if (NB_EQ > 0) {
	    for(i=0 ; i<(3+DIMENSION) ; i++)
		free(eg[i].colonne);
	    free(eg);
	}
	
	for(i=0;i<(3 + NB_INEQ + NB_EQ + DIMENSION); i++) 
	    free(t[i].colonne);
	free(t);
	free(nlle_colonne);
	return (soluble) ;
    }
}     /* main */

/* (that is all, folks!:-)
 */
