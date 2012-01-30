
void scilab_rt_sum_i3_i0(int sin00, int sin01, int sin02, int in0[sin00][sin01][sin02],
      int* out0)
{
  int i;
  int j;
  int k;

  int val0 = 0;
  for (i = 0; i < sin00; ++i) {
    for (j = 0; j < sin01; ++j) {
      for (k = 0; k < sin02; ++k) {
        val0 += in0[i][j][k];
      }
    }
  }

  *out0 = val0;
}

