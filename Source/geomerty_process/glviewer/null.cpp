// This file is part of libigl, a simple c++ geometry processing library.
// 
// Copyright (C) 2015 Alec Jacobson <alecjacobson@gmail.com>
// 
// This Source Code Form is subject to the terms of the Mozilla Public License 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.
#include "null.h"
#include "EPS.h"

template <typename DerivedA, typename DerivedN>
 void null(
  const Eigen::PlainObjectBase<DerivedA> & A,
  Eigen::PlainObjectBase<DerivedN> & N)
{
  using namespace Eigen;
  typedef typename DerivedA::Scalar Scalar;
  JacobiSVD<Eigen::Matrix<Scalar,Eigen::Dynamic,Eigen::Dynamic> > svd(A, ComputeFullV);
  svd.setThreshold(A.cols() * svd.singularValues().maxCoeff() * EPS<Scalar>());
  N = svd.matrixV().rightCols(A.cols()-svd.rank());
}


template void null<Eigen::Matrix<double, 1, 3, 1, 1, 3>, Eigen::Matrix<double, -1, -1, 0, -1, -1> >(Eigen::PlainObjectBase<Eigen::Matrix<double, 1, 3, 1, 1, 3> > const&, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> >&);
template void null<Eigen::Matrix<float, 1, 3, 1, 1, 3>, Eigen::Matrix<float, 3, 2, 0, 3, 2> >(Eigen::PlainObjectBase<Eigen::Matrix<float, 1, 3, 1, 1, 3> > const&, Eigen::PlainObjectBase<Eigen::Matrix<float, 3, 2, 0, 3, 2> >&);

