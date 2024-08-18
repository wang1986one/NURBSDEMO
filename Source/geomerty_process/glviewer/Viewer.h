#pragma once

#ifndef IGL_OPENGL_4
#define IGL_OPENGL_4
#endif

#include "MeshGL.h"
#include "ViewerCore.h"
#include "ViewerData.h"


#include <Eigen/Core>
#include <Eigen/Geometry>

#include <vector>
#include <string>
#include <cstdint>





namespace opengl
{
namespace glfw
{
  // GLFW-based mesh viewer
  class Viewer
  {
  public:
    // UI Enumerations
    enum class MouseButton {Left, Middle, Right};
    enum class MouseMode { None, Rotation, Zoom, Pan, Translation} mouse_mode;
    Eigen::Quaternionf down_rotation;
    int current_mouse_x;
    int current_mouse_y;
    int down_mouse_x;
    int down_mouse_y;
    float down_mouse_z;
    Eigen::Vector3f down_translation;
    bool down;
    bool hack_never_moved=true;
    int width=1;
    int height = 1;
    bool mouse_down(MouseButton button);
    bool mouse_up(MouseButton button);
    bool mouse_move(int mouse_x, int mouse_y);
    bool mouse_scroll(float delta_y);
    void init();

    Viewer();
    ~Viewer();
    // Mesh IO
    
    // Draw everything
     void draw();
    // Render given ViewerCore to a buffer. The width and height are determined
    // by non-zeros dimensions of R or – if both are zero — are set to this
    // core's viewport sizes. Other buffers are resized to fit if needed.
    //
    // Template:
    //   T  image storage type, e.g., unsigned char (values ∈ [0,255]), double
    //     (values ∈ [0.0,1.0]).
    // Inputs:
    //   data  which ViewerData to draw
    //   update_matrices  whether to update view, proj, and norm matrices in
    //     shaders
    // Outputs:
    //   R  width by height red pixel color values
    //   G  width by height green pixel color values
    //   B  width by height blue pixel color values
    //   A  width by height alpha pixel color values
    //   D  width by height depth pixel values. Depth values are _not_
    //     anti-aliased like RGBA.
    //
    template <typename T>
     void draw_buffer(
      // can't be const because of writing in and out of `core.viewport`
      /*const*/ opengl::ViewerCore & core, 
      Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic> & R,
      Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic> & G,
      Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic> & B,
      Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic> & A,
      Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic> & D);
    // OpenGL context resize
     void resize(int w,int h); // explicitly set window size
     void post_resize(int w,int h); // external resize due to user interaction
    // Helper functions
     void snap_to_canonical_quaternion();


    ////////////////////////
    // Multi-mesh methods //
    ////////////////////////

    // Return the current mesh, or the mesh corresponding to a given unique identifier
    //
    // Inputs:
    //   mesh_id  unique identifier associated to the desired mesh (current mesh if -1)
     ViewerData& data(int mesh_id = -1);
     const ViewerData& data(int mesh_id = -1) const;

    // Append a new "slot" for a mesh (i.e., create empty entries at the end of
    // the data_list and opengl_state_list.
    //
    // Inputs:
    //   visible  If true, the new mesh is set to be visible on all existing viewports
    // Returns the id of the last appended mesh
    //
    // Side Effects:
    //   selected_data_index is set this newly created, last entry (i.e.,
    //   #meshes-1)
     int append_mesh(bool visible = true);

    // Erase a mesh (i.e., its corresponding data and state entires in data_list
    // and opengl_state_list)
    //
    // Inputs:
    //   index  index of mesh to erase
    // Returns whether erasure was successful <=> cannot erase last mesh
    //
    // Side Effects:
    //   If selected_data_index is greater than or equal to index then it is
    //   decremented
    // Example:
    //   // Erase all mesh slots except first and clear remaining mesh
    //   viewer.selected_data_index = viewer.data_list.size()-1;
    //   while(viewer.erase_mesh(viewer.selected_data_index)){};
    //   viewer.data().clear();
    //
     bool erase_mesh(const size_t index);

    // Retrieve mesh index from its unique identifier
    // Returns 0 if not found
     size_t mesh_index(const int id) const;

    ////////////////////////////
    // Multi-viewport methods //
    ////////////////////////////

    // Return the current viewport, or the viewport corresponding to a given unique identifier
    //
    // Inputs:
    //   core_id  unique identifier corresponding to the desired viewport (current viewport if 0)
     ViewerCore& core(unsigned core_id = 0);
     const ViewerCore& core(unsigned core_id = 0) const;

    // Append a new "slot" for a viewport (i.e., copy properties of the current viewport, only
    // changing the viewport size/position)
    //
    // Inputs:
    //   viewport      Vector specifying the viewport origin and size in screen coordinates.
    //   append_empty  If true, existing meshes are hidden on the new viewport.
    //
    // Returns the unique id of the newly inserted viewport. There can be a maximum of 31
    //   viewports created in the same viewport. Erasing a viewport does not change the id of
    //   other existing viewports
     int append_core(Eigen::Vector4f viewport, bool append_empty = false);

    // Erase a viewport
    //
    // Inputs:
    //   index  index of the viewport to erase
     bool erase_core(const size_t index);

    // Retrieve viewport index from its unique identifier
    // Returns 0 if not found
     size_t core_index(const int id) const;



public:
    //////////////////////
    // Member variables //
    //////////////////////

    // Alec: I call this data_list instead of just data to avoid confusion with
    // old "data" variable.
    // Stores all the data that should be visualized
    std::vector<ViewerData> data_list;

    size_t selected_data_index;
    int next_data_id;


    // Stores all the viewing options
    std::vector<ViewerCore> core_list;
    size_t selected_core_index;
    int next_core_id;
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  };

} // end namespace
} // end namespace

