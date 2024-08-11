#pragma once
#include <Eigen/Dense>

  /// Find *combinatorially* unique simplices in F.  **Order independent**
  ///
  /// @param[in] F  #F by simplex-size list of simplices
  /// @param[out] FF  #FF by simplex-size list of unique simplices in F
  /// @param[out] IA  #FF index vector so that FF == sort(F(IA,:),2);
  /// @param[out] IC  #F index vector so that sort(F,2) == FF(IC,:);
  template <
    typename DerivedF,
    typename DerivedFF,
    typename DerivedIA,
    typename DerivedIC>
   void unique_simplices(
    const Eigen::MatrixBase<DerivedF>& F,
    Eigen::PlainObjectBase<DerivedFF>& FF,
    Eigen::PlainObjectBase<DerivedIA>& IA,
    Eigen::PlainObjectBase<DerivedIC>& IC);
  /// \overload
  template <
    typename DerivedF,
    typename DerivedFF>
   void unique_simplices(
    const Eigen::MatrixBase<DerivedF>& F,
    Eigen::PlainObjectBase<DerivedFF>& FF);
