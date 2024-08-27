#include<glad/glad.h>
#include "panels/ControllerView.h"
#include "core/graph.h"
#include "UI/Panels/PanelsManager.h"
#include "core/ServiceLocator.h"
#include "nodes/nurbs_arc_node.h"
#include "algorithm/mesh_triangulate.h"
#include "nurbs/nous_evalute_nurbs_curve.h"
namespace Geomerty {
	void NurbsArc_Node::InstallUi()
	{
		//(const vec3& center, vec3 xaxis, vec3 yaxis, scalar start_angle, scalar end_angle)
		bool flag = false;
		flag |= ImGui::DragFloat3("center", center.data(), 0.01f);
		flag |= ImGui::DragFloat3("xaxis", xaxis.data(), 0.01f);
		flag |= ImGui::DragFloat3("yaxis", yaxis.data(), 0.01f);
		flag |= ImGui::DragFloat("start_angle", &start_angle, 0.001f);
		flag |= ImGui::DragFloat("end_angle", &end_angle, 0.001f);
		if (flag) {
			auto& graph = Geomerty::ServiceLocator::Get<Geomerty::Graph>();
			Geomerty::ExetContex ctx;
			auto& input_manager = Geomerty::ServiceLocator::Get<UI::Panels::PanelsManager>();
			if (graph.GetNodeETX(this, ctx)) {
				//should be delay
				Execute(&ctx);
				Present(input_manager.GetPanelAs<Geomerty::ControllerView>("Scene View").viewer);
			}
		}
	}
	void NurbsArc_Node::Init(Graph* graph)
	{

		Outputs.emplace_back(GetNextId(), "OutputArc", typeid(nous::nurbs::RationalCurve).hash_code(), PinKind::Output);
		Outputs.back().Node = this;
		graph->registry[Outputs.back().index] = { nullptr,typeid(nous::nurbs::RationalCurve).hash_code() };

	}
	void NurbsArc_Node::Execute(ExetContex* ctx)
	{
		using Vec3 = nous::nurbs::util::vec3;
		nous::nurbs::RationalCurve* crv = new nous::nurbs::RationalCurve();
		*crv = nous::nurbs::util::rational_ellipse_arc_curve(center, xaxis, yaxis, start_angle, end_angle);
		ctx->graph->registry[Outputs.back().index].Set<nous::nurbs::RationalCurve>(crv);
	}
	void NurbsArc_Node::Present(Geomerty::Viewer& viewer)
	{
		auto& registry = Geomerty::ServiceLocator::Get<Geomerty::Graph>().registry;
		auto crv = registry[Outputs.back().index].Get<nous::nurbs::RationalCurve>();
		if (crv != nullptr) {
			auto pos_arr = nous::nurbs::util::sample_curve(*crv, 80);
			Eigen::MatrixXd TV;
			Eigen::MatrixXi TE;
			Eigen::MatrixXd TC;
			TV.resize(pos_arr.size(), 3);
			TE.resize(pos_arr.size() - 1, 2);
			TC.resize(pos_arr.size() - 1, 3);
			for (int i = 0; i < pos_arr.size(); i++) {
				TV.row(i) << pos_arr[i][0], pos_arr[i][1], pos_arr[i][2];
				if (i > 0) {
					TE.row(i - 1) << i - 1, i;
					TC.row(i - 1) << 1, 1, 1;
				}
			}
			viewer.data(0).clear();
			viewer.data(0).set_points(TV, Eigen::RowVector3d(1, 1, 0));
			viewer.data(0).point_size = 10;
			viewer.data(0).set_edges(TV, TE, TC);
			viewer.data(0).line_width = 2;
			// get properties
		}
	}
}