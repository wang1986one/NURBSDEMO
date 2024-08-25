#pragma once
#include "core/nodes.h"
namespace Geomerty {
	class Parametrization_Node :public Node {
	private:
		bool use_uniform = false;

	public:
		Parametrization_Node(int id, const char* name, ImColor color = ImColor(255, 255, 255)) :Node(id, name, color) {}
		void InstallUi() override;
		void Init(Graph* graph) override;
		void Execute(ExetContex* ctx)override;
		void Present(Geomerty::Viewer& viewer)override;
	};

}
