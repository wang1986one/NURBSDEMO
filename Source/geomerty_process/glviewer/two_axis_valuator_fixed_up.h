#pragma once
#include <Eigen/Core>
#include <Eigen/Geometry>

  /// Applies a two-axis valuator drag rotation (as seen in Maya/Studio max) to a given rotation.
  ///
  /// @param[in] w  width of the trackball context
  /// @param[in] h  height of the trackball context
  /// @param[in] speed  controls how fast the trackball feels, 1 is normal
  /// @param[in] down_quat  rotation at mouse down, i.e. the rotation we're applying the
  ///     trackball motion to (as quaternion). **Note:** Up-vector that is fixed
  ///     is with respect to this rotation.
  /// @param[in] down_x position of mouse down
  /// @param[in] down_y position of mouse down
  /// @param[in] mouse_x  current x position of mouse
  /// @param[in] mouse_y  current y position of mouse
  /// @param[out] quat  the resulting rotation (as quaternion)
  ///
  /// \see snap_to_fixed_up
  template <typename Scalardown_quat, typename Scalarquat>
  void two_axis_valuator_fixed_up(
    const int w,
    const int h,
    const double speed,
    const Eigen::Quaternion<Scalardown_quat> & down_quat,
    const int down_x,
    const int down_y,
    const int mouse_x,
    const int mouse_y,
    Eigen::Quaternion<Scalarquat> & quat);


