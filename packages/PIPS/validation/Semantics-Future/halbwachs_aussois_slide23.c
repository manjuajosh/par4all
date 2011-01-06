// Example of Linear Relation Analysis given by Nicolas Halbwachs in his great
// tutorial at Aussois, 9/12/2010
// http://compilation.gforge.inria.fr/2010_12_Aussois/programpage/programme.html

// $Id$

#define USE_ASSERT 0
#define GOOD (ok == 1)

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

#define check(e) {if (!(e)) checking_error();}
#define check_not(e) {if (e) checking_error();}
#define freeze {while(1);}

// transition system

#define trans(g, c) {if (g) {c; check(GOOD);} else freeze;}

#define xor(a, b) ((a == 0 && b == 1) || (a == 1 && b == 0))

#define G1 (b0 == b1 && !xor(b0, b1))
#define C1 {b_tmp = b0; b0 = 1 - b1; b1 = b_tmp; x++; if (ok == 1 && x >= y) {ok = 1;} else {ok = 0;}}
#define T1 {trans(G1, C1);}

#define G2 (b0 != b1 && xor(b0, b1))
#define C2 {b_tmp = b0; b0 = 1 - b1; b1 = b_tmp; y++; if (ok == 1 && x >= y) {ok = 1;} else {ok = 0;}}
#define T2 {trans(G2, C2);}

#define S1 {assert(G1);}
#define S2 {assert(G2);}

void run(void) {
	int b_tmp, b0, b1, ok, x, y;
	b0 = b1 = x = y = 0;
	ok = 1;
	
	while (1) {
		S1;
		T1;
		S2;
		T2;
	}
	
}

int main(void) {
	run();
	return 0;
}

