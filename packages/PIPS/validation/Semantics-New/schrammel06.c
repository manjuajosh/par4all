// Check that identity transformers do not lead to some information
// loss
//
// Here j = 2 before the loop, which is entered or not, and j is
// conditionnaly set to 3 within the loops. So its only two possible
// value after the loop are 2 and 3. This information is only found if
// the test k>=3 is removed

main()
{
  int i, j = 2, k;

  while(i*i>1) {
    if(k>=3)
      j = 3;
  }
  j = j;
}
