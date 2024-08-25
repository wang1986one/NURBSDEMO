#pragma once
#include "MeshGL.h"
#include "ViewerCore.h"
#include "ViewerData.h"


#include <Eigen/Core>
#include <Eigen/Geometry>

#include <vector>
#include <string>
#include <cstdint>

namespace Geomerty
{
	class Viewer
	{
	public:
		// UI Enumerations
		enum class MouseButton { Left, Middle, Right };
		enum class MouseMode { None, Rotation, Zoom, Pan, Translation } mouse_mode;
		Eigen::Quaternionf down_rotation;
		int current_mouse_x;
		int current_mouse_y;
		int down_mouse_x;
		int down_mouse_y;
		float down_mouse_z;
		Eigen::Vector3f down_translation;
		bool down;
		bool hack_never_moved = true;
		int width = 1;
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
		template <typename T>
		void draw_buffer(
			// can't be const because of writing in and out of `core.viewport`
			/*const*/ Geomerty::ViewerCore& core,
			Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>& R,
			Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>& G,
			Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>& B,
			Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>& A,
			Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>& D);
		// OpenGL context resize
		void resize(int w, int h); // explicitly set window size
		void post_resize(int w, int h); // external resize due to user interaction
		// Helper functions
		void snap_to_canonical_quaternion();
		////////////////////////
		// Multi-mesh methods //
		////////////////////////
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

		bool erase_core(const size_t index);

		// Retrieve viewport index from its unique identifier
		// Returns 0 if not found
		size_t core_index(const int id) const;
	public:
		//////////////////////
		// Member variables //
		//////////////////////
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

