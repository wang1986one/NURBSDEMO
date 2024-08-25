
#include "load_shader.h"

// Copyright Denis Kovacs 4/10/08
#include "print_shader_info_log.h"
#include <cstdio>
 GLuint Geomerty::load_shader(
  const std::string & src,const GLenum type)
{
  if(src.empty())
  {
    return (GLuint) 0;
  }

  GLuint s = glCreateShader(type);
  if(s == 0)
  {
    fprintf(stderr,"Error: load_shader() failed to create shader.\n");
    return 0;
  }
  // Pass shader source string
  const char *c = src.c_str();
  glShaderSource(s, 1, &c, NULL);
  glCompileShader(s);
  // Print info log (if any)
  Geomerty::print_shader_info_log(s);
  return s;
}
