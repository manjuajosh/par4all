// array _u_x must not be scalarized at the loop level because
// its reference indices use __lv2 which is modified.
// it can nowever be scalarized at the inner statement level.

#include <stdio.h>

int __lv1;
int __lv2;

int main(int argc, char* argv[])
{
  /* x = [0.1:0.1:1]; */
   double _u_x[10][1];
  _u_x[0][0]=0.1;
  _u_x[1][0]=0.2;
  _u_x[2][0]=0.3;
  _u_x[3][0]=0.4;
  _u_x[4][0]=0.5;
  _u_x[5][0]=0.6;
  _u_x[6][0]=0.7;
  _u_x[7][0]=0.8;
  _u_x[8][0]=0.9;
  _u_x[9][0]=1;
  double _u_c[10][1];
  for (__lv1=0;  __lv1<10;  __lv1++)
    {
      // originaly generated by simplify_control_directly for loop
      // for (__lv2=0;  __lv2<1;  __lv2++)
      __lv2 = 0;
      _u_c[__lv1][__lv2] = _u_x[__lv1][__lv2] + _u_x[__lv1][__lv2];
      __lv2 = 0+1;
    }

  for (__lv1=0;  __lv1<10;  __lv1++)
    for (__lv2=0;  __lv2<1;  __lv2++)
      printf( "%lf\n",_u_c[__lv1][__lv2]);

  return 0;
}
