// This file is part of libigl, a simple c++ geometry processing library.
// 
// Copyright (C) 2013 Alec Jacobson <alecjacobson@gmail.com>
// 
// This Source Code Form is subject to the terms of the Mozilla Public License 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.
#include "EPS.h"

template <>  float EPS()
{
  return FLOAT_EPS;
}
template <>  double EPS()
{
  return DOUBLE_EPS;
}

template <>  float EPS_SQ()
{
  return FLOAT_EPS_SQ;
}
template <>  double EPS_SQ()
{
  return DOUBLE_EPS_SQ;
}

