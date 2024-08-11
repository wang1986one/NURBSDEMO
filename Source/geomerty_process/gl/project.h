#pragma once
#include <Eigen/Dense>

  /// Eigen reimplementation of gluProject
  /// @param[in] obj*  3D objects' x, y, and z coordinates respectively
  /// @param[in] model  model matrix
  /// @param[in] proj  projection matrix
  /// @param[in] viewport  viewport vector
  /// @return screen space x, y, and z coordinates respectively
  template <typename Scalar>
   Eigen::Matrix<Scalar,3,1> project(
    const    Eigen::Matrix<Scalar,3,1>&  obj,
    const    Eigen::Matrix<Scalar,4,4>& model,
    const    Eigen::Matrix<Scalar,4,4>& proj,
    const    Eigen::Matrix<Scalar,4,1>&  viewport);
  /// \overload
  ///
  /// @param[in] V  #V by 3 list of object points
  /// @param[in] model  model matrix
  /// @param[in] proj  projection matrix
  /// @param[in] viewport  viewport vector
  /// @param[out] P  #V by 3 list of screen space points
  ///
  /// \bug The compiler will not complain if V and P are Vector3d, but the result
  ///   will be incorrect.
  ///
  /// ####Example:
  /// \code{cpp}
  ///   igl::opengl::glfw::Viewer vr;
  ///   ...
  ///   igl::project(V,vr.core().view,vr.core().proj,vr.core().viewport,P);
  /// \endcode
  template <typename DerivedV, typename DerivedM, typename DerivedN, typename DerivedO, typename DerivedP>
   void project(
    const    Eigen::MatrixBase<DerivedV>&  V,
    const    Eigen::MatrixBase<DerivedM>& model,
    const    Eigen::MatrixBase<DerivedN>& proj,
    const    Eigen::MatrixBase<DerivedO>&  viewport,
    Eigen::PlainObjectBase<DerivedP> & P);
