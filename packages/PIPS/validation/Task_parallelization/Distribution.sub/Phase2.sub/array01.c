//init a and first column of b
//init b and all cluster use the first column for that
//transform 2D array (b) into 1D array (c)

int main(int toto)
{
   // Generated by Pass VARIABLE_REPLICATION
   int __i_0, __i_1, __i_2, __i_3;
   // Generated by Pass VARIABLE_REPLICATION
   int __a_0[4], __a_1[4], __a_2[4], __a_3[4];
   // Generated by Pass VARIABLE_REPLICATION
   int __b_0[4][5], __b_1[4][5], __b_2[4][5], __b_3[4][5];
#pragma distributed on_cluster=0
   {
      for(__i_0 = 0; __i_0 <= 3; __i_0 += 1) {
         __a_0[__i_0] = __i_0;
         __a_1[__i_0] = __a_0[__i_0];
         __a_2[__i_0] = __a_0[__i_0];
         __a_3[__i_0] = __a_0[__i_0];
         __b_0[__i_0][0] = __a_0[__i_0];
         __b_1[__i_0][0] = __b_0[__i_0][0];
         __b_2[__i_0][0] = __b_0[__i_0][0];
         __b_3[__i_0][0] = __b_0[__i_0][0];
      }
      __i_1 = __i_0;
      __i_2 = __i_0;
      __i_3 = __i_0;
   }
#pragma distributed on_cluster=0
   {
      int j;
      for(j = 1; j <= 4; j += 1) {
         __b_0[0][j] = __b_0[0][0]+j*10;
	 __b_1[0][j] = __b_0[0][j];
         __b_2[0][j] = __b_0[0][j];
         __b_3[0][j] = __b_0[0][j];
      }
   }
#pragma distributed on_cluster=1
   {
      int j;
      for(j = 1; j <= 4; j += 1) {
         __b_1[1][j] = __b_1[1][0]+j*10;
         __b_0[1][j] = __b_1[1][j];
         __b_2[1][j] = __b_1[1][j];
         __b_3[1][j] = __b_1[1][j];
      }
   }
#pragma distributed on_cluster=2
   {
      int j;
      for(j = 1; j <= 4; j += 1) {
         __b_2[2][j] = __b_2[2][0]+j*10;
         __b_0[2][j] = __b_2[2][j];
         __b_1[2][j] = __b_2[2][j];
         __b_3[2][j] = __b_2[2][j];
      }
   }
#pragma distributed on_cluster=3
   {
      int j;
      for(j = 1; j <= 4; j += 1) {
         __b_3[3][j] = __b_3[3][0]+j*10;
         __b_0[3][j] = __b_3[3][j];
         __b_1[3][j] = __b_3[3][j];
         __b_2[3][j] = __b_3[3][j];
      }
   }
   // Generated by Pass VARIABLE_REPLICATION
   int __c_0[20], __c_1[20], __c_2[20], __c_3[20];
#pragma distributed on_cluster=0
   {
      int j, k;
      for(__i_0 = 0; __i_0 <= 3; __i_0 += 1)
         for(j = 1; j <= 4; j += 1) {
            k = __i_0*4+j;
            __c_0[k] = __b_0[__i_0][j];
            __c_1[k] = __c_0[k];
            __c_2[k] = __c_0[k];
            __c_3[k] = __c_0[k];
         }
      __i_1 = __i_0;
      __i_2 = __i_0;
      __i_3 = __i_0;
   }

   return 0;
}
