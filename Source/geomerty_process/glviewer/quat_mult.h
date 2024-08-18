#pragma once

  /// Computes out = q1 * q2 with quaternion multiplication
  /// A Quaternion, q, is defined here as an arrays of four floats (x,y,z,w),
  /// such that q = x*i + y*j + z*k + w
  ///
  /// @param[in] q1  left quaternion
  /// @param[in] q2  right quaternion
  /// @param[out] out  result of multiplication
  ///
  template <typename Q_type>
  void quat_mult(
    const Q_type *q1, 
    const Q_type *q2,
    Q_type *out);
