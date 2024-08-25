#pragma once
#include "core/nodes.h"
namespace Geomerty {

	class Read_MeshNode :public Node {
	public:
		Read_MeshNode(int id, const char* name, ImColor color = ImColor(255, 255, 255)) :Node(id, name, color) {}
		void InstallUi() override;
		void Init(Graph* graph) override;
		void Execute(ExetContex* ctx)override;
		void Present(Geomerty::Viewer& viewer)override;
	};


}
