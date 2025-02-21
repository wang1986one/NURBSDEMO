#pragma once
#include <Eigen/Core>
#include<glad/glad.h>

namespace Geomerty
{
  /// Read full viewport into color, alpha and depth arrays suitable for
  /// igl::png::writePNG
  ///
  /// @param[in] width  width of viewport
  /// @param[in] height height of viewport
  /// @param[out] R  width by height list of red values
  /// @param[out] G  width by height list of green values
  /// @param[out] B  width by height list of blue values
  /// @param[out] A  width by height list of alpha values
  /// @param[out] D  width by height list of depth values
  template <typename T>
  void read_pixels(
    const GLuint width,
    const GLuint height,
    Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic> & R,
    Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic> & G,
    Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic> & B,
    Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic> & A,
    Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic> & D);
}
