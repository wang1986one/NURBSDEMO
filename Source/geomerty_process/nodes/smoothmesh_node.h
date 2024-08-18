#pragma once
#include "core/nodes.h"
namespace Geomerty {
	class Smooth_MeshNode :public Node {
	private:
		int iterations = 2;
		bool use_uniform = false;
	public:
		Smooth_MeshNode(int id, const char* name, ImColor color = ImColor(255, 255, 255)) :Node(id, name, color) {}
		void InstallUi() override;
		void Init(Graph* graph) override;
		void Execute(ExetContex* ctx)override;
		void Present(opengl::glfw::Viewer& viewer)override;
	};

}
