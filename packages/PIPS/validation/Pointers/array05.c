
#include <stdio.h>

int main() {
  int t[100][10][3];

  int (*p)[3];
  int *pu;

  int (*q)[10][3];
  int (*qu)[3];

  int (*r)[100][10][3];
  int (*ru)[10][3];


  p = &t[1][2];
  (*p)[1] = 0;
  p[1][1] = 0;

  pu = &t[1][2][0];
  *pu = 0;
  pu[2] = 0;

  q = &t[1];
  (*q)[2][3] = 0;
  q[1][2][3] = 0;

  qu = &t[1][0];
  (*qu)[1] = 0;
  qu[1][1] = 0;

  r = &t;
  (*r)[0][0][0] = 0;
  (*r)[1][2][3] = 0;

  ru = &t[0];
  (*ru)[0][0] = 0;
  ru[0][0][0] = 0;
  ru[1][2][1] = 0;

  return 0;
}
