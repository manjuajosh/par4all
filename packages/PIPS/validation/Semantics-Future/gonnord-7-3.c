// Laure Gonnord: Accélération abstraite pour l'amélioration de la précision en
// Analyse des Relations Linéaires
// figure 7.3

// $Id$

#define USE_ASSERT 0
#define USE_CHECK 1
#define RESTRUCTURE 1
#define GOOD (x >= 0 && z >= 0 && z <= 10 && z <= x)

// usefull stuff

#include <stdlib.h>

int flip(void) {
	return rand() % 2;
}

void assert_error(void) {
	exit(1);
}

void checking_error(void) {
	exit(2);
}

#if USE_ASSERT == 0
#define assert(e)
#else
#define assert(e) {if (!(e)) assert_error();}
#endif

#if USE_CHECK == 0
#define check(e)
#define check_not(e)
#else
#define check(e) {if (!(e)) checking_error();}
#define check_not(e) {if (e) checking_error();}
#endif

#define freeze {while(1);}

#ifdef GOOD
#define trans(g, c) {if (g) {c; check(GOOD);} else freeze;}
#else
#ifdef BAD
#define trans(g, c) {if (g) {c; check_not(BAD);} else freeze;}
#else
#define trans(g, c) {if (g) {c;} else freeze;}
#endif
#endif

// transition system

#define G1 (z <= 9)
#define G1a (z <= 8)
#define G1b (z == 9)
#define U1 {x++; z++;}
#define T1 {trans(G1, U1);}
#define T1a {trans(G1a, U1);}
#define T1b {trans(G1b, U1);}

#define G2 (z == 10)
#define U2 {z = 0;}
#define T2 {trans(G2, U2);}

#define S1 {assert(z <= 9);}
#define S2 {assert(z == 10);}

void run(void) {
	
	int x, z;
	x = z = 0;
	
#if RESTRUCTURE == 0
	
	while (1) {
		if (flip()) {
			T1;
		}
		else {
			T2;
		}
	}

#else
	
	S1;
	while (1) {
		while (flip()) {
			T1a; S1;
		}
		while (flip()) {
			T1b; S2;
			T2; S1;
		}
	}
	
#endif
	
}

int main(void) {
	run();
	return 0;
}

