#pragma once

#include <Eigen/Dense>


  /// Implementation of the deprecated glOrtho function.
  ///
  /// @param[in] left  coordinate of left vertical clipping plane
  /// @param[in] right  coordinate of right vertical clipping plane
  /// @param[in] bottom  coordinate of bottom vertical clipping plane
  /// @param[in] top  coordinate of top vertical clipping plane
  /// @param[in] nearVal  distance to near plane
  /// @param[in] farVal  distance to far plane
  /// @param[out] P  4x4 perspective matrix
  template < typename DerivedP>
  void ortho(
    const typename DerivedP::Scalar left,
    const typename DerivedP::Scalar right,
    const typename DerivedP::Scalar bottom,
    const typename DerivedP::Scalar top,
    const typename DerivedP::Scalar nearVal,
    const typename DerivedP::Scalar farVal,
    Eigen::PlainObjectBase<DerivedP> & P);
