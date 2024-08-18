#pragma once 

  /// Convert axis angle representation of a rotation to a quaternion.
  /// A Quaternion, q, is defined here as an arrays of four floats (x,y,z,w),
  ///
  /// such that q = x*i + y*j + z*k + w
  /// @param[in] axis  3d vector
  /// @param[in] angle  float
  /// @param[out] out  pointer to new quaternion
  ///
  /// \deprecated Use `Eigen::AngleAxisd` instead
  template <typename Q_type>
  void axis_angle_to_quat(
    const Q_type *axis, 
    const Q_type angle,
    Q_type *out);

