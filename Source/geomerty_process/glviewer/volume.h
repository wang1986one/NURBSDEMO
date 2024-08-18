#pragma once
#include <Eigen/Core>

  /// Compute volume for all tets of a given tet mesh (V,T)
  ///
  /// @param[in] V  #V by dim list of vertex positions
  /// @param[in] T  #T by 4 list of tet indices
  /// @param[out] vol  #T list of tetrahedron volumes
  ///
  template <
    typename DerivedV, 
    typename DerivedT, 
    typename Derivedvol>
   void volume(
    const Eigen::MatrixBase<DerivedV>& V,
    const Eigen::MatrixBase<DerivedT>& T,
    Eigen::PlainObjectBase<Derivedvol>& vol);
  /// \overload
  /// @param[in] A  #V by dim list of first corner position
  /// @param[in] B  #V by dim list of second corner position
  /// @param[in] C  #V by dim list of third corner position
  /// @param[in] D  #V by dim list of fourth corner position
  template <
    typename DerivedA,
    typename DerivedB,
    typename DerivedC,
    typename DerivedD,
    typename Derivedvol>
   void volume(
    const Eigen::MatrixBase<DerivedA> & A,
    const Eigen::MatrixBase<DerivedB> & B,
    const Eigen::MatrixBase<DerivedC> & C,
    const Eigen::MatrixBase<DerivedD> & D,
    Eigen::PlainObjectBase<Derivedvol> & vol);
  /// \overload
  /// \brief Single tet
  template <
    typename VecA,
    typename VecB,
    typename VecC,
    typename VecD>
   typename VecA::Scalar volume_single(
    const VecA & a,
    const VecB & b,
    const VecC & c,
    const VecD & d);
  /// \overload
  /// \brief Intrinsic version:
  ///
  /// @param[in] L  #V by 6 list of edge lengths (see edge_lengths)
  template <
    typename DerivedL, 
    typename Derivedvol>
   void volume(
    const Eigen::MatrixBase<DerivedL>& L,
    Eigen::PlainObjectBase<Derivedvol>& vol);



