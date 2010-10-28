
/* Header automatically inserted by PYPS for defining MAX, MIN, MOD and others */
#ifndef MAX0
# define MAX0(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MAX
# define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
# define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MOD
# define MOD(a, b) ((a) % (b))
#endif

#ifndef DBLE
# define DBLE(a) ((double)(a))
#endif

#ifndef INT
# define INT(a) ((int)(a))
#endif

#ifdef WITH_TRIGO
#  include <math.h>
#  ifndef COS
#    define COS(a) (cos(a))
#  endif

#  ifndef SIN
#    define SIN(a) (sin(a))
#  endif
#endif
/* End header automatically inserted by PYPS for defining MAX, MIN, MOD and others */


#include <xmmintrin.h>

typedef float  a2sf[2] __attribute__ ((aligned (16)));
typedef float  a4sf[4] __attribute__ ((aligned (16)));
typedef double a2df[2] __attribute__ ((aligned (16)));
typedef int	a4si[4] __attribute__ ((aligned (16)));

typedef __m128  v4sf;
typedef __m128d v2df;
typedef __m128i v4si;
typedef __m128i v8hi;

/* float */
#define SIMD_LOAD_V4SF(vec,arr) vec=_mm_loadu_ps(arr)
#define SIMD_LOADA_V4SF(vec,arr) vec=_mm_load_ps(arr)
#define SIMD_MULPS(vec1,vec2,vec3) vec1=_mm_mul_ps(vec2,vec3)
#define SIMD_DIVPS(vec1,vec2,vec3) vec1=_mm_div_ps(vec2,vec3)
#define SIMD_ADDPS(vec1,vec2,vec3) vec1=_mm_add_ps(vec2,vec3)
#define SIMD_SUBPS(vec1, vec2, vec3) vec1 = _mm_sub_ps(vec2, vec3)
/* umin as in unary minus */
#define SIMD_UMINPS(vec1, vec2)						do {								__m128 __pips_tmp;					__pips_tmp = _mm_setzero_ps();				vec1 = _mm_sub_ps(__pips_tmp, vec2);			} while(0)

#define SIMD_STORE_V4SF(vec,arr) _mm_storeu_ps(arr,vec)
#define SIMD_STOREA_V4SF(vec,arr) _mm_store_ps(arr,vec)
#define SIMD_STORE_GENERIC_V4SF(vec,v0,v1,v2,v3)					do {										float __pips_tmp[4] __attribute__ ((aligned (16)));			SIMD_STOREA_V4SF(vec,&__pips_tmp[0]);					*(v0)=__pips_tmp[0];							*(v1)=__pips_tmp[1];							*(v2)=__pips_tmp[2];							*(v3)=__pips_tmp[3];							} while (0)

#define SIMD_ZERO_V4SF(vec) vec = _mm_setzero_ps()

#define SIMD_LOAD_GENERIC_V4SF(vec,v0,v1,v2,v3)						do {										float __pips_v[4] __attribute ((aligned (16)));		__pips_v[0]=v0;		__pips_v[1]=v1;		__pips_v[2]=v2;		__pips_v[3]=v3;		SIMD_LOADA_V4SF(vec,&__pips_v[0]);					} while(0)

/* handle padded value, this is a very bad implementation ... */
#define SIMD_STORE_MASKED_V4SF(vec,arr)							do {										float __pips_tmp[4] __attribute__ ((aligned (16)));							SIMD_STOREA_V4SF(vec,&__pips_tmp[0]);					(arr)[0] = __pips_tmp[0];						(arr)[1] = __pips_tmp[1];						(arr)[2] = __pips_tmp[2];						} while(0)

#define SIMD_LOAD_V4SI_TO_V4SF(v, f)				do {							float __pips_tmp[4];				__pips_tmp[0] = (f)[0];				__pips_tmp[1] = (f)[1];				__pips_tmp[2] = (f)[2];				__pips_tmp[3] = (f)[3];				SIMD_LOAD_V4SF(v, __pips_tmp);			} while(0)

/* double */
#define SIMD_LOAD_V2DF(vec,arr) vec=_mm_loadu_pd(arr)
#define SIMD_MULPD(vec1,vec2,vec3) vec1=_mm_mul_pd(vec2,vec3)
#define SIMD_ADDPD(vec1,vec2,vec3) vec1=_mm_add_pd(vec2,vec3)
#define SIMD_UMINPD(vec1, vec2)						do {								__m128d __pips_tmp;					__pips_tmp = _mm_setzero_pd();				vec1 = _mm_sub_pd(__pips_tmp, vec2);			} while(0)

