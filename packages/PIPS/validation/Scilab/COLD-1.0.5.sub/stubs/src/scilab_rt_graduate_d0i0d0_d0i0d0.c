
#include <stdio.h>

#include <stdlib.h>

void scilab_rt_graduate_d0i0d0_d0i0d0(double scalarin0, 
    int scalarin1, 
    double scalarin2,
     double* scalarout0, 
    int* scalarout1, 
    double* scalarout2)
{
  printf("%f", scalarin0);

  printf("%d", scalarin1);

  printf("%f", scalarin2);

  *scalarout0 = rand();

  *scalarout1 = rand();

  *scalarout2 = rand();

}
