#define sizeN 1024
#define sizeM 1024
#define z(i,j) (i)*sizeM+(j)
#include <stdio.h>
#include <stdlib.h>

float Gx[sizeN*sizeM], Gy[sizeN*sizeM], Ixx[sizeN*sizeM], Iyy[sizeN*sizeM], Ixy[sizeN*sizeM], Sxx[sizeN*sizeM], Sxy[sizeN*sizeM], Syy[sizeN*sizeM], in[sizeN*sizeM], out[sizeN*sizeM];

void SobelX(float GX[sizeN*sizeM], float X[sizeN*sizeM]){
   unsigned int i,j;
   for(i = 1; i < sizeN - 1; i++)
     for(j = 1; j < sizeM - 1; j++){
      GX[z(i,j)] = (X[z(i-1,j-1)]-X[z(i-1,j+1)]+2*(X[z(i,j-1)]-X[z(i,j+1)])+X[z(i+1,j-1)]-X[z(i+1,j+1)])/8;
     }
}

void SobelY(float GY[sizeN*sizeM], float X[sizeN*sizeM]){
   unsigned int i,j;
   for(i = 1; i < sizeN - 1; i++)
     for(j = 1; j < sizeM - 1; j++){
      GY[z(i,j)] = (X[z(i-1,j-1)]-X[z(i+1,j-1)]+2*(X[z(i-1,j)]-X[z(i+1,j)])+X[z(i-1,j+1)]+X[z(i+1,j+1)])/8;
     }
}

void Gauss(float S[sizeN*sizeM], float X[sizeN*sizeM]){
  unsigned int i,j;
  for(i = 1; i < sizeN - 1; i++)
   for(j = 1; j < sizeM - 1; j++)
     S[z(i,j)] = (X[z(i-1,j-1)]+2*X[z(i-1,j)]+X[z(i-1,j+1)]+2*(X[z(i,j-1)]+2*X[z(i,j)]+X[z(i,j+1)])+X[z(i+1,j-1)]+X[z(i+1,j)]+X[z(i+1,j+1)])/16;
}

void MultiplY(float M[sizeN*sizeM],float X[sizeN*sizeM],float Y[sizeN*sizeM]){
  unsigned int i,j;
  for(i = 0; i < sizeN; i++)
   for(j = 0; j < sizeM; j++)
     M[z(i,j)] = X[z(i,j)]*Y[z(i,j)];
}


void CoarsitY(float K[sizeN*sizeM], float X[sizeN*sizeM],float Y[sizeN*sizeM],float Z[sizeN*sizeM]){
  unsigned int i,j;
  for(i = 0; i < sizeN; i++)
   for(j = 0; j < sizeM; j++)
     K[z(i,j)] = X[z(i,j)]*Y[z(i,j)]-Z[z(i,j)]*Z[z(i,j)];
}

void  InitHarris(float in[sizeN*sizeM]){
  unsigned int i,j;
  for(i = 0 ; i < sizeN ; i++)
    for(j = 0 ; j < sizeM ; j=j+3){
      in[z(i,j)] = 5; /* A dummy initialisation, we could load a true image instead */
      in[z(i,j+1)] = 3;in[z(i,j+2)] = 7;
    }  
}
void main(int argc, char *argv[]){
 InitHarris(in);
 /* Now we run the Harris procedure */
 //Sobel
 SobelX(Gx, in);
 SobelY(Gy, in);
 //Multiply
 MultiplY(Ixx, Gx, Gx);
 MultiplY(Iyy, Gy, Gy);
 MultiplY(Ixy, Gx, Gy);
 //Gauss
 Gauss(Sxx, Ixx);
 Gauss(Syy, Iyy);
 Gauss(Sxy, Ixy);
 //Coarsity
 CoarsitY(out, Sxx, Syy, Sxy);
}



