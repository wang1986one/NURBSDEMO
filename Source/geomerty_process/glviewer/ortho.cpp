// This file is part of libigl, a simple c++ geometry processing library.
// 
// Copyright (C) 2015 Alec Jacobson <alecjacobson@gmail.com>
// 
// This Source Code Form is subject to the terms of the Mozilla Public License 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.
#include "ortho.h"

template < typename DerivedP>
void ortho(
  const typename DerivedP::Scalar left,
  const typename DerivedP::Scalar right,
  const typename DerivedP::Scalar bottom,
  const typename DerivedP::Scalar top,
  const typename DerivedP::Scalar nearVal,
  const typename DerivedP::Scalar farVal,
  Eigen::PlainObjectBase<DerivedP> & P)
{
  P.setIdentity();
  P(0,0) = 2. / (right - left);
  P(1,1) = 2. / (top - bottom);
  P(2,2) = - 2./ (farVal - nearVal);
  P(0,3) = - (right + left) / (right - left);
  P(1,3) = - (top + bottom) / (top - bottom);
  P(2,3) = - (farVal + nearVal) / (farVal - nearVal);
}


template void ortho<Eigen::Matrix<float, 4, 4, 0, 4, 4> >(Eigen::Matrix<float, 4, 4, 0, 4, 4>::Scalar, Eigen::Matrix<float, 4, 4, 0, 4, 4>::Scalar, Eigen::Matrix<float, 4, 4, 0, 4, 4>::Scalar, Eigen::Matrix<float, 4, 4, 0, 4, 4>::Scalar, Eigen::Matrix<float, 4, 4, 0, 4, 4>::Scalar, Eigen::Matrix<float, 4, 4, 0, 4, 4>::Scalar, Eigen::PlainObjectBase<Eigen::Matrix<float, 4, 4, 0, 4, 4> >&);
