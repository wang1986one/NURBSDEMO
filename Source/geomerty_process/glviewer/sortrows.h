#pragma once
#include <vector>
#include <Eigen/Core>

  /// Act like matlab's [Y,I] = sortrows(X)
  ///
  /// @tparam DerivedX derived float type, e.g. MatrixXi or MatrixXd
  /// @tparam DerivedI derived integer type, e.g. MatrixXi
  /// @param[in] X  m by n matrix whose entries are to be sorted
  /// @param[in] ascending  sort ascending (true, matlab default) or descending (false)
  /// @param[out] Y  m by n matrix whose entries are sorted (**should not** be same
  ///     reference as X)
  /// @param[out] I  m list of indices so that Y = X(I,:);
  template <typename DerivedX, typename DerivedY,typename DerivedI>
   void sortrows(
    const Eigen::DenseBase<DerivedX>& X,
    const bool ascending,
    Eigen::PlainObjectBase<DerivedY>& Y,
    Eigen::PlainObjectBase<DerivedI>& I);
  /// \overload
  template <typename DerivedX, typename DerivedY>
   void sortrows(
    const Eigen::DenseBase<DerivedX>& X,
    const bool ascending,
    Eigen::PlainObjectBase<DerivedY>& Y);
