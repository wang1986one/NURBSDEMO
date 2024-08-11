#pragma once
#include <Eigen/Dense>

  /// Computes the barycenter of every simplex.
  ///
  /// @param[in] V  #V x dim matrix of vertex coordinates
  /// @param[in] F  #F x simplex_size  matrix of indices of simplex corners into V
  /// @param[out] BC  #F x dim matrix of 3d vertices
  ///
  template <
    typename DerivedV,
    typename DerivedF,
    typename DerivedBC>
   void barycenter(
      const Eigen::MatrixBase<DerivedV> & V,
      const Eigen::MatrixBase<DerivedF> & F,
      Eigen::PlainObjectBase<DerivedBC> & BC);

