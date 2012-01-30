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

  /*  t188.sce: test inttrap function */
  int _u_a[3][3];
  _u_a[0][0]=1;
  _u_a[0][1]=2;
  _u_a[0][2]=3;
  _u_a[1][0]=4;
  _u_a[1][1]=5;
  _u_a[1][2]=6;
  _u_a[2][0]=7;
  _u_a[2][1]=8;
  _u_a[2][2]=9;
  double _u_ar;
  scilab_rt_inttrap_i2_d0(3,3,_u_a,&_u_ar);
  scilab_rt_display_s0d0_("ar",_u_ar);
  double _u_b[3][3];
  scilab_rt_cos_i2_d2(3,3,_u_a,3,3,_u_b);
  double _u_br;
  scilab_rt_inttrap_d2_d0(3,3,_u_b,&_u_br);
  scilab_rt_display_s0d0_("br",_u_br);
  int _u_c[1][12];
  _u_c[0][0]=1;
  _u_c[0][1]=2;
  _u_c[0][2]=3;
  _u_c[0][3]=4;
  _u_c[0][4]=5;
  _u_c[0][5]=6;
  _u_c[0][6]=7;
  _u_c[0][7]=8;
  _u_c[0][8]=9;
  _u_c[0][9]=10;
  _u_c[0][10]=11;
  _u_c[0][11]=12;
  double _u_d[1][12];
  scilab_rt_cos_i2_d2(1,12,_u_c,1,12,_u_d);
  double _u_dr;
  scilab_rt_inttrap_d2_d0(1,12,_u_d,&_u_dr);
  scilab_rt_display_s0d0_("dr",_u_dr);
  double _tmpxx0[1][12];
  scilab_rt_sin_i2_d2(1,12,_u_c,1,12,_tmpxx0);
  double _u_cr;
  scilab_rt_inttrap_i2d2_d0(1,12,_u_c,1,12,_tmpxx0,&_u_cr);
  scilab_rt_display_s0d0_("cr",_u_cr);
  double _u_t[1][11];
  _u_t[0][0]=0;
  _u_t[0][1]=0.1;
  _u_t[0][2]=0.2;
  _u_t[0][3]=0.3;
  _u_t[0][4]=0.4;
  _u_t[0][5]=0.5;
  _u_t[0][6]=0.6;
  _u_t[0][7]=0.7;
  _u_t[0][8]=0.8;
  _u_t[0][9]=0.9;
  _u_t[0][10]=1;
  double _tmpxx1[1][11];
  scilab_rt_sin_d2_d2(1,11,_u_t,1,11,_tmpxx1);
  double _u_tr;
  scilab_rt_inttrap_d2d2_d0(1,11,_u_t,1,11,_tmpxx1,&_u_tr);
  scilab_rt_display_s0d0_("tr",_u_tr);
  int _u_r[3][3];
  _u_r[0][0]=1;
  _u_r[0][1]=2;
  _u_r[0][2]=3;
  _u_r[1][0]=4;
  _u_r[1][1]=5;
  _u_r[1][2]=6;
  _u_r[2][0]=7;
  _u_r[2][1]=8;
  _u_r[2][2]=9;
  double _tmpxx2[3][3];
  scilab_rt_cos_i2_d2(3,3,_u_r,3,3,_tmpxx2);
  double _u_rr;
  scilab_rt_inttrap_i2d2_d0(3,3,_u_r,3,3,_tmpxx2,&_u_rr);
  scilab_rt_display_s0d0_("rr",_u_rr);
  double _u_p[2][5];
  _u_p[0][0]=1.2;
  _u_p[0][1]=5.6;
  _u_p[0][2]=1.1;
  _u_p[0][3]=2.5;
  _u_p[0][4]=3.6;
  _u_p[1][0]=4.5;
  _u_p[1][1]=8.2;
  _u_p[1][2]=4.3;
  _u_p[1][3]=1.2;
  _u_p[1][4]=1.8;
  double _tmpxx3[2][5];
  scilab_rt_cos_d2_d2(2,5,_u_p,2,5,_tmpxx3);
  double _u_pr;
  scilab_rt_inttrap_d2d2_d0(2,5,_u_p,2,5,_tmpxx3,&_u_pr);
  scilab_rt_display_s0d0_("pr",_u_pr);
  double _u_k[2][5];
  _u_k[0][0]=1.2;
  _u_k[0][1]=5.6;
  _u_k[0][2]=1.1;
  _u_k[0][3]=2.5;
  _u_k[0][4]=3.6;
  _u_k[1][0]=4.5;
  _u_k[1][1]=8.2;
  _u_k[1][2]=4.3;
  _u_k[1][3]=1.2;
  _u_k[1][4]=1.8;
  double _tmpxx4[2][5];
  scilab_rt_cosh_d2_d2(2,5,_u_p,2,5,_tmpxx4);
  double _u_kr;
  scilab_rt_inttrap_d2d2_d0(2,5,_u_p,2,5,_tmpxx4,&_u_kr);
  scilab_rt_display_s0d0_("kr",_u_kr);

  scilab_rt_terminate();
}

