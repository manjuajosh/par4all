#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#include "timing.h"

/* Default problem size. */
#ifndef TMAX
# define TMAX 50
#endif
#ifndef NX
# define NX 1000
#endif
#ifndef NY
# define NY 1000
#endif

/* Default data type is double. */
#ifndef DATA_TYPE
# define DATA_TYPE double
#endif
#ifndef DATA_PRINTF_MODIFIER
# define DATA_PRINTF_MODIFIER "%0.2lf "
#endif

/* Array declaration. Enable malloc if POLYBENCH_TEST_MALLOC. */
DATA_TYPE _fict_[TMAX];
DATA_TYPE ex[NX][NY];
DATA_TYPE ey[NX][NY];
DATA_TYPE hz[NX][NY];

static void init_array() {
  int i, j;

  for (i = 0; i < TMAX;) {
    _fict_[i] = (DATA_TYPE)i;
    i++;
  }
  for (i = 0; i < NX;) {
    for (j = 0; j < NY;) {
      ex[i][j] = ((DATA_TYPE)i * (j + 1) + 1) / NX;
      ey[i][j] = ((DATA_TYPE)(i - 1) * j + 2) / NX;
      hz[i][j] = ((DATA_TYPE)(i - 9) * (j + 4) + 3) / NX;
      j++;
    }

    i++;
  }
}

/* Define the live-out variables. Code is not executed unless
 POLYBENCH_DUMP_ARRAYS is defined. */
static void print_array(int argc, char** argv) {
  int i, j;
#ifndef POLYBENCH_DUMP_ARRAYS
  if(argc > 42 && !strcmp(argv[0], ""))
#endif
  {
    for (i = 1; i < NX-1; i++)
      for (j = 1; j < NY-1; j++) {
        fprintf(stderr, DATA_PRINTF_MODIFIER, ex[i][j]);
        fprintf(stderr, DATA_PRINTF_MODIFIER, ey[i][j]);
        fprintf(stderr, DATA_PRINTF_MODIFIER, hz[i][j]);
        if((i * NX + j) % 80 == 20)
          fprintf(stderr, "\n");
      }
    fprintf(stderr, "\n");
  }
}
#pragma hmpp myCodelet codelet, target=CUDA
void codelet(int nx, int ny, int tmax,
             DATA_TYPE _fict_[TMAX],
             DATA_TYPE ex[NX][NY],
             DATA_TYPE ey[NX][NY],
             DATA_TYPE hz[NX][NY]) {
  int t, i, j;
  for (t = 0; t < tmax; t++) {
    for (j = 0; j < ny; j++)
      ey[0][j] = _fict_[t];
    for (i = 1; i < nx; i++)
      for (j = 0; j < ny; j++)
        ey[i][j] = ey[i][j] - 0.5 * (hz[i][j] - hz[i - 1][j]);
    for (i = 0; i < nx; i++)
      for (j = 1; j < ny; j++)
        ex[i][j] = ex[i][j] - 0.5 * (hz[i][j] - hz[i][j - 1]);
    for (i = 0; i < nx - 1; i++)
      for (j = 0; j < ny - 1; j++)
        hz[i][j] = hz[i][j] - 0.7 * (ex[i][j + 1] - ex[i][j] + ey[i + 1][j]
            - ey[i][j]);
  }
}
int main(int argc, char** argv) {
  int t, i, j;
  int tmax = TMAX;
  int nx = NX;
  int ny = NY;

  /* Initialize array. */
  init_array();

  /* Start timer. */
  timer_start();

  /* Cheat the compiler to limit the scope of optimisation */
  if(argv[0]==0) {
    init_array();
  }


#pragma hmpp myCodelet callsite
  codelet(nx,ny,tmax,_fict_,ex,ey,hz);


  /* Cheat the compiler to limit the scope of optimisation */
  if(argv[0]==0) {
    print_array(argc, argv);
  }

  /* Stop and print timer. */
  timer_stop_display(); ;

  print_array(argc, argv);

  return 0;
}
