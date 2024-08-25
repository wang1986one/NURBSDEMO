#pragma once
#include<glad/glad.h>
#include <string>


  namespace Geomerty
  {
    /// Creates and compiles a shader from a given string
    ///
    /// @param[in] src  string containing GLSL shader code
    /// @param[in] type  GLSL type of shader, one of:
    ///     GL_VERTEX_SHADER
    ///     GL_FRAGMENT_SHADER
    ///     GL_GEOMETRY_SHADER
    /// @return index id of the newly created shader, 0 on error
    /// 
    /// Will immediately return 0 if src is empty.
    GLuint load_shader(
      const std::string & src,const GLenum type);
  }


