
void scilab_rt_squeeze_i3_i2(int in00, int in01, int in02, int matrixin0[in00][in01][in02],
     int out00, int out01, int matrixout0[out00][out01])
{
  int i;
  int j;
  int k;

  int val0 = 0;
  for (i = 0; i < in00; ++i) {
    for (j = 0; j < in01; ++j) {
      for (k = 0; k < in02; ++k) {
        val0 += matrixin0[i][j][k];
      }
    }
  }

  for (i = 0; i < out00; ++i) {
    for (j = 0; j < out01; ++j) {
        matrixout0[i][j] = val0;
    }
  }
}

