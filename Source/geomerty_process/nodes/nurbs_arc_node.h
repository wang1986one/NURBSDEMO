#pragma once
#include "core/nodes.h"


namespace Geomerty {
	class NurbsArc_Node :public Node {
	private:
		std::string path;
		int item_current_idx = 0;

		//(const vec3& center, vec3 xaxis, vec3 yaxis, scalar start_angle, scalar end_angle)
		Eigen::Vector3<float> center{ 0,0,0 };
		Eigen::Vector3<float> xaxis{ 1,0,0 };
		Eigen::Vector3<float> yaxis{ 0,1,0 };
		float start_angle{ 0.0 };
		float end_angle{ 6.28 };

		int num_points = 2;
		std::vector< Eigen::Vector3<float>>polyline;

		int num_Berzier_points = 3;
		std::vector< Eigen::Vector3<float>>Beizer_points;
		std::vector<float>weights;
	public:
		NurbsArc_Node(int id, const char* name, ImColor color = ImColor(255, 255, 255)) :Node(id, name, color) {
			weights.reserve(3);
			polyline.reserve(2);
			Beizer_points.reserve(3);
			path.reserve(255);
		}
		void InstallUi() override;
		void Init(Graph* graph) override;
		void Execute(ExetContex* ctx)override;
		void Present(Geomerty::Viewer& viewer)override;
	};

}
