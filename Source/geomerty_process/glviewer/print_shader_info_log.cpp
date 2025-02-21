
#include "print_shader_info_log.h"

#include <cstdio>
#include <stdlib.h>
// Copyright Denis Kovacs 4/10/08
 void Geomerty::print_shader_info_log(const GLuint obj)
{
  GLint infologLength = 0;
  GLint charsWritten  = 0;
  char *infoLog;
  
  // Get shader info log from Geomerty
  glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);
  // Only print if there is something in the log
  if (infologLength > 0)
  {
    infoLog = (char *)malloc(infologLength);
    glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
    printf("%s\n",infoLog);
    free(infoLog);
  }
}
