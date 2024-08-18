#pragma once
  /// Convert a quaternion to a 4x4 matrix
  /// A Quaternion, q, is defined here as an arrays of four scalars (x,y,z,w),
  /// such that q = x*i + y*j + z*k + w
  ///
  /// @param[in] quat  pointer to four elements of quaternion (x,y,z,w)  
  /// @param[out] mat  pointer to 16 elements of matrix
  template <typename Q_type>
  void quat_to_mat(const Q_type * quat, Q_type * mat);
