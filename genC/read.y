%{
/*

	-- NewGen Project

	The NewGen software has been designed by Remi Triolet and Pierre
	Jouvelot (Ecole des Mines de Paris). This prototype implementation
	has been written by Pierre Jouvelot.

	This software is provided as is, and no guarantee whatsoever is
	provided regarding its appropriate behavior. Any request or comment
	should be sent to newgen@isatis.ensmp.fr.

	(C) Copyright Ecole des Mines de Paris, 1989

*/


/* read.y 

   The syntax of objects printed by GEN_WRITE. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "newgen_include.h"
#include "genC.h"

extern FILE * yyin;

/* This constant should be adapted to the particular need of the application */

#define YYMAXDEPTH 1000

/* User selectable options. */

int warn_on_ref_without_def_in_read_tabulated = FALSE;

/* Where the root will be. */

chunk *Read_chunk ;

/* The SHARED_TABLE maps a shared pointer number to its chunk pointer value. */

static chunk **shared_table ;
static int shared_number ;

/* The GEN_TABULATED_NAMES hash table maps ids to index in the table of
   the tabulated domains. In case of multiple definition, if the previous
   value is negative, then it came from a REF (by READ_TABULATED) and
   no error is reported. */

hash_table Gen_tabulated_names = (hash_table)NULL ;

/* Management of forward references in read */

int allow_forward_ref = FALSE ;

static char *read_external() ;
static chunk *make_def(), *make_ref() ;
%}

%token CHUNK_BEGIN
%token VECTOR_BEGIN
%token ARROW_BEGIN
%token READ_BOOL
%token LP
%token RP
%token LC
%token RC
%token LB
%token SHARED_POINTER
%token READ_EXTERNAL
%token READ_DEF
%token READ_REF
%token READ_NULL

%token READ_LIST_UNDEFINED
%token READ_SET_UNDEFINED
%token READ_ARRAY_UNDEFINED

%term READ_UNIT
%term READ_CHAR
%term READ_INT
%term READ_FLOAT
%term READ_STRING

%union {
  chunk chunk ;
  chunk *chunkp ;
  cons *consp ;
  int val ;
}

%type <chunk> Data Basis 
%type <chunkp> Chunk String
%type <consp> Sparse_Datas Datas
%type <val> Int Shared_chunk Type

%%
Read	: Nb_of_shared_pointers Chunk {
		Read_chunk = $2 ;
		free( shared_table ) ;
		YYACCEPT ;
                /*NOTREACHED*/
		}
	;

Nb_of_shared_pointers 
  	: Int 	{
		shared_table = (chunk **)alloc( $1*sizeof( chunk * )) ;
		}
	;

Chunk 	: Shared_chunk CHUNK_BEGIN Type Datas RP {
  		int i ;
		cons *cp ;
		int length = gen_length( $4 ) ;

		$$ = ($1) ? shared_table[ $1-1 ]:
			(chunk *)alloc( (HEADER+length)*sizeof( chunk )) ;
		$$->i = $3 ;

		for( i=0, cp=gen_nreverse( $4 ); i<length ; i++, cp = cp->cdr )
			*($$+1+i) = cp->car ;
#ifdef DBG_READ
		write_chunk( stderr, $$, HEADER+length ) ;
#endif
		}
	;

Shared_chunk
	: LB Int {
		$$ = shared_number = $2 ;
		}
	|	{
		$$ = shared_number = 0 ;
		}
	;

Type	: Int	{
		if( shared_number ) {
			struct binding *bp = &Domains[ $1 ] ;

			shared_table[ shared_number-1 ] = 
				(chunk *)alloc(gen_size( bp )*
					       sizeof( chunk )) ;
		}
		$$ = $1 ;
		}
	;

Datas	: Datas Data {	
	        $$ = CONS( CHUNK, $2.p, $1 ) ;
		}
	| 	{
		$$ = NIL ;
		}
	;

Sparse_Datas	
	: Sparse_Datas Int Data {	
	        $$ = CONS(CONSP, 
			  CONS( INT, $2, CONS( CHUNK, $3.p, NIL)), 
			  $1 ) ;
		}
	| 	{
		$$ = NIL ;
		}
	;

