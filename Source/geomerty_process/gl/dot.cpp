
#include "dot.h"

 double dot(
  const double *a, 
  const double *b)
{
  return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}
