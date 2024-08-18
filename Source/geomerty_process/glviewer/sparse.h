#pragma once
#define EIGEN_YES_I_KNOW_SPARSE_MODULE_IS_NOT_STABLE_YET
#include <Eigen/Dense>
#include <Eigen/Sparse>

  /// Build a sparse matrix from list of indices and values (I,J,V), functions
  /// like the sparse function in matlab
  ///
  /// @tparam IndexVector  list of indices, value should be non-negative and should
  ///     expect to be cast to an index. Must implement operator(i) to retrieve
  ///     ith element
  /// @tparam ValueVector  list of values, value should be expect to be cast to type
  ///     T. Must implement operator(i) to retrieve ith element
  /// @tparam T  should be a eigen sparse matrix primitive type like int or double
  /// @param[in] I  nnz vector of row indices of non zeros entries in X
  /// @param[in] J  nnz vector of column indices of non zeros entries in X
  /// @param[in] V  nnz vector of non-zeros entries in X
  /// @param[in] m  number of rows
  /// @param[in] n  number of cols
  /// @param[out] X  m by n matrix of type T whose entries are to be found 
  ///
  /// \deprecated just use Eigen::SparseMatrix<>.setFromTriplets()
  template <
    class IndexVectorI, 
    class IndexVectorJ, 
    class ValueVector, 
    typename T>
   void sparse(
    const IndexVectorI & I,
    const IndexVectorJ & J,
    const ValueVector & V,
    const size_t m,
    const size_t n,
    Eigen::SparseMatrix<T>& X);
  /// \overload
  template <class IndexVector, class ValueVector, typename T>
   void sparse(
    const IndexVector & I,
    const IndexVector & J,
    const ValueVector & V,
    Eigen::SparseMatrix<T>& X);
  /// Convert a full, dense matrix to a sparse one
  ///
  /// \note Just use .sparseView()
  ///
  /// @tparam T  should be a eigen sparse matrix primitive type like int or double
  /// @param[in] D  m by n full, dense matrix
  /// @param[out] X  m by n sparse matrix
  template <typename DerivedD, typename T>
   void sparse(
    const Eigen::PlainObjectBase<DerivedD>& D,
    Eigen::SparseMatrix<T>& X);
  /// \overload
  template <typename DerivedD>
   Eigen::SparseMatrix<typename DerivedD::Scalar > sparse(
    const Eigen::PlainObjectBase<DerivedD>& D);


