
#include<glad/glad.h>


  namespace opengl
  {
    /// Properly destroy a shader program. Detach and delete each of its shaders
    /// and delete it
    ///
    /// @param[in] id  index id of created shader, set to 0 on error
    /// @return true on success, false on error
    /// 
    /// \note caller is responsible for making sure he doesn't foolishly continue
    /// to use id as if it still contains a program
    /// 
    /// \see create_shader_program
     bool destroy_shader_program(const GLuint id);
  }


