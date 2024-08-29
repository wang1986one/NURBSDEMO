#pragma once
#include "core/nodes.h"
namespace Geomerty {
	class NurbsCurve_LoadNode :public Node {
	private:
		std::string path;
	public:
		NurbsCurve_LoadNode(int id, const char* name, ImColor color = ImColor(255, 255, 255)) :Node(id, name, color) {}
		void InstallUi() override;
		void Init(Graph* graph) override;
		void Execute(ExetContex* ctx)override;
		void Present(Geomerty::Viewer& viewer)override;
	};

}
