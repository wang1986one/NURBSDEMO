#pragma once
#include<glad/glad.h>
#include <Eigen/Core>


  namespace Geomerty
  {

     void create_index_vbo(
      const Eigen::MatrixXi & F,
      GLuint & F_vbo_id);
  }


