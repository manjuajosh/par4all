// Check controlizer

void block05()
{
  int i = 1;

  i--;
  {
    int j = i++;
    i += j;
  }
}
