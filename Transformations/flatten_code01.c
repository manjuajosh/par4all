/* Basic test case: the second "i" declaration ("int i = 2") conflicts
   with the first one, it will need to be rewritten.
 */

#include <stdio.h>

int flatten_code01()
{
  int i = 1;
  int j;
  i++;
  {
    int i = 2;
    i++;
    j=1;
  }
  i=j;

  printf("%d %d\n", i, j);
}

int main(int argc, char **argv)
{
  flatten_code01();
}
