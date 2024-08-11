#pragma once
#include <Eigen/Core>

  /// Eigen reimplementation of gluUnproject
  ///
  /// @param[in] win  #P by 3 or 3-vector (#P=1) of screen space x, y, and z coordinates
  /// @param[in] model  4x4 model-view matrix
  /// @param[in] proj  4x4 projection matrix
  /// @param[in] viewport  4-long viewport vector
  /// @param[out] scene  #P by 3 or 3-vector (#P=1) the unprojected x, y, and z coordinates
  ///
  /// \bug The compiler will not complain if V and P are Vector3d, but the
  /// result will be incorrect.
  template <
    typename Derivedwin,
    typename Derivedmodel,
    typename Derivedproj,
    typename Derivedviewport,
    typename Derivedscene>
   void unproject(
    const Eigen::MatrixBase<Derivedwin>&  win,
    const Eigen::MatrixBase<Derivedmodel>& model,
    const Eigen::MatrixBase<Derivedproj>& proj,
    const Eigen::MatrixBase<Derivedviewport>&  viewport,
    Eigen::PlainObjectBase<Derivedscene> & scene);
  /// \overload
  template <typename Scalar>
   Eigen::Matrix<Scalar,3,1> unproject(
    const Eigen::Matrix<Scalar,3,1>&  win,
    const Eigen::Matrix<Scalar,4,4>& model,
    const Eigen::Matrix<Scalar,4,4>& proj,
    const Eigen::Matrix<Scalar,4,1>&  viewport);
