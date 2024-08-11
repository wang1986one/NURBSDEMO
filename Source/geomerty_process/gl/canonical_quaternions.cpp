
#include "canonical_quaternions.h"

template <>  float CANONICAL_VIEW_QUAT<float>(int i, int j)
{
  return (float)CANONICAL_VIEW_QUAT_F[i][j];
}
template <>  double CANONICAL_VIEW_QUAT<double>(int i, int j)
{
  return (double)CANONICAL_VIEW_QUAT_D[i][j];
}

