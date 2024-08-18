#pragma

#include <Eigen/Dense>


  /// Constructs a list of squared lengths of edges opposite each index in a face
  /// (triangle/tet) list
  ///
  /// @tparam DerivedV derived from vertex positions matrix type: i.e. MatrixXd
  /// @tparam DerivedF derived from face indices matrix type: i.e. MatrixXi
  /// @tparam DerivedL derived from edge lengths matrix type: i.e. MatrixXd
  /// @param[in] V  eigen matrix #V by 3
  /// @param[in] F  #F by (2|3|4) list of mesh edges, triangles or tets
  /// @param[out] L  #F by {1|3|6} list of edge lengths squared
  ///     for edges, column of lengths
  ///     for triangles, columns correspond to edges [1,2],[2,0],[0,1]
  ///     for tets, columns correspond to edges
  ///     [3 0],[3 1],[3 2],[1 2],[2 0],[0 1]
  ///
  template <typename DerivedV, typename DerivedF, typename DerivedL>
   void squared_edge_lengths(
    const Eigen::MatrixBase<DerivedV>& V,
    const Eigen::MatrixBase<DerivedF>& F,
    Eigen::PlainObjectBase<DerivedL>& L);

