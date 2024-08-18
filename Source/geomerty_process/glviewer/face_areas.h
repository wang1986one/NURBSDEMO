#pragma once
#include <Eigen/Dense>

  /// Constructs a list of face areas of faces opposite each index in a tet list
  ///
  /// @param[in] V  #V by 3 list of mesh vertex positions
  /// @param[in] T  #T by 3 list of tet mesh indices into V
  /// @param[out] A   #T by 4 list of face areas corresponding to faces opposite vertices
  ///     0,1,2,3
  ///
  template <typename DerivedV, typename DerivedT, typename DerivedA>
   void face_areas(
    const Eigen::MatrixBase<DerivedV>& V,
    const Eigen::MatrixBase<DerivedT>& T,
    Eigen::PlainObjectBase<DerivedA>& A);
  /// \overload
  /// \brief Compute tet-mesh face areas from edge lengths.
  ///
  /// @param[in] L  #T by 6 list of tet-mesh edge lengths corresponding to edges
  ///     [3 0],[3 1],[3 2],[1 2],[2 0],[0 1]
  template <typename DerivedL, typename DerivedA>
   void face_areas(
    const Eigen::MatrixBase<DerivedL>& L,
    Eigen::PlainObjectBase<DerivedA>& A);
  /// \overload
  ///
  /// @param[in] doublearea_nan_replacement  See doublearea.h
  template <typename DerivedL, typename DerivedA>
   void face_areas(
    const Eigen::MatrixBase<DerivedL>& L,
    const typename DerivedL::Scalar doublearea_nan_replacement,
    Eigen::PlainObjectBase<DerivedA>& A);



