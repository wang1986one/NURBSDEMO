#pragma once
#include <Eigen/Dense>
#include <Eigen/Sparse>

  /// Type of mass matrix
  enum MassMatrixType
  {
    /// Lumping area of each element to corner vertices in equal parts
    MASSMATRIX_TYPE_BARYCENTRIC = 0,
    /// Lumping area by Voronoi dual area (clamped to be positive according to
    /// Meyer et al. 2003)
    MASSMATRIX_TYPE_VORONOI = 1,
    /// Full (non-diagonal mass matrix) for piecewise linear functions
    MASSMATRIX_TYPE_FULL = 2,
    /// Use MASSMATRIX_TYPE_VORONOI for triangles and MASSMATRIX_TYPE_BARYCENTRIC
    /// for tetrahedra
    MASSMATRIX_TYPE_DEFAULT = 3,
    /// Total number of mass matrix types
    NUM_MASSMATRIX_TYPES = 4
  };

  /// Constructs the mass (area) matrix for a given mesh (V,F).
  ///
  /// @tparam DerivedV  derived type of eigen matrix for V (e.g. derived from
  ///     MatrixXd)
  /// @tparam DerivedF  derived type of eigen matrix for F (e.g. derived from
  ///     MatrixXi)
  /// @tparam Scalar  float type for eigen sparse matrix (e.g. double)
  /// @param[in] V  #V by dim list of mesh vertex positions
  /// @param[in] F  #F by simplex_size list of mesh elements (triangles or tetrahedra)
  /// @param[in] type  one of the following ints:
  ///     MASSMATRIX_TYPE_BARYCENTRIC  barycentric {default for tetrahedra}
  ///     MASSMATRIX_TYPE_VORONOI voronoi-hybrid {default for triangles}
  ///     MASSMATRIX_TYPE_FULL full
  /// @param[out] M  #V by #V mass matrix
  ///
  /// \see cotmatrix
  ///
  template <typename DerivedV, typename DerivedF, typename Scalar>
   void massmatrix(
    const Eigen::MatrixBase<DerivedV> & V, 
    const Eigen::MatrixBase<DerivedF> & F, 
    const MassMatrixType type,
    Eigen::SparseMatrix<Scalar>& M);
