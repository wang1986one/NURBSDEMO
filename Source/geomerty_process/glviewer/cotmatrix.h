#pragma

#include <Eigen/Dense>
#include <Eigen/Sparse>

// History:
//  Used const references rather than copying the entire mesh 
//    Alec 9 October 2011
//  removed cotan (uniform weights) optional parameter it was building a buggy
//    half of the uniform laplacian, please see adjacency_matrix instead 
//    Alec 9 October 2011

  /// Constructs the cotangent stiffness matrix (discrete laplacian) for a given
  /// mesh (V,F).
  ///
  ///   @tparam DerivedV  derived type of eigen matrix for V (e.g. derived from
  ///     MatrixXd)
  ///   @tparam DerivedF  derived type of eigen matrix for F (e.g. derived from
  ///     MatrixXi)
  ///   @tparam Scalar  float type for eigen sparse matrix (e.g. double)
  ///   @param[in] V  #V by dim list of mesh vertex positions
  ///   @param[in] F  #F by simplex_size list of mesh elements (triangles or tetrahedra)
  ///   @param[out] L  #V by #V cotangent matrix, each row i corresponding to V(i,:)
  ///
  /// \see
  ///   adjacency_matrix
  ///
  /// \note This Laplacian uses the convention that diagonal entries are
  /// **minus** the sum of off-diagonal entries. The diagonal entries are
  /// therefore in general negative and the matrix is **negative** semi-definite
  /// (immediately, -L is **positive** semi-definite)
  ///
  template <typename DerivedV, typename DerivedF, typename Scalar>
   void cotmatrix(
    const Eigen::MatrixBase<DerivedV> & V, 
    const Eigen::MatrixBase<DerivedF> & F, 
    Eigen::SparseMatrix<Scalar>& L);
  /// Cotangent Laplacian (and mass matrix) for polygon meshes according to
  /// "Polygon Laplacian Made Simple" [Bunge et al.\ 2020]
  ///
  /// @param[in] V  #V by 3 list of mesh vertex positions
  /// @param[in] I  #I vectorized list of polygon corner indices into rows of some matrix V
  /// @param[in] C  #polygons+1 list of cumulative polygon sizes so that C(i+1)-C(i) = size of
  ///     the ith polygon, and so I(C(i)) through I(C(i+1)-1) are the indices of
  ///     the ith polygon
  /// @param[out] L  #V by #V polygon Laplacian made simple matrix
  /// @param[out] M  #V by #V mass matrix
  /// @param[out] P  #V+#polygons by #V prolongation operator
  template <
    typename DerivedV, 
    typename DerivedI, 
    typename DerivedC, 
    typename Scalar>
   void cotmatrix(
    const Eigen::MatrixBase<DerivedV> & V, 
    const Eigen::MatrixBase<DerivedI> & I, 
    const Eigen::MatrixBase<DerivedC> & C, 
    Eigen::SparseMatrix<Scalar>& L,
    Eigen::SparseMatrix<Scalar>& M,
    Eigen::SparseMatrix<Scalar>& P);

