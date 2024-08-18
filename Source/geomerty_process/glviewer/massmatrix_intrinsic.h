#pragma once 
#include "massmatrix.h"

#include <Eigen/Dense>
#include <Eigen/Sparse>


  /// Constructs the mass (area) matrix for a given mesh (V,F).
  ///
  /// @param[in] l  #l by simplex_size list of mesh edge lengths
  /// @param[in] F  #F by simplex_size list of mesh elements (triangles or tetrahedra)
  /// @param[in] type  one of the following ints:
  ///     MASSMATRIX_TYPE_BARYCENTRIC  barycentric
  ///     MASSMATRIX_TYPE_VORONOI voronoi-hybrid {default}
  ///     MASSMATRIX_TYPE_FULL full
  /// @param[out] M  #V by #V mass matrix
  ///
  /// \see massmatrix
  ///
  template <typename Derivedl, typename DerivedF, typename Scalar>
   void massmatrix_intrinsic(
    const Eigen::MatrixBase<Derivedl> & l, 
    const Eigen::MatrixBase<DerivedF> & F, 
    const MassMatrixType type,
    Eigen::SparseMatrix<Scalar>& M);
  /// \overload
  /// @param[in] n  number of vertices (>= F.maxCoeff()+1)
  template <typename Derivedl, typename DerivedF, typename Scalar>
   void massmatrix_intrinsic(
    const Eigen::MatrixBase<Derivedl> & l, 
    const Eigen::MatrixBase<DerivedF> & F, 
    const MassMatrixType type,
    const int n,
    Eigen::SparseMatrix<Scalar>& M);



