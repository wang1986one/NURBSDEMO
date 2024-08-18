#pragma once


#include <string>


  namespace opengl
  {

      unsigned int report_gl_error(const std::string id);
    /// \overload
      unsigned int report_gl_error();
  }