#define SIMD_COSPD(vec1, vec2)								do {										double __pips_tmp[2] __attribute__ ((aligned (16)));			SIMD_STORE_V2DF(vec2, __pips_tmp);					__pips_tmp[0] = cos(__pips_tmp[0]);					__pips_tmp[1] = cos(__pips_tmp[1]);					SIMD_LOAD_V2DF(vec2, __pips_tmp);					} while(0)

#define SIMD_SINPD(vec1, vec2)								do {										double __pips_tmp[2] __attribute__ ((aligned (16)));			SIMD_STORE_V2DF(vec2, __pips_tmp);					__pips_tmp[0] = sin(__pips_tmp[0]);					__pips_tmp[1] = sin(__pips_tmp[1]);					} while(0)

#define SIMD_STORE_V2DF(vec,arr) _mm_storeu_pd(arr,vec)
#define SIMD_STORE_GENERIC_V2DF(vec, v0, v1)			do {							double __pips_tmp[2];					SIMD_STORE_V2DF(vec,&__pips_tmp[0]);			*(v0)=__pips_tmp[0];					*(v1)=__pips_tmp[1];					} while (0)
#define SIMD_LOAD_GENERIC_V2DF(vec,v0,v1)			do {							double v[2] = { v0,v1};				SIMD_LOAD_V2DF(vec,&v[0]);			} while(0)

/* conversions */
#define SIMD_STORE_V2DF_TO_V2SF(vec,f)					do {								double __pips_tmp[2];					SIMD_STORE_V2DF(vec, __pips_tmp);			(f)[0] = __pips_tmp[0];					(f)[1] = __pips_tmp[1];					} while(0)

#define SIMD_LOAD_V2SF_TO_V2DF(vec,f)				SIMD_LOAD_GENERIC_V2DF(vec,(f)[0],(f)[1])

/* char */
#define SIMD_LOAD_V8HI(vec,arr) 		vec = (__m128i*)(arr)

#define SIMD_STORE_V8HI(vec,arr)		*(__m128i *)(&(arr)[0]) = vec

#define SIMD_STORE_V8HI_TO_V8SI(vec,arr)	SIMD_STORE_V8HI(vec,arr)
#define SIMD_LOAD_V8SI_TO_V8HI(vec,arr)	SIMD_LOAD_V8HI(vec,arr)



/*
 * file for gaussian_noise_reduce.c
 */
/* PIPS include guard begin: #include <stdio.h> */
#include <stdio.h>
/* PIPS include guard end: #include <stdio.h> */
/* PIPS include guard begin: #include <stdlib.h> */
#include <stdlib.h>
/* PIPS include guard end: #include <stdlib.h> */

/*
        GAUSSIAN_NOISE_ REDUCE
        apply 5x5 Gaussian convolution filter, shrinks the image by 4 pixels in each direction, using Gaussian filter found here:
        http://en.wikipedia.org/wiki/Canny_edge_detector
        code dl from http://code.google.com/p/fast-edge
*/
void gaussian_noise_reduce(int w, int h, float img_in[w*h], float img_out[w*h]);

