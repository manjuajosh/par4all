/** @addtogroup CUpscaling Classic C version

    @{
*/

/** @defgroup FUpscaling Upscaling functions.

    @{
    Functions to upscale the video.
    Chrominances are just copied.
    The Luminance is interpolated with an interpolation function at 6 points.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "yuv.h"
#include "upscale.h"

/* Upscaling at the half-pixel */
/* frame_in: input frame  */
/* frame_out: output frame */
const short normalisation=5;
const short bias=16;


void upscale_chrominance(type_yuv_frame_in frame_in, uint8 u_out[SIZE_UV_OUT], uint8 v_out[SIZE_UV_OUT])
{
  int sizeChrW = W_UV_IN;
  int sizeChrWout = W_UV_OUT;
  int sizeChrHout = H_UV_OUT;
  int sizeChrH = H_UV_IN;

   // Respecte mieux la localit� des donn�es
  for (int j = 0;j < sizeChrHout;j+=2) {
    int l = j >> 1;
    int jj = sizeChrWout*j;
    int ll = sizeChrW*l;
    // premi�re ligne de frame_out
    for (int i = 0; i < sizeChrWout;i+=2) {
      int k = i>>1;
      int indice  = ll+k;
      u_out[jj+i] = frame_in.u[indice];
      u_out[jj+i+1] = frame_in.u[indice];

      v_out[jj+i] = frame_in.v[indice];
      v_out[jj+i+1] = frame_in.v[indice];
    }
    jj += sizeChrWout;
    // deuxi�me ligne de frame_out
    for (int i = 0; i < sizeChrWout;i+=2) {
      int k = i>>1;
      int indice  = ll+k;
      u_out[jj+i] = frame_in.u[indice];
      u_out[jj+i+1] = frame_in.u[indice];

      v_out[jj+i] = frame_in.v[indice];
      v_out[jj+i+1] = frame_in.v[indice];
    }
  }
}

uint8 interpolation_luminance(uint8 a, uint8 b,uint8 c,uint8 d,uint8 e,uint8 f)
{
  short res = (short)a + (short)f -5*((short)b+(short)e - (((short)c+(short)d)<<2));
  res = (res+bias)>>normalisation;
  res=clip((res));
  return (uint8)res;
}

void upscale_luminance_centre(type_yuv_frame_in frame_in, uint8 y_out[SIZE_Y_OUT])
{
  int h = H_Y_IN;
  int w = W_Y_IN;

  int H = H_Y_OUT;
  int W = W_Y_OUT;
  
  int offset_y = OFFSET;
  int offset_x = OFFSET;

  for (int y=0; y < H; y +=2) {
    int ydiv2=(y>>1)+offset_y;
    int wydiv2=w*ydiv2;
    int Wy = y*W;
    for(int x=0;x< W;x += 2) {
      int xdiv2=(x>>1)+offset_x;
      int indice = xdiv2+wydiv2;
      // (x,y)
      y_out[Wy+x]=frame_in.y[xdiv2+wydiv2];
      //(x+1,y)
      y_out[Wy+x+1]=interpolation_luminance(frame_in.y[indice-2],frame_in.y[indice-1],frame_in.y[indice],frame_in.y[indice+1],frame_in.y[indice+2],frame_in.y[indice+3]);
      //(x,y+1)
      y_out[x+Wy+W]=interpolation_luminance(frame_in.y[indice-w*2],frame_in.y[indice-w],frame_in.y[indice],frame_in.y[indice+w],frame_in.y[indice+2*w],frame_in.y[indice+3*w]);
    }
  }
}

void  upscale_luminance_xplus1yplus1(uint8 y_fout[SIZE_Y_OUT],int W,int H)
{
  const int offset_max = W-OFFSET-2;

  for(int y=0;y<H;y=y+2){
    int Wy = W*(y+1);

    for(int x=0;x < OFFSET; x += 2) {
      int indice = x+Wy;
      y_fout[Wy+x+1]=interpolation_luminance(y_fout[Wy],y_fout[Wy],y_fout[indice],y_fout[indice+2],y_fout[indice+4],y_fout[indice+6]);
    }

    for(int x=OFFSET;x <= offset_max-2;x=x+2) {
      int indice = x+Wy;
      y_fout[Wy+x+1]=interpolation_luminance(y_fout[indice-4],y_fout[indice-2],y_fout[indice],y_fout[indice+2],y_fout[indice+4],y_fout[indice+6]);
    }


    int lim = W-2+W*(y+1);
    int x = offset_max;
    int indice = x+Wy;
    y_fout[Wy+x+1]=interpolation_luminance(y_fout[indice-4],y_fout[indice-2],y_fout[indice],y_fout[indice+2],y_fout[indice+4],y_fout[lim]);

    x = offset_max+2;
    indice = x+Wy;
    y_fout[Wy+x+1]=interpolation_luminance(y_fout[indice-4],y_fout[indice-2],y_fout[indice],y_fout[indice+2],y_fout[lim],y_fout[lim]);

    x = offset_max+4;
    indice = x+Wy;
    y_fout[Wy+x+1]=interpolation_luminance(y_fout[indice-4],y_fout[indice-2],y_fout[indice],y_fout[lim],y_fout[lim],y_fout[lim]);
  }
}

void upscale_luminance(type_yuv_frame_in frame_in,uint8 y_out[SIZE_Y_OUT])
{
  
  int W=W_Y_OUT;
  int H=H_Y_OUT;

  // Tout � la fin (x+1,y+1)
  upscale_luminance_centre(frame_in,y_out);
  upscale_luminance_xplus1yplus1(y_out,W,H); 
}

void upscale(type_yuv_frame_in frame_in,uint8 y[SIZE_Y_OUT], uint8 u[SIZE_UV_OUT], uint8 v[SIZE_UV_OUT])
{
  upscale_luminance(frame_in,y);
  upscale_chrominance(frame_in,u,v);
}


/** @} */
/** @} */
