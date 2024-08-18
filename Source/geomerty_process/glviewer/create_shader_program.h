
#include<glad/glad.h>
#include <string>
#include <map>


  namespace opengl
  {
    /// Create a shader program with a vertex and fragments shader loading from
    /// source strings and vertex attributes assigned from a map before linking the
    /// shaders to the program, making it ready to use with glUseProgram(id)
    ///
    /// @param[in] geom_source  string containing source code of geometry shader (can be
    ///     "" to mean use default pass-through)
    /// @param[in] vert_source  string containing source code of vertex shader
    /// @param[in] frag_source  string containing source code of fragment shader
    /// @param[in] attrib  map containing table of vertex attribute strings add their
    ///            correspondingly ids (generated previously using glBindAttribLocation)
    /// @param[out] id  index id of created shader, set to 0 on error
    /// @return true on success, false on error
    ///
    /// \note Caller is responsible for making sure that current value of id is not
    /// leaking a shader (since it will be overwritten)
    ///
    /// \see destroy_shader_program
     bool create_shader_program(
      const std::string &geom_source,
      const std::string &vert_source,
      const std::string &frag_source,
      const std::map<std::string,GLuint> &attrib,
      GLuint & id);
    /// \overload
     bool create_shader_program(
      const std::string &vert_source,
      const std::string &frag_source,
      const std::map<std::string,GLuint> &attrib,
      GLuint & id);
    /// \overload
    ///
    /// @return index id of created shader, set to 0 on error
    GLuint create_shader_program(
      const std::string & geom_source,
      const std::string & vert_source,
      const std::string & frag_source,
      const std::map<std::string,GLuint> &attrib);
    /// \overload
    ///
    /// @return index id of created shader, set to 0 on error
     GLuint create_shader_program(
      const std::string & vert_source,
      const std::string & frag_source,
      const std::map<std::string,GLuint> &attrib);
  }

