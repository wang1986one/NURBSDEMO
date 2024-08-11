#pragma once
#include <Eigen/Geometry>
// A Quaternion, q, is defined here as an arrays of four scalars (x,y,z,w),
// such that q = x*i + y*j + z*k + w

  /// Snap the quaternion q to the nearest canonical view quaternion
  ///
  /// @param[in] q  quaternion to be snapped (also see Outputs)
  /// @param[in] threshold  (optional) threshold:
  ///     1.0 --> snap any input
  ///     0.5 --> snap inputs somewhat close to canonical views
  ///     0.0 --> snap no input
  /// @param[out] q  quaternion possibly set to nearest canonical view
  /// @return true only if q was snapped to the nearest canonical view
  template <typename Scalarq, typename Scalars>
   bool snap_to_canonical_view_quat(
    const Eigen::Quaternion<Scalarq> & q,
    const double threshold,
    Eigen::Quaternion<Scalars> & s);
  /// \overload
  template <typename Q_type>
   bool snap_to_canonical_view_quat(
    const Q_type* q,
    const Q_type threshold,
    Q_type* s);
