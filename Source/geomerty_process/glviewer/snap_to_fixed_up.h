#pragma once
#include <Eigen/Core>
#include <Eigen/Geometry>


  /// Snap an arbitrary rotation to a rotation resulting from a rotation about
  /// the y-axis then the x-axis (maintaining fixed up like
  /// two_axis_valuator_fixed_up.)
  ///
  /// @param[in] q  General rotation as quaternion
  /// @param[out] s the resulting rotation (as quaternion)
  ///
  /// \see two_axis_valuator_fixed_up
  template <typename Qtype>
  void snap_to_fixed_up(
    const Eigen::Quaternion<Qtype> & q,
    Eigen::Quaternion<Qtype> & s);

