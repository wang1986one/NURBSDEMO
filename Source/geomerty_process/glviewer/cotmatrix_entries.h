#pragma once
#include <Eigen/Core>

  /// Compute the cotmatrix contributions (cotangents) of each angle in mesh (V,F)
  /// 
  /// @param[in] V  #V by dim list of rest domain positions
  /// @param[in] F  #F by {3|4} list of {triangle|tetrahedra} indices into V
  /// @param[out]   C  #F by 3 list of 1/2*cotangents corresponding angles
  ///       for triangles, columns correspond to edges [1,2],[2,0],[0,1]
  ///     or
  ///     C  #F by 6 list of 1/6*cotangents of dihedral angles*edge lengths
  ///       for tets, columns along edges [1,2],[2,0],[0,1],[3,0],[3,1],[3,2] 
  ///
  /// \see cotmatrix
  template <typename DerivedV, typename DerivedF, typename DerivedC>
   void cotmatrix_entries(
    const Eigen::MatrixBase<DerivedV>& V,
    const Eigen::MatrixBase<DerivedF>& F,
    Eigen::PlainObjectBase<DerivedC>& C);
  /// Compute the cotmatrix contributions (cotangents) of each angle in mesh with edge lengths (l)
  /// 
  /// @param[in] l  #F by 3 list of triangle edge lengths (see edge_lengths)
  /// @param[out] C  #F by 3 list of 1/2*cotangents corresponding angles
  ///     for triangles, columns correspond to edges [1,2],[2,0],[0,1]
  ///
  /// \see cotmatrix
  template <typename Derivedl, typename DerivedC>
   void cotmatrix_entries(
    const Eigen::MatrixBase<Derivedl>& l,
    Eigen::PlainObjectBase<DerivedC>& C);

