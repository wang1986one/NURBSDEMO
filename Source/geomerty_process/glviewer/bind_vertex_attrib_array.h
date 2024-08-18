#pragma once
#include <glad/glad.h>

#include <Eigen/Core>
#include <string>

  namespace opengl
  {

    template <typename Scalar, int RowsAtCompileTime, int ColsAtCompileTime>
     GLint bind_vertex_attrib_array(
      const GLuint program_shader,
      const std::string &name, 
      GLuint bufferID, 
      const Eigen::Matrix<Scalar,RowsAtCompileTime,ColsAtCompileTime,Eigen::RowMajor> &M,
      const bool refresh);
  }

