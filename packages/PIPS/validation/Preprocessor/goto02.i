main()
{
  int a[5];
  int i;
  for (i=0; i<5;i++)
    if (a[i] <0)
      goto found;
 found: printf("found\n");
}
