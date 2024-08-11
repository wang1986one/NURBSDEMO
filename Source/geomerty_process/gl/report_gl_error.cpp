#include<glad/glad.h>
#include "report_gl_error.h"

#include <cstdio>

unsigned int opengl::report_gl_error(const std::string id)
{
  // http://stackoverflow.com/q/28485180/148668

  // gluErrorString was deprecated
  const auto gluErrorString = [](unsigned int errorCode)->const char *
  {
    switch(errorCode)
    {
      default:
        return "unknown error code";
      case GL_NO_ERROR:
        return "no error";
      case GL_INVALID_ENUM:
        return "invalid enumerant";
      case GL_INVALID_VALUE:
        return "invalid value";
      case GL_INVALID_OPERATION:
        return "invalid operation";
#ifndef GL_VERSION_3_0
      case GL_STACK_OVERFLOW:
        return "stack overflow";
      case GL_STACK_UNDERFLOW:
        return "stack underflow";
      case GL_TABLE_TOO_LARGE:
        return "table too large";
#endif
      case GL_OUT_OF_MEMORY:
        return "out of memory";
#ifdef GL_EXT_framebuffer_object
      case GL_INVALID_FRAMEBUFFER_OPERATION_EXT:
        return "invalid framebuffer operation";
#endif
    }
  };

  GLenum err = glGetError();
  if(GL_NO_ERROR != err)
  {

    fprintf(stderr,"%s%s\n",id.c_str(),gluErrorString(err));
  }
  return err;
}

 GLenum opengl::report_gl_error()
{
  return opengl::report_gl_error(std::string(""));
}

