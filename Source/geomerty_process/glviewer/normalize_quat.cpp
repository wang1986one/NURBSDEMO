// This file is part of libigl, a simple c++ geometry processing library.
// 
// Copyright (C) 2013 Alec Jacobson <alecjacobson@gmail.com>
// 
// This Source Code Form is subject to the terms of the Mozilla Public License 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.
#include "normalize_quat.h"

#include "EPS.h"
#include <cmath>

template <typename Q_type>
 bool normalize_quat(
  const Q_type *q,
  Q_type *out)
{
  // Get length
  Q_type len = sqrt(
    q[0]*q[0]+
    q[1]*q[1]+
    q[2]*q[2]+
    q[3]*q[3]);

  // Noramlize each coordinate
  out[0] = q[0]/len;
  out[1] = q[1]/len;
  out[2] = q[2]/len;
  out[3] = q[3]/len;

  // Test whether length was below Epsilon
  return (len > EPS<Q_type>());
}


template bool normalize_quat<double>(double const*, double*);

template bool normalize_quat<float>(float const*, float*);
