/* Code generated by Faust compiler
 *
 * 
 */

float 	fslider2=0.;
int 	fYec0_idx=0;
int 	fYec0_idx_save=0;


void compute (int fullcount, float input[0][0], float output[1][256])
{
  float 	fSlow2 = (4.656613e-10f * fslider2);
  int index;
  int i;

  float output0[256];


  for (index = 0; index <= fullcount - 32; index += 32) {
    const int count = 32;
    float* output0 = &output[0][index];
    
    
    fYec0_idx =  (fYec0_idx+fYec0_idx_save)&1023;
    
    for (i=0; i<count; i++)
      output0[i] = fSlow2;
    
  }
}


void main()
{
  int count=256;
  float in[0][0];
  float out[1][count];
  compute(count,in,out);
}

