#pragma once
#include <Eigen/Core>

  /// Compute dihedral angles for all tets of a given tet mesh (V,T).
  ///
  /// @param[in] V  #V by dim list of vertex positions
  /// @param[in] T  #V by 4 list of tet indices
  /// @param[out] theta  #T by 6 list of dihedral angles (in radians)
  /// @param[out] cos_theta  #T by 6 list of cosine of dihedral angles (in radians)
  ///
  template <
    typename DerivedV, 
    typename DerivedT, 
    typename Derivedtheta,
    typename Derivedcos_theta>
   void dihedral_angles(
    const Eigen::MatrixBase<DerivedV>& V,
    const Eigen::MatrixBase<DerivedT>& T,
    Eigen::PlainObjectBase<Derivedtheta>& theta,
    Eigen::PlainObjectBase<Derivedcos_theta>& cos_theta);
  /// \overload
  /// 
  /// \brief Intrinsic version.
  ///
  /// @param[in] L  #L by 6 list of edge lengths
  /// @param[in] A  #A by 4 list of face areas
  template <
    typename DerivedL, 
    typename DerivedA, 
    typename Derivedtheta,
    typename Derivedcos_theta>
   void dihedral_angles_intrinsic(
    const Eigen::MatrixBase<DerivedL>& L,
    const Eigen::MatrixBase<DerivedA>& A,
    Eigen::PlainObjectBase<Derivedtheta>& theta,
    Eigen::PlainObjectBase<Derivedcos_theta>& cos_theta);

