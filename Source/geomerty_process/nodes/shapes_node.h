#pragma once
#include "core/nodes.h"
namespace Geomerty {

	class ShapeNode :public Node {
	private:
		int item_current_idx = 0;
	public:
		ShapeNode(int id, const char* name, ImColor color = ImColor(255, 255, 255)) :Node(id, name, color) {}
		void InstallUi() override;
		void Init(Graph* graph) override;
		void Execute(ExetContex* ctx)override;
		void Present(opengl::glfw::Viewer& viewer)override;
	};


}