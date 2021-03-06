#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#include "instrument.h"

/* Default problem size. */
#ifndef TSTEPS
# define TSTEPS 20
#endif
#ifndef N
# define N 1000
#endif

/* Default data type is double. */
#ifndef DATA_TYPE
# define DATA_TYPE double
#endif
#ifndef DATA_PRINTF_MODIFIER
# define DATA_PRINTF_MODIFIER "%0.2lf "
#endif

/* Array declaration. Enable malloc if POLYBENCH_TEST_MALLOC. */
#ifndef POLYBENCH_TEST_MALLOC
DATA_TYPE A[N][N];
#else
DATA_TYPE** A = (DATA_TYPE**)malloc(N * sizeof(DATA_TYPE*));
{
  int i;
  for (i = 0; i < N; ++i)
    A[i] = (DATA_TYPE*)malloc(N * sizeof(DATA_TYPE));
}
#endif

static inline
void init_array()
{
  int i, j;

  for (i = 0; i < N; i++)
    for (j = 0; j < N; j++)
      A[i][j] = ((DATA_TYPE) (i-3)*j + 10) / N;
}

/* Define the live-out variables. Code is not executed unless
   POLYBENCH_DUMP_ARRAYS is defined. */
static inline
void print_array(int argc, char** argv)
{
  int i, j;
#ifndef POLYBENCH_DUMP_ARRAYS
  if (argc > 42 && ! strcmp(argv[0], ""))
#endif
    {
      for (i = 0; i < N; i++)
	for (j = 0; j < N; j++) {
	  fprintf(stderr, DATA_PRINTF_MODIFIER, A[i][j]);
	  if ((i * N + j) % 80 == 20) fprintf(stderr, "\n");
	}
      fprintf(stderr, "\n");
    }
}


int main(int argc, char** argv)
{
  int t, i, j;
  int tsteps = TSTEPS;
  int n = N;

  /* Initialize array. */
  init_array();

  /* Start timer. */
  polybench_start_instruments;

#pragma scop
#pragma live-out A

  for (t = 0; t <= tsteps - 1; t++)
    for (i = 1; i<= n - 2; i++)
      for (j = 1; j <= n - 2; j++)
	A[i][j] = (A[i-1][j-1] + A[i-1][j] + A[i-1][j+1]
		   + A[i][j-1] + A[i][j] + A[i][j+1]
		   + A[i+1][j-1] + A[i+1][j] + A[i+1][j+1])/9.0;

#pragma endscop

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  print_array(argc, argv);

  return 0;
}
