
#include <stdio.h>

void scilab_rt_hist3d_i2i0i0s0_(int in00, int in01, int matrixin0[in00][in01], 
    int scalarin0, 
    int scalarin1, 
    char* scalarin2)
{
  int i;
  int j;

  int val0 = 0;
  for (i = 0; i < in00; ++i) {
    for (j = 0; j < in01; ++j) {
      val0 += matrixin0[i][j];
    }
  }
  printf("%d", val0);

  printf("%d", scalarin0);

  printf("%d", scalarin1);

  printf("%s", scalarin2);

}