Data	: Basis	{
                $$ = $1 ;
		}
        | READ_LIST_UNDEFINED {
	        $$.l = list_undefined ;
	        }
	| LP Datas RP {
		$$.l = gen_nreverse( $2 ) ;
		}
        | READ_SET_UNDEFINED {
	        $$.t = set_undefined ;
	    }
        | LC Int Datas RC {
	        $$.t = set_make( $2 ) ;
		MAPL( cp, {
		  switch( $2 ) {
		  case set_int:
		    set_add_element( $$.t, $$.t, (char *)cp->car.i ) ;
		    break ;
		  default:
		    set_add_element( $$.t, $$.t, cp->car.s ) ;
		    break ;
		  }}, $3 ) ;
		gen_free_list( $3 ) ;
	        }
        | READ_ARRAY_UNDEFINED {
	        $$.p = array_undefined ;
	        }
	| VECTOR_BEGIN Int Sparse_Datas RP {
                chunk *kp ;
		cons *cp ;
		int i ;

		kp = (chunk *)alloc( $2*sizeof( chunk )) ;

		for( i=0 ; i != $2 ; i++ ) {
		    kp[ i ].p = chunk_undefined ;
		}
		for( cp=$3 ; cp!=NULL ; cp=cp->cdr ) {
		    cons *pair = CONSP( CAR( cp )) ;
		    
		    kp[ INT(CAR(pair)) ] = CAR(CDR(pair)) ;
		}
		gen_free_list( $3 ) ;
                $$.p = kp ;
		}
	| ARROW_BEGIN Datas RP {
		hash_table h = hash_table_make( hash_chunk, 0 )	;
		cons *cp ;

		for( cp = gen_nreverse($2) ; cp != NULL ; cp=cp->cdr->cdr ) {
			chunk *k = (chunk *)alloc( sizeof( chunk )) ;
			chunk *v = (chunk *)alloc( sizeof( chunk )) ;
	
			*k = CAR(  cp ) ;
			*v = CAR( CDR( cp )) ;
			hash_put( h, (char *)k, (char *)v ) ;
		}
		gen_free_list( $2 ) ;
		$$.h = h ;
		}
	| Chunk {
		$$.p = $1 ;
		}
	| SHARED_POINTER Int {
		$$.p = shared_table[ $2-1 ] ;
		}
	;
  
Basis	: READ_UNIT 	{
	        $$.u = 1 ;
		}
	| READ_BOOL Int {
		$$.b = $2 ;
		}
	| READ_CHAR	{
		$$.c = *(xxtext+2) ;
		}
	| Int	{
		$$.i = $1 ;
		}
	| READ_FLOAT {
		$$.f = (double)atof( xxtext ) ;
		}
	| String {
	        $$ = *$1 ;
		}
 	| READ_EXTERNAL Int {
		$$.s = read_external( $2 ) ;
		}
	| READ_DEF Int String Chunk {
	        $$.p = make_def( $2, $3, $4 ) ;
	        }
	| READ_REF Int String {
	        $$.p = make_ref( $2, $3 ) ;
	        }
	| READ_NULL {
		$$.p = chunk_undefined ;
		}
	;

Int     : READ_INT   {
  		$$ = atoi( xxtext ) ;
		}
	;

String  : READ_STRING {
		extern char literal[] ;
		char *p = alloc( strlen( literal ) + 1) ;
		chunk *obj = (chunk *)alloc( sizeof( chunk )) ;

		strcpy( p, literal ) ; 
		literal[ 0 ] = '\0' ;
		obj->s = p ;
		$$ = obj ;
	    }
		    
%%

/* YYERROR manages a syntax error while reading an object. */

yyerror( s )
char *s ;
{
  int c;

  fprintf( stderr, "%s before ", s ) ;
	
  while( (c=yyinput()) != EOF )
    fprintf( stderr, "%c", c ) ;

  fatal( "Incorrect object written by GEN_WRITE\n", (char *)NULL ) ;
}

/* READ_EXTERNAL reads external types on stdin */

static char *
read_external( which )
int which ;
{
    struct binding *bp = &Domains[ which ] ;
    union domain *dp = bp->domain ;
    extern int yyinput() ;

    if( dp->ba.type != EXTERNAL ) {
	fatal( "gen_read: undefined external %s\n", bp->name ) ;
	/*NOTREACHED*/
    }
    if( dp->ex.read == NULL ) {
	user( "gen_read: uninitialized external %s\n", bp->name ) ;
	return( NULL ) ;
    }
    if( yyinput() != ' ' ) {
	fatal( "read_external: white space expected\n", (char *)NULL ) ;
	/*NOTREACHED*/
    }
    /*
      Attention, ce qui suit est absolument horrible. Les fonctions
      suceptibles d'etre  appelees a cet endroit sont:
      - soit des fonctions 'user-written' pour les domaines externes
      non geres par NewGen
      - soit la fonctions gen_read pour les domaines externes geres
      par NewGen 
      
      Dans le 1er cas, il faut passer la fonction de lecture d'un caractere
      (yyinput) a la fonction de lecture du domaine externe (on ne peut pas
      passer le pointeur de fichier car lex bufferise les caracteres en
      entree). Dans le second cas, il faut passer le pointeur de fichier a
      cause de yacc/lex.
      
      Je decide donc de passer les deux parametres: pointeur de fichier et
      pointeur de fonction de lecture. Dans chaque cas, l'un ou l'autre sera
      ignore. 
      */
    return( (*(dp->ex.read))( yyin, yyinput )) ;
}