int main(int argc, char **argv);
void gaussian_noise_reduce(int w, int h, float img_in[w*h], float img_out[w*h])
{
   int y;
   //PIPS generated variable
   int LU_IND0;
   //PIPS generated variable
   float F_484, F_474, F_464, F_444, F_424, F_404, F_384, F_364, F_344, F_324, F_304, F_284, F_264, F_244, F_224, F_204, F_184, F_164, F_144, F_124, F_104, F_84, F_64, F_418, F_219;
   //PIPS generated variable
   int LU_IB00, LU_NUB00, max_y0, I_00, max_x0;
   //SAC generated temporary array
   a4si pdata0 = {2, 4, 5, 4};
   //SAC generated temporary array
   a4sf pdata1 = {0, 0, 0, 0};
   //SAC generated temporary array
   a4si pdata2 = {2, 4, 9, 12};
   //SAC generated temporary array
   a4sf pdata3 = {0, 0, 0, 0};
   //SAC generated temporary array
   a4si pdata4 = {9, 4, 5, 12};
   //SAC generated temporary array
   a4sf pdata5 = {0, 0, 0, 0};
   //SAC generated temporary array
   a4si pdata6 = {15, 12, 5, 4};
   //SAC generated temporary array
   a4sf pdata7 = {0, 0, 0, 0};
   //SAC generated temporary array
   a4si pdata8 = {9, 12, 9, 4};
   //SAC generated temporary array
   a4sf pdata9 = {0, 0, 0, 0};
   //SAC generated temporary array
   a4si pdata10 = {2, 4, 5, 4};
   //SAC generated temporary array
   a4sf pdata11 = {0, 0, 0, 0};
   //SAC generated temporary array
   a4si pdata12 = {2, 4, 2, 5};
   //SAC generated temporary array
   a4sf pdata13 = {0, 0, 0, 0};
   //SAC generated temporary array
   a4si pdata14 = {4, 2, 4, 5};
   //SAC generated temporary array
   a4sf pdata15 = {0, 0, 0, 0};
   //SAC generated temporary array
   a4si pdata16 = {4, 2, 2, 4};
   //SAC generated temporary array
   a4sf pdata17 = {0, 0, 0, 0};
   //SAC generated temporary array
   a4si pdata18 = {5, 4, 2, 4};
   //SAC generated temporary array
   a4sf pdata19 = {0, 0, 0, 0};
   //SAC generated temporary array
   a4si pdata20 = {5, 2, 4, 2};
   //SAC generated temporary array
   a4sf pdata21 = {0, 0, 0, 0};
   //SAC generated temporary array
   a4si pdata22 = {4, 9, 4, 12};
   //SAC generated temporary array
   a4sf pdata23 = {0, 0, 0, 0};
   //SAC generated temporary array
   a4si pdata24 = {9, 4, 9, 12};
   //SAC generated temporary array
   a4sf pdata25 = {0, 0, 0, 0};
   //SAC generated temporary array
   a4si pdata26 = {9, 4, 4, 9};
   //SAC generated temporary array
   a4sf pdata27 = {0, 0, 0, 0};
   //SAC generated temporary array
   a4si pdata28 = {12, 9, 4, 9};
   //SAC generated temporary array
   a4sf pdata29 = {0, 0, 0, 0};
   //SAC generated temporary array
   a4si pdata30 = {12, 4, 9, 4};
   //SAC generated temporary array
   a4sf pdata31 = {0, 0, 0, 0};
   //SAC generated temporary array
   a4si pdata32 = {5, 12, 5, 15};
   //SAC generated temporary array
   a4sf pdata33 = {0, 0, 0, 0};
   //SAC generated temporary array
   a4si pdata34 = {12, 5, 12, 15};
   //SAC generated temporary array
   a4sf pdata35 = {0, 0, 0, 0};
   //SAC generated temporary array
   a4si pdata36 = {12, 5, 5, 12};
   //SAC generated temporary array
   a4sf pdata37 = {0, 0, 0, 0};
   //SAC generated temporary array
   a4si pdata38 = {15, 12, 5, 12};
   //SAC generated temporary array
   a4sf pdata39 = {0, 0, 0, 0};
   //SAC generated temporary array
   a4si pdata40 = {15, 5, 12, 5};
   //SAC generated temporary array
   a4sf pdata41 = {0, 0, 0, 0};
   //SAC generated temporary array
   a4si pdata42 = {4, 9, 4, 12};
   //SAC generated temporary array
   a4sf pdata43 = {0, 0, 0, 0};
   //SAC generated temporary array
   a4si pdata44 = {9, 4, 9, 12};
   //SAC generated temporary array
   a4sf pdata45 = {0, 0, 0, 0};
   //SAC generated temporary array
   a4si pdata46 = {9, 4, 4, 9};
   //SAC generated temporary array
   a4sf pdata47 = {0, 0, 0, 0};
   //SAC generated temporary array
   a4si pdata48 = {12, 9, 4, 9};
   //SAC generated temporary array
   a4sf pdata49 = {0, 0, 0, 0};
   //SAC generated temporary array
   a4si pdata50 = {12, 4, 9, 4};
   //SAC generated temporary array
   a4sf pdata51 = {0, 0, 0, 0};
   //SAC generated temporary array
   a4si pdata52 = {2, 4, 2, 5};
   //SAC generated temporary array
   a4sf pdata53 = {0, 0, 0, 0};
   //SAC generated temporary array
   a4si pdata54 = {4, 2, 4, 5};
   //SAC generated temporary array
   a4sf pdata55 = {0, 0, 0, 0};
   //SAC generated temporary array
   a4si pdata56 = {4, 2, 2, 4};
   //SAC generated temporary array
   a4sf pdata57 = {0, 0, 0, 0};
   //SAC generated temporary array
   a4si pdata58 = {5, 4, 2, 4};
   //SAC generated temporary array
   a4sf pdata59 = {0, 0, 0, 0};
   //SAC generated temporary array
   a4si pdata60 = {5, 2, 4, 2};
   //SAC generated temporary array
   a4sf pdata61 = {0, 0, 0, 0}, pdata85 = {0, 0, 0, 0};
   //PIPS generated variable
   v4sf vec00_0, vec10_0, vec20_0, vec30_0, vec40_0, vec50_0, vec60_0, vec70_0, vec80_0, vec90_0, vec100_0, vec110_0, vec120_0, vec130_0, vec140_0, vec150_0, vec160_0, vec170_0, vec180_0, vec190_0, vec200_0, vec210_0, vec220_0, vec230_0, vec240_0, vec250_0, vec260_0, vec270_0, vec280_0, vec290_0, vec300_0, vec310_0, vec320_0, vec330_0, vec340_0, vec350_0, vec360_0, vec370_0, vec380_0, vec390_0, vec400_0, vec410_0, vec420_0, vec430_0, vec440_0, vec450_0, vec460_0, vec470_0, vec480_0, vec490_0, vec500_0, vec510_0, vec520_0, vec530_0, vec540_0, vec550_0, vec560_0, vec570_0, vec580_0, vec590_0, vec600_0, vec610_0, vec620_0, vec630_0, vec640_0, vec650_0, vec660_0, vec670_0, vec680_0, vec690_0, vec700_0, vec710_0, vec720_0, vec730_0, vec740_0, vec750_0, vec760_0, vec770_0, vec780_0, vec790_0, vec800_0, vec810_0, vec820_0, vec830_0, vec840_0, vec850_0, vec860_0, vec870_0, vec880_0, vec890_0, vec900_0, vec910_0, vec920_0, vec930_0, vec940_0, vec950_0, vec960_0, vec980_0, vec990_0, vec1010_0, vec1020_0, vec1040_0, vec1050_0, vec1070_0, vec1080_0, vec1100_0, vec1110_0, vec1130_0, vec1140_0, vec1160_0, vec1170_0, vec1190_0, vec1200_0, vec1220_0, vec1230_0, vec1250_0, vec1260_0, vec1280_0, vec1290_0, vec1310_0, vec1320_0, vec1340_0, vec1350_0, vec1370_0, vec1380_0, vec1400_0, vec1410_0, vec1430_0, vec1440_0, vec1460_0, vec1470_0, vec1490_0, vec1500_0, vec1520_0, vec1530_0, vec1550_0, vec1560_0, vec1580_0, vec1590_0, vec1610_0, vec1620_0, vec1640_0;
   max_x0 = w-2;
   I_00 = h-2;
   max_y0 = w*I_00;
   SIMD_LOAD_V4SI_TO_V4SF(vec10_0, &pdata0[0]);
   SIMD_LOAD_V4SI_TO_V4SF(vec40_0, &pdata2[0]);
   SIMD_LOAD_V4SI_TO_V4SF(vec70_0, &pdata4[0]);
   SIMD_LOAD_V4SI_TO_V4SF(vec100_0, &pdata6[0]);
   SIMD_LOAD_V4SI_TO_V4SF(vec130_0, &pdata8[0]);
   SIMD_LOAD_V4SI_TO_V4SF(vec160_0, &pdata10[0]);
   SIMD_LOAD_V4SI_TO_V4SF(vec190_0, &pdata12[0]);
   SIMD_LOAD_V4SI_TO_V4SF(vec220_0, &pdata14[0]);
   SIMD_LOAD_V4SI_TO_V4SF(vec250_0, &pdata16[0]);
   SIMD_LOAD_V4SI_TO_V4SF(vec280_0, &pdata18[0]);
   SIMD_LOAD_V4SI_TO_V4SF(vec310_0, &pdata20[0]);
   SIMD_LOAD_V4SI_TO_V4SF(vec340_0, &pdata22[0]);
   SIMD_LOAD_V4SI_TO_V4SF(vec370_0, &pdata24[0]);
   SIMD_LOAD_V4SI_TO_V4SF(vec400_0, &pdata26[0]);
   SIMD_LOAD_V4SI_TO_V4SF(vec430_0, &pdata28[0]);
   SIMD_LOAD_V4SI_TO_V4SF(vec460_0, &pdata30[0]);
   SIMD_LOAD_V4SI_TO_V4SF(vec490_0, &pdata32[0]);
   SIMD_LOAD_V4SI_TO_V4SF(vec520_0, &pdata34[0]);
   SIMD_LOAD_V4SI_TO_V4SF(vec550_0, &pdata36[0]);
   SIMD_LOAD_V4SI_TO_V4SF(vec580_0, &pdata38[0]);
   SIMD_LOAD_V4SI_TO_V4SF(vec610_0, &pdata40[0]);
   SIMD_LOAD_V4SI_TO_V4SF(vec640_0, &pdata42[0]);
   SIMD_LOAD_V4SI_TO_V4SF(vec670_0, &pdata44[0]);
   SIMD_LOAD_V4SI_TO_V4SF(vec700_0, &pdata46[0]);
   SIMD_LOAD_V4SI_TO_V4SF(vec730_0, &pdata48[0]);
   SIMD_LOAD_V4SI_TO_V4SF(vec760_0, &pdata50[0]);
   SIMD_LOAD_V4SI_TO_V4SF(vec790_0, &pdata52[0]);
   SIMD_LOAD_V4SI_TO_V4SF(vec820_0, &pdata54[0]);
   SIMD_LOAD_V4SI_TO_V4SF(vec850_0, &pdata56[0]);
   SIMD_LOAD_V4SI_TO_V4SF(vec880_0, &pdata58[0]);
   SIMD_LOAD_V4SI_TO_V4SF(vec910_0, &pdata60[0]);
   SIMD_LOAD_GENERIC_V4SF(vec950_0, pdata13[1], pdata15[0], pdata17[0], pdata19[1]);
   SIMD_LOAD_GENERIC_V4SF(vec940_0, pdata13[0], pdata13[2], pdata15[1], pdata17[1]);
   SIMD_LOAD_GENERIC_V4SF(vec980_0, pdata13[3], pdata15[3], pdata19[0], pdata21[0]);
   SIMD_LOAD_GENERIC_V4SF(vec1010_0, pdata15[2], pdata17[3], pdata19[3], pdata21[2]);
   SIMD_LOAD_GENERIC_V4SF(vec1040_0, pdata17[2], pdata19[2], pdata21[1], pdata21[3]);
   SIMD_LOAD_GENERIC_V4SF(vec1070_0, pdata23[0], pdata23[2], pdata25[1], pdata27[1]);
   SIMD_LOAD_GENERIC_V4SF(vec1100_0, pdata23[1], pdata25[0], pdata27[0], pdata29[1]);
   SIMD_LOAD_GENERIC_V4SF(vec1130_0, pdata23[3], pdata25[3], pdata29[0], pdata31[0]);
   SIMD_LOAD_GENERIC_V4SF(vec1160_0, pdata25[2], pdata27[3], pdata29[3], pdata31[2]);
   SIMD_LOAD_GENERIC_V4SF(vec1190_0, pdata27[2], pdata29[2], pdata31[1], pdata31[3]);
   SIMD_LOAD_GENERIC_V4SF(vec1220_0, pdata33[0], pdata33[2], pdata35[1], pdata37[1]);
   SIMD_LOAD_GENERIC_V4SF(vec1250_0, pdata33[1], pdata35[0], pdata37[0], pdata39[1]);
   SIMD_LOAD_GENERIC_V4SF(vec1280_0, pdata33[3], pdata35[3], pdata39[0], pdata41[0]);
   SIMD_LOAD_GENERIC_V4SF(vec1310_0, pdata35[2], pdata37[3], pdata39[3], pdata41[2]);
   SIMD_LOAD_GENERIC_V4SF(vec1340_0, pdata37[2], pdata39[2], pdata41[1], pdata41[3]);
   SIMD_LOAD_GENERIC_V4SF(vec1370_0, pdata43[0], pdata43[2], pdata45[1], pdata47[1]);
   SIMD_LOAD_GENERIC_V4SF(vec1400_0, pdata43[1], pdata45[0], pdata47[0], pdata49[1]);
   SIMD_LOAD_GENERIC_V4SF(vec1430_0, pdata43[3], pdata45[3], pdata49[0], pdata51[0]);
   SIMD_LOAD_GENERIC_V4SF(vec1460_0, pdata45[2], pdata47[3], pdata49[3], pdata51[2]);
   SIMD_LOAD_GENERIC_V4SF(vec1490_0, pdata47[2], pdata49[2], pdata51[1], pdata51[3]);
   SIMD_LOAD_GENERIC_V4SF(vec1520_0, pdata53[0], pdata53[2], pdata55[1], pdata57[1]);
   SIMD_LOAD_GENERIC_V4SF(vec1550_0, pdata53[1], pdata55[0], pdata57[0], pdata59[1]);
   SIMD_LOAD_GENERIC_V4SF(vec1580_0, pdata53[3], pdata55[3], pdata59[0], pdata61[0]);
   SIMD_LOAD_GENERIC_V4SF(vec1610_0, pdata55[2], pdata57[3], pdata59[3], pdata61[2]);
   SIMD_LOAD_GENERIC_V4SF(vec1640_0, pdata57[2], pdata59[2], pdata61[1], pdata61[3]);
   for(y = 2*w; y <= max_y0-1; y += w) {
      LU_NUB00 = max_x0-2;
      LU_IB00 = MOD(LU_NUB00, 4);
      for(LU_IND0 = 0; LU_IND0 <= LU_IB00-1; LU_IND0 += 1) {
         //PIPS:SAC generated v4sf vector(s)
         SIMD_LOAD_V4SF(vec20_0, &img_in[LU_IND0+y-2*w]);
         SIMD_MULPS(vec00_0, vec10_0, vec20_0);
         SIMD_STORE_V4SF(vec00_0, &pdata1[0]);
         SIMD_LOAD_GENERIC_V4SF(vec50_0, img_in[4+LU_IND0+y-2*w], img_in[LU_IND0+y-w], img_in[1+LU_IND0+y-w], img_in[2+LU_IND0+y-w]);
         SIMD_MULPS(vec30_0, vec40_0, vec50_0);
         SIMD_STORE_V4SF(vec30_0, &pdata3[0]);
         SIMD_LOAD_GENERIC_V4SF(vec80_0, img_in[3+LU_IND0+y-w], img_in[4+LU_IND0+y-w], img_in[LU_IND0+y], img_in[1+LU_IND0+y]);
         SIMD_MULPS(vec60_0, vec70_0, vec80_0);
         SIMD_STORE_V4SF(vec60_0, &pdata5[0]);
         SIMD_LOAD_GENERIC_V4SF(vec110_0, img_in[2+LU_IND0+y], img_in[3+LU_IND0+y], img_in[4+LU_IND0+y], img_in[w+LU_IND0+y]);
         SIMD_MULPS(vec90_0, vec100_0, vec110_0);
         SIMD_STORE_V4SF(vec90_0, &pdata7[0]);
         SIMD_LOAD_V4SF(vec140_0, &img_in[1+w+LU_IND0+y]);
         SIMD_MULPS(vec120_0, vec130_0, vec140_0);
         SIMD_STORE_V4SF(vec120_0, &pdata9[0]);
         SIMD_LOAD_V4SF(vec170_0, &img_in[LU_IND0+y+2*w]);
         SIMD_MULPS(vec150_0, vec160_0, vec170_0);
         SIMD_STORE_V4SF(vec150_0, &pdata11[0]);
         pdata1[0] = 2*img_in[LU_IND0+y-2*w];
         F_474 = 2*img_in[4+LU_IND0+y+2*w];
         F_219 = pdata1[0]+pdata1[1];
         F_418 = F_219+pdata1[2];
         F_64 = F_418+pdata1[3];
         F_84 = F_64+pdata3[0];
         F_104 = F_84+pdata3[1];
         F_124 = F_104+pdata3[2];
         F_144 = F_124+pdata3[3];
         F_164 = F_144+pdata5[0];
         F_184 = F_164+pdata5[1];
         F_204 = F_184+pdata5[2];
         F_224 = F_204+pdata5[3];
         F_244 = F_224+pdata7[0];
         F_264 = F_244+pdata7[1];
         F_284 = F_264+pdata7[2];
         F_304 = F_284+pdata7[3];
         F_324 = F_304+pdata9[0];
         F_344 = F_324+pdata9[1];
         F_364 = F_344+pdata9[2];
         F_384 = F_364+pdata9[3];
         F_404 = F_384+pdata11[0];
         F_424 = F_404+pdata11[1];
         F_444 = F_424+pdata11[2];
         F_464 = F_444+pdata11[3];
         F_484 = F_464+F_474;
         img_out[2+LU_IND0+y] = F_484/159;
      }
      for(LU_IND0 = LU_IB00; LU_IND0 <= LU_NUB00-1; LU_IND0 += 4) {
         //PIPS:SAC generated v4sf vector(s)
         SIMD_LOAD_GENERIC_V4SF(vec200_0, img_in[LU_IND0+y-2*w], img_in[1+LU_IND0+y-2*w], img_in[1+LU_IND0+y-2*w], img_in[2+LU_IND0+y-2*w]);
         SIMD_MULPS(vec180_0, vec190_0, vec200_0);
         SIMD_LOAD_GENERIC_V4SF(vec230_0, img_in[2+LU_IND0+y-2*w], img_in[2+LU_IND0+y-2*w], img_in[3+LU_IND0+y-2*w], img_in[3+LU_IND0+y-2*w]);
         SIMD_MULPS(vec210_0, vec220_0, vec230_0);
         SIMD_LOAD_GENERIC_V4SF(vec260_0, img_in[3+LU_IND0+y-2*w], img_in[3+LU_IND0+y-2*w], img_in[4+LU_IND0+y-2*w], img_in[4+LU_IND0+y-2*w]);
         SIMD_MULPS(vec240_0, vec250_0, vec260_0);
         SIMD_LOAD_GENERIC_V4SF(vec290_0, img_in[4+LU_IND0+y-2*w], img_in[4+LU_IND0+y-2*w], img_in[5+LU_IND0+y-2*w], img_in[5+LU_IND0+y-2*w]);
         SIMD_MULPS(vec270_0, vec280_0, vec290_0);
         SIMD_LOAD_GENERIC_V4SF(vec320_0, img_in[5+LU_IND0+y-2*w], img_in[6+LU_IND0+y-2*w], img_in[6+LU_IND0+y-2*w], img_in[7+LU_IND0+y-2*w]);
         SIMD_MULPS(vec300_0, vec310_0, vec320_0);
         SIMD_LOAD_GENERIC_V4SF(vec350_0, img_in[LU_IND0+y-w], img_in[1+LU_IND0+y-w], img_in[1+LU_IND0+y-w], img_in[2+LU_IND0+y-w]);
         SIMD_MULPS(vec330_0, vec340_0, vec350_0);
         SIMD_LOAD_GENERIC_V4SF(vec380_0, img_in[2+LU_IND0+y-w], img_in[2+LU_IND0+y-w], img_in[3+LU_IND0+y-w], img_in[3+LU_IND0+y-w]);
         SIMD_MULPS(vec360_0, vec370_0, vec380_0);
         SIMD_LOAD_GENERIC_V4SF(vec410_0, img_in[3+LU_IND0+y-w], img_in[3+LU_IND0+y-w], img_in[4+LU_IND0+y-w], img_in[4+LU_IND0+y-w]);
         SIMD_MULPS(vec390_0, vec400_0, vec410_0);
         SIMD_LOAD_GENERIC_V4SF(vec440_0, img_in[4+LU_IND0+y-w], img_in[4+LU_IND0+y-w], img_in[5+LU_IND0+y-w], img_in[5+LU_IND0+y-w]);
         SIMD_MULPS(vec420_0, vec430_0, vec440_0);
         SIMD_LOAD_GENERIC_V4SF(vec470_0, img_in[5+LU_IND0+y-w], img_in[6+LU_IND0+y-w], img_in[6+LU_IND0+y-w], img_in[7+LU_IND0+y-w]);
         SIMD_MULPS(vec450_0, vec460_0, vec470_0);
         SIMD_LOAD_GENERIC_V4SF(vec500_0, img_in[LU_IND0+y], img_in[1+LU_IND0+y], img_in[1+LU_IND0+y], img_in[2+LU_IND0+y]);
         SIMD_MULPS(vec480_0, vec490_0, vec500_0);
         SIMD_LOAD_GENERIC_V4SF(vec530_0, img_in[2+LU_IND0+y], img_in[2+LU_IND0+y], img_in[3+LU_IND0+y], img_in[3+LU_IND0+y]);
         SIMD_MULPS(vec510_0, vec520_0, vec530_0);
         SIMD_LOAD_GENERIC_V4SF(vec560_0, img_in[3+LU_IND0+y], img_in[3+LU_IND0+y], img_in[4+LU_IND0+y], img_in[4+LU_IND0+y]);
         SIMD_MULPS(vec540_0, vec550_0, vec560_0);
         SIMD_LOAD_GENERIC_V4SF(vec590_0, img_in[4+LU_IND0+y], img_in[4+LU_IND0+y], img_in[5+LU_IND0+y], img_in[5+LU_IND0+y]);
         SIMD_MULPS(vec570_0, vec580_0, vec590_0);
         SIMD_LOAD_GENERIC_V4SF(vec620_0, img_in[5+LU_IND0+y], img_in[6+LU_IND0+y], img_in[6+LU_IND0+y], img_in[7+LU_IND0+y]);
         SIMD_MULPS(vec600_0, vec610_0, vec620_0);
         SIMD_LOAD_GENERIC_V4SF(vec650_0, img_in[w+LU_IND0+y], img_in[1+w+LU_IND0+y], img_in[1+w+LU_IND0+y], img_in[2+w+LU_IND0+y]);
         SIMD_MULPS(vec630_0, vec640_0, vec650_0);
         SIMD_LOAD_GENERIC_V4SF(vec680_0, img_in[2+w+LU_IND0+y], img_in[2+w+LU_IND0+y], img_in[3+w+LU_IND0+y], img_in[3+w+LU_IND0+y]);
         SIMD_MULPS(vec660_0, vec670_0, vec680_0);
         SIMD_LOAD_GENERIC_V4SF(vec710_0, img_in[3+w+LU_IND0+y], img_in[3+w+LU_IND0+y], img_in[4+w+LU_IND0+y], img_in[4+w+LU_IND0+y]);
         SIMD_MULPS(vec690_0, vec700_0, vec710_0);
         SIMD_LOAD_GENERIC_V4SF(vec740_0, img_in[4+w+LU_IND0+y], img_in[4+w+LU_IND0+y], img_in[5+w+LU_IND0+y], img_in[5+w+LU_IND0+y]);
         SIMD_MULPS(vec720_0, vec730_0, vec740_0);
         SIMD_LOAD_GENERIC_V4SF(vec770_0, img_in[5+w+LU_IND0+y], img_in[6+w+LU_IND0+y], img_in[6+w+LU_IND0+y], img_in[7+w+LU_IND0+y]);
         SIMD_MULPS(vec750_0, vec760_0, vec770_0);
         SIMD_LOAD_GENERIC_V4SF(vec800_0, img_in[LU_IND0+y+2*w], img_in[1+LU_IND0+y+2*w], img_in[1+LU_IND0+y+2*w], img_in[2+LU_IND0+y+2*w]);
         SIMD_MULPS(vec780_0, vec790_0, vec800_0);
         SIMD_LOAD_GENERIC_V4SF(vec830_0, img_in[2+LU_IND0+y+2*w], img_in[2+LU_IND0+y+2*w], img_in[3+LU_IND0+y+2*w], img_in[3+LU_IND0+y+2*w]);
         SIMD_MULPS(vec810_0, vec820_0, vec830_0);
         SIMD_LOAD_GENERIC_V4SF(vec860_0, img_in[3+LU_IND0+y+2*w], img_in[3+LU_IND0+y+2*w], img_in[4+LU_IND0+y+2*w], img_in[4+LU_IND0+y+2*w]);
         SIMD_MULPS(vec840_0, vec850_0, vec860_0);
         SIMD_LOAD_GENERIC_V4SF(vec890_0, img_in[4+LU_IND0+y+2*w], img_in[4+LU_IND0+y+2*w], img_in[5+LU_IND0+y+2*w], img_in[5+LU_IND0+y+2*w]);
         SIMD_MULPS(vec870_0, vec880_0, vec890_0);
         SIMD_LOAD_GENERIC_V4SF(vec920_0, img_in[5+LU_IND0+y+2*w], img_in[6+LU_IND0+y+2*w], img_in[6+LU_IND0+y+2*w], img_in[7+LU_IND0+y+2*w]);
         SIMD_MULPS(vec900_0, vec910_0, vec920_0);
         SIMD_ADDPS(vec930_0, vec940_0, vec950_0);
         SIMD_ADDPS(vec960_0, vec930_0, vec980_0);
         SIMD_ADDPS(vec990_0, vec960_0, vec1010_0);
         SIMD_ADDPS(vec1020_0, vec990_0, vec1040_0);
         SIMD_ADDPS(vec1050_0, vec1020_0, vec1070_0);
         SIMD_ADDPS(vec1080_0, vec1050_0, vec1100_0);
         SIMD_ADDPS(vec1110_0, vec1080_0, vec1130_0);
         SIMD_ADDPS(vec1140_0, vec1110_0, vec1160_0);
         SIMD_ADDPS(vec1170_0, vec1140_0, vec1190_0);
         SIMD_ADDPS(vec1200_0, vec1170_0, vec1220_0);
         SIMD_ADDPS(vec1230_0, vec1200_0, vec1250_0);
         SIMD_ADDPS(vec1260_0, vec1230_0, vec1280_0);
         SIMD_ADDPS(vec1290_0, vec1260_0, vec1310_0);
         SIMD_ADDPS(vec1320_0, vec1290_0, vec1340_0);
         SIMD_ADDPS(vec1350_0, vec1320_0, vec1370_0);
         SIMD_ADDPS(vec1380_0, vec1350_0, vec1400_0);
         SIMD_ADDPS(vec1410_0, vec1380_0, vec1430_0);
         SIMD_ADDPS(vec1440_0, vec1410_0, vec1460_0);
         SIMD_ADDPS(vec1470_0, vec1440_0, vec1490_0);
         SIMD_ADDPS(vec1500_0, vec1470_0, vec1520_0);
         SIMD_ADDPS(vec1530_0, vec1500_0, vec1550_0);
         SIMD_ADDPS(vec1560_0, vec1530_0, vec1580_0);
         SIMD_ADDPS(vec1590_0, vec1560_0, vec1610_0);
         SIMD_ADDPS(vec1620_0, vec1590_0, vec1640_0);
         SIMD_STORE_V4SF(vec1620_0, &pdata85[0]);
         img_out[2+LU_IND0+y] = pdata85[0]/159;
         img_out[3+LU_IND0+y] = pdata85[1]/159;
         img_out[4+LU_IND0+y] = pdata85[2]/159;
         img_out[5+LU_IND0+y] = pdata85[3]/159;
      }
   }
   ;
}
int main(int argc, char **argv)
{
   int n = argc==1?100:atoi(argv[1]);
   float (*in)[n*n], (*out)[n*n];
   int i;
   in = malloc(n*n);
   out = malloc(n*n);
   for(i = 0; i <= n*n-1; i += 1) {
      (*in)[i] = (float) i;
      (*out)[i] = 0;
   }
   gaussian_noise_reduce(n, n, *in, *out);
   for(i = 0; i <= n*n-1; i += 1)
      printf("%c", (*out)[i]);
   free(in);
   free(out);
   return 0;
}
