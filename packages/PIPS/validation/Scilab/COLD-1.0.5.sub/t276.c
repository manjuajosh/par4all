/*
 * (c) HPC Project - 2010-2011 - All rights reserved
 *
 */

#include "scilab_rt.h"


int __lv0;
int __lv1;
int __lv2;
int __lv3;

/*----------------------------------------------------*/


/*----------------------------------------------------*/

int main(int argc, char* argv[])
{
  scilab_rt_init(argc, argv, COLD_MODE_STANDALONE);

  /*  t275.sce _ fft function Forward and Backward */
  double _u_a1[1][9];
  _u_a1[0][0]=1.0;
  _u_a1[0][1]=2;
  _u_a1[0][2]=13;
  _u_a1[0][3]=4;
  _u_a1[0][4]=15;
  _u_a1[0][5]=6;
  _u_a1[0][6]=7;
  _u_a1[0][7]=18;
  _u_a1[0][8]=9;
  scilab_rt_display_s0d2_("a1",1,9,_u_a1);
  double complex _u_y1[1][9];
  scilab_rt_fft_d2d0_z2(1,9,_u_a1,(-1),1,9,_u_y1);
  scilab_rt_display_s0z2_("y1",1,9,_u_y1);
  scilab_rt_display_s0d2_("a1",1,9,_u_a1);
  int _u_a2[9][1];
  _u_a2[0][0]=1;
  _u_a2[1][0]=2;
  _u_a2[2][0]=13;
  _u_a2[3][0]=4;
  _u_a2[4][0]=15;
  _u_a2[5][0]=6;
  _u_a2[6][0]=7;
  _u_a2[7][0]=18;
  _u_a2[8][0]=9;
  scilab_rt_display_s0i2_("a2",9,1,_u_a2);
  double complex _u_y2[9][1];
  scilab_rt_fft_i2d0_z2(9,1,_u_a2,(-1),9,1,_u_y2);
  scilab_rt_display_s0z2_("y2",9,1,_u_y2);
  scilab_rt_display_s0i2_("a2",9,1,_u_a2);
  double complex _tmpxx0 = (2*I);
  double complex _tmpxx1 = (3*I);
  double complex _tmpxx2 = (14*I);
  double complex _tmpxx3 = (5*I);
  double complex _tmpxx4 = (16*I);
  double complex _tmpxx5 = (7*I);
  double complex _tmpxx6 = (8*I);
  double complex _tmpxx7 = (19*I);
  double complex _tmpxx8 = (10*I);
  double complex _u_a3[9][1];
  _u_a3[0][0]=(1+_tmpxx0);
  _u_a3[1][0]=(2+_tmpxx1);
  _u_a3[2][0]=(13+_tmpxx2);
  _u_a3[3][0]=(4+_tmpxx3);
  _u_a3[4][0]=(15+_tmpxx4);
  _u_a3[5][0]=(6+_tmpxx5);
  _u_a3[6][0]=(7+_tmpxx6);
  _u_a3[7][0]=(18+_tmpxx7);
  _u_a3[8][0]=(9+_tmpxx8);
  scilab_rt_display_s0z2_("a3",9,1,_u_a3);
  double complex _u_y3[9][1];
  scilab_rt_fft_z2d0_z2(9,1,_u_a3,(-1),9,1,_u_y3);
  scilab_rt_display_s0z2_("y3",9,1,_u_y3);
  scilab_rt_display_s0z2_("a3",9,1,_u_a3);
  double _u_a4[1][9];
  _u_a4[0][0]=1.0;
  _u_a4[0][1]=2;
  _u_a4[0][2]=13;
  _u_a4[0][3]=4;
  _u_a4[0][4]=15;
  _u_a4[0][5]=6;
  _u_a4[0][6]=7;
  _u_a4[0][7]=18;
  _u_a4[0][8]=9;
  scilab_rt_display_s0d2_("a4",1,9,_u_a4);
  double complex _u_y4[1][9];
  scilab_rt_fft_d2d0_z2(1,9,_u_a4,1,1,9,_u_y4);
  scilab_rt_display_s0z2_("y4",1,9,_u_y4);
  scilab_rt_display_s0d2_("a4",1,9,_u_a4);
  int _u_a5[9][1];
  _u_a5[0][0]=1;
  _u_a5[1][0]=2;
  _u_a5[2][0]=13;
  _u_a5[3][0]=4;
  _u_a5[4][0]=15;
  _u_a5[5][0]=6;
  _u_a5[6][0]=7;
  _u_a5[7][0]=18;
  _u_a5[8][0]=9;
  scilab_rt_display_s0i2_("a5",9,1,_u_a5);
  double complex _u_y5[9][1];
  scilab_rt_fft_i2d0_z2(9,1,_u_a5,1,9,1,_u_y5);
  scilab_rt_display_s0z2_("y5",9,1,_u_y5);
  scilab_rt_display_s0i2_("a5",9,1,_u_a5);
  double complex _tmpxx9 = (2*I);
  double complex _tmpxx10 = (3*I);
  double complex _tmpxx11 = (14*I);
  double complex _tmpxx12 = (5*I);
  double complex _tmpxx13 = (16*I);
  double complex _tmpxx14 = (7*I);
  double complex _tmpxx15 = (8*I);
  double complex _tmpxx16 = (19*I);
  double complex _tmpxx17 = (10*I);
  double complex _u_a6[9][1];
  _u_a6[0][0]=(1+_tmpxx9);
  _u_a6[1][0]=(2+_tmpxx10);
  _u_a6[2][0]=(13+_tmpxx11);
  _u_a6[3][0]=(4+_tmpxx12);
  _u_a6[4][0]=(15+_tmpxx13);
  _u_a6[5][0]=(6+_tmpxx14);
  _u_a6[6][0]=(7+_tmpxx15);
  _u_a6[7][0]=(18+_tmpxx16);
  _u_a6[8][0]=(9+_tmpxx17);
  scilab_rt_display_s0z2_("a6",9,1,_u_a6);
  double complex _u_y6[9][1];
  scilab_rt_fft_z2d0_z2(9,1,_u_a6,1,9,1,_u_y6);
  scilab_rt_display_s0z2_("y6",9,1,_u_y6);
  scilab_rt_display_s0z2_("a6",9,1,_u_a6);

  scilab_rt_terminate();
}

