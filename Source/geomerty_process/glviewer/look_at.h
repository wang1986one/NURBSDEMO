#pragma once

#include <Eigen/Dense>


  /// Implementation of the deprecated gluLookAt function.
  ///
  /// @param[in] eye  3-vector of eye position
  /// @param[in] center  3-vector of center reference point
  /// @param[in] up  3-vector of up vector
  /// @param[out] R  4x4 rotation matrix
  ///
  template <
    typename Derivedeye,
    typename Derivedcenter,
    typename Derivedup,
    typename DerivedR >
  void look_at(
    const Eigen::PlainObjectBase<Derivedeye> & eye,
    const Eigen::PlainObjectBase<Derivedcenter> & center,
    const Eigen::PlainObjectBase<Derivedup> & up,
    Eigen::PlainObjectBase<DerivedR> & R);

