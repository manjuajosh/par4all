#include <stdio.h>

int main () {
  float a, b, c, d, e;
  a = 1.0;
  b = 100.0E+25;
  c = b;
  e = a + c - b;
  d = a + (c - b);
  printf ("a + c - b = %e\na + (c - b)= %e\n", e, d);
  return (int) d;
}
