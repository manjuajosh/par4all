#include "freia.h"
freia_status my_addsat
(freia_data2d *o, freia_data2d *i0, freia_data2d *i1)
{
  return freia_aipo_addsat(o, i0, i1);
}
