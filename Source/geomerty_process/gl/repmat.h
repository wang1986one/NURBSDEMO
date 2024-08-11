#pragma once

#define EIGEN_YES_I_KNOW_SPARSE_MODULE_IS_NOT_STABLE_YET
#include <Eigen/Dense>
#include <Eigen/Sparse>


  /// Replicate and tile a matrix
  ///
  /// @tparam T  should be a eigen matrix primitive type like int or double
  /// @param[in] A  m by n input matrix
  /// @param[in] r  number of row-direction copies
  /// @param[in] c  number of col-direction copies
  /// @param[out] B  r*m by c*n output matrix
  ///
  /// \note At least for Dense matrices this is replaced by `replicate` e.g., dst = src.replicate(n,m);
  /// http://forum.kde.org/viewtopic.php?f=74&t=90876#p173517
  ///
  template <typename DerivedA,typename DerivedB>
   void repmat(
    const Eigen::MatrixBase<DerivedA> & A,
    const int r,
    const int c,
    Eigen::PlainObjectBase<DerivedB> & B);
  /// \overload
  template <typename T, int majorType>
   void repmat(
    const Eigen::SparseMatrix<T, majorType> & A,
    const int r,
    const int c,
    Eigen::SparseMatrix<T, majorType> & B);
