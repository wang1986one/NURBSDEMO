#pragma once 

#include <Eigen/Dense>


  /// Compute a basis for the null space for the given matrix A: the columns of
  /// the output N form a basis for the space orthogonal to that spanned by the
  /// rows of A.
  /// Like MATLAB's null
  ///
  /// @param[in] A  m by n matrix
  /// @param[out] N  n by r matrix, where r is the row rank of A
  template <typename DerivedA, typename DerivedN>
   void null(
    const Eigen::PlainObjectBase<DerivedA> & A,
    Eigen::PlainObjectBase<DerivedN> & N);

