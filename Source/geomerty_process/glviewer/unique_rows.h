#pragma once
#include <vector>
#include <Eigen/Core>
 /// Act like matlab's [C,IA,IC] = unique(X,'rows')
  ///
  /// @tparam  DerivedA derived scalar type, e.g. MatrixXi or MatrixXd
  /// @tparam  DerivedIA derived integer type, e.g. MatrixXi
  /// @tparam  DerivedIC derived integer type, e.g. MatrixXi
  /// @param[in] A  m by n matrix whose entries are to unique'd according to rows
  /// @param[out] C  #C vector of unique rows in A
  /// @param[out] IA  #C index vector so that C = A(IA,:);
  /// @param[out] IC  #A index vector so that A = C(IC,:);
  template <typename DerivedA, typename DerivedC, typename DerivedIA, typename DerivedIC>
   void unique_rows(
    const Eigen::DenseBase<DerivedA>& A,
    Eigen::PlainObjectBase<DerivedC>& C,
    Eigen::PlainObjectBase<DerivedIA>& IA,
    Eigen::PlainObjectBase<DerivedIC>& IC);