/* ENTER_TABULATED_DEF enters a new definition (previous refs are allowed if
   ALLOW_REF) in the INDEX tabulation table of the DOMAIN, with the unique
   ID and value CHUNKP. */

chunk *
enter_tabulated_def( index, domain, id, chunkp, allow_ref )
int index  ;
int domain ;
char *id ;
chunk *chunkp ;
int allow_ref ;
{
    static char local[ 1024 ] ;
    chunk *hash ;
    
    sprintf( local, "%d%c%s", domain, HASH_SEPAR, id ) ;

    if( Gen_tabulated_[ index ] == (chunk *)NULL ) {
	fatal( "enter_tabulated_def: Uninitialized %s\n", 
	       Domains[ domain ].name ) ;
    }
    if( Gen_tabulated_names == NULL ) {
	fatal( "enter_tabulated_def: Gen_tabulated_names on %s\n", 
	       Domains[ domain ].name ) ;
    }
    if((hash=(chunk *)hash_get( Gen_tabulated_names, local )) != 
       (chunk *)HASH_UNDEFINED_VALUE ) {
	if( allow_ref && hash->i < 0 ) {
	    int i, size = gen_size( Domains+domain ) ;
	    chunk *cp, *gp ;

	    hash->i = -hash->i ;

	    if( (gp=(Gen_tabulated_[ index ]+hash->i)->p) == NULL ) {
		fatal( "make_def: Null for %s\n", local ) ;
	    }
	    for( cp=chunkp, i=0 ; i<size ; i++ ) {
		*gp++ = *cp++ ;
	    }
	    ((Gen_tabulated_[ index ]+hash->i)->p+1)->i = hash->i ;
	    return( (Gen_tabulated_[ index ]+hash->i)->p ) ;
	} 
	else {
	    if (hash_warn_on_redefinition_p()) {
		user("Tabulated entry %s already defined: updating\n",
		     local );
	    }
	}
    }
    else {
	char *new_key = alloc( strlen( local )+1 ) ;

	strcpy( new_key, local ) ;
	hash = (chunk *)alloc( sizeof( chunk )) ;
	hash->i = find_free_tabulated( &Domains[ domain ] ) ;
	hash_put( Gen_tabulated_names, new_key, (char *)hash ) ;
    }
    (Gen_tabulated_[ index ]+hash->i)->p = chunkp ;
    (chunkp+1)->i = hash->i ;
    return( chunkp ) ;
}

/* MAKE_DEF defines the object CHUNK of name STRING to be in the tabulation 
   table INT. */

static chunk *
make_def( Int, String, Chunk )
int Int ;
chunk *String, *Chunk ;
{
    int domain ;
    char *id ;

    sscanf( String->s, "%d", &domain ) ;
    id = strchr( String->s, HASH_SEPAR )+1 ;

    return( enter_tabulated_def( Int, domain, id, Chunk, allow_forward_ref )) ;
}

/* MAKE_REF references the object of hash name STRING in the tabulation table
   INT. Forward references are dealt with here. */

static chunk *
make_ref( Int, String )
int Int ;
chunk *String ;
{
    chunk *hash ;
    chunk *cp ;
    int domain ;

    if( Gen_tabulated_[ Int ] == (chunk *)NULL ) {
	user( "read: Unloaded tabulated domain %s\n", Domains[ Int ].name ) ;
    }
    sscanf( String->s, "%d", &domain ) ;

    if( (hash=(chunk *)hash_get( Gen_tabulated_names, String->s ))
       == (chunk *)HASH_UNDEFINED_VALUE ) {
	if( allow_forward_ref ) {
	    hash = (chunk *)alloc( sizeof( chunk )) ;
	    hash->i = -find_free_tabulated( &Domains[ domain ] ) ;
	    hash_put( Gen_tabulated_names, String->s, (char *)hash ) ;

	    if((Gen_tabulated_[ Int ]+abs( hash->i ))->p != chunk_undefined) {
	        fatal("make_ref: trying to re-allocate for %s\n", String->s) ;
	    }
	    (Gen_tabulated_[ Int ]+abs( hash->i ))->p = 
		(chunk *)alloc( gen_size( Domains+domain )* sizeof( chunk )) ;
        }
	else {
	    user("make_ref: Forward references to %s prohibited\n",
		 String->s) ;
        }
    }
    cp = (Gen_tabulated_[ Int ]+abs( hash->i ))->p ;
    (cp+1)->i = abs( hash->i ) ;
    return( cp ) ;
}
