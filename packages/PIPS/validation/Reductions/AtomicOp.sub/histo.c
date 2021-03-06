#include <string.h>
#include <math.h>

void atomicAddInt(int *target, int increment) {
  *target += increment;
}
#define NPART 10000000

void histogram(int data[128][128][128], int histo[128][128][128])
{
   int i, j, k;
   memset(histo, 0, 128*128*128*sizeof(int));
      
   // This is parallel with a reduction on elements of histo
   for(i = 0; i <= 127; i += 1) {
     for(j = 0; j <= 127; j += 1) {
       for(k = 0; k <= 127; k += 1) {
         int x = floor((float) data[i][j][k]/((float) (128*128)));
         int y = floor((float) (data[i][j][k]-x*128*128)/((float) 128));
         int z = data[i][j][k]-x*128*128-y*128;
         ++histo[x][y][z];
       }
     }
   }

  // This is parallel with a reduction on the scalar s
  int s = 0;
  for(i = 0; i <= 127; i += 1) {
    for(j = 0; j <= 127; j += 1) {
      for(k = 0; k <= 127; k += 1) {
        s+=data[i][j][k];
      }
    }
  }

  // This is parallel without a reduction
  for(i = 0; i <= 127; i += 1) {
    for(j = 0; j <= 127; j += 1) {
      for(k = 0; k <= 127; k += 1) {
        data[i][j][k]=data[i][j][k]+1;
      }
    }
  }
}

