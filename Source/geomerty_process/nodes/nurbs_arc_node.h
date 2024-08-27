#pragma once
#include "core/nodes.h"
namespace Geomerty {
	class NurbsArc_Node :public Node {
	private:

		//(const vec3& center, vec3 xaxis, vec3 yaxis, scalar start_angle, scalar end_angle)
		vec3 center{ 0,0,0 };
		vec3 xaxis{ 1,0,0 };
		vec3 yaxis{ 0,1,0 };
		float start_angle{ 0.0 };
		float end_angle{ 6.28 };

	public:
		NurbsArc_Node(int id, const char* name, ImColor color = ImColor(255, 255, 255)) :Node(id, name, color) {}
		void InstallUi() override;
		void Init(Graph* graph) override;
		void Execute(ExetContex* ctx)override;
		void Present(Geomerty::Viewer& viewer)override;
	};

}
