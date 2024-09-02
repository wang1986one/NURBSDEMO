#pragma once
#include "core/nodes.h"
namespace Geomerty {
	class NurbsCurve_SplitNode :public Node {
	private:
		float u = 0.5f;
	public:
		NurbsCurve_SplitNode(int id, const char* name, ImColor color = ImColor(255, 255, 255)) :Node(id, name, color) {
		}
		void InstallUi() override;
		void Init(Graph* graph) override;
		void Execute(ExetContex* ctx)override;
		void Present(Geomerty::Viewer& viewer)override;
	};

}
