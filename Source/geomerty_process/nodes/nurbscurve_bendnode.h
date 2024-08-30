#pragma once
#include "core/nodes.h"


namespace Geomerty {
	class NurbsCurve_BendNode :public Node {
	private:
		//
		bool bend_flag = false;
		float start_parameter = 0.01f;
		float end_parameter = 0.99f;
		float radius = 1.0f;
		float cross_ratio = 0.1f;
		Eigen::Vector3<float>bend_center = Eigen::Vector3<float>::Zero();
	public:
		NurbsCurve_BendNode(int id, const char* name, ImColor color = ImColor(255, 255, 255)) :Node(id, name, color) {

		}
		void InstallUi() override;
		void Init(Graph* graph) override;
		void Execute(ExetContex* ctx)override;
		void Present(Geomerty::Viewer& viewer)override;
	};

}
