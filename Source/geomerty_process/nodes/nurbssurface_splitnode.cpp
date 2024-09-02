#include<glad/glad.h>
#include "panels/ControllerView.h"
#include "core/graph.h"
#include "UI/Panels/PanelsManager.h"
#include "core/ServiceLocator.h"
#include "nodes/nurbssurface_splitnode.h"
#include "nurbs/nurbs_evalute_surface.h"

namespace Geomerty {
	void NurbsSurface_SplitNode::InstallUi()
	{
		ImGui::Checkbox("Along U", &split_along_u);
		if (ImGui::SliderFloat("split_param", &u, 0.01f, 0.99f)) {
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
	void NurbsSurface_SplitNode::Init(Graph* graph)
	{
		Inputs.emplace_back(GetNextId(), "InputSurface", typeid(Geomerty::nurbs::RationalSurface).hash_code(), PinKind::Input);
		Inputs.back().Node = this;
		Outputs.emplace_back(GetNextId(), "SplitLeft", typeid(Geomerty::nurbs::RationalSurface).hash_code(), PinKind::Output);
		Outputs.back().Node = this;
		graph->registry[Outputs.back().index] = { nullptr,typeid(Geomerty::nurbs::RationalSurface).hash_code() };
		Outputs.emplace_back(GetNextId(), "SplitRight", typeid(Geomerty::nurbs::RationalSurface).hash_code(), PinKind::Output);
		Outputs.back().Node = this;
		graph->registry[Outputs.back().index] = { nullptr,typeid(Geomerty::nurbs::RationalSurface).hash_code() };

	}
	void NurbsSurface_SplitNode::Execute(ExetContex* ctx)
	{
		auto InputCrv = ctx->inputs[0]->Get<Geomerty::nurbs::RationalSurface>();
		if (InputCrv != nullptr) {
			auto [srfleft, srfright] = split_along_u ? nurbs::util::surface_split_u(*InputCrv, u) : nurbs::util::surface_split_v(*InputCrv, u);
			Geomerty::nurbs::RationalSurface* srf = new Geomerty::nurbs::RationalSurface();
			*srf = srfleft;
			ctx->graph->registry[Outputs[0].index].Set<Geomerty::nurbs::RationalSurface>(srf);
			srf = new Geomerty::nurbs::RationalSurface();
			*srf = srfright;
			ctx->graph->registry[Outputs[1].index].Set<Geomerty::nurbs::RationalSurface>(srf);
		}
	}
	void NurbsSurface_SplitNode::Present(Geomerty::Viewer& viewer)
	{
		auto& registry = Geomerty::ServiceLocator::Get<Geomerty::Graph>().registry;
		auto srfleft = registry[Outputs[0].index].Get<Geomerty::nurbs::RationalSurface>();
		auto srfright = registry[Outputs[1].index].Get<Geomerty::nurbs::RationalSurface>();
		if (srfleft != nullptr && srfright != nullptr) {
			auto&& [pos_arrleft, index_arrleft] = Geomerty::nurbs::util::sample_nurbs_surface(*srfleft, 20);
			auto&& [pos_arrright, index_arrright] = Geomerty::nurbs::util::sample_nurbs_surface(*srfright, 20);
			Eigen::MatrixXd TV;
			Eigen::MatrixXi TF;
			Eigen::MatrixXd TC;
			Eigen::MatrixXd TFC;
			int num_leftfaces = index_arrleft.size() / 3;
			int num_rightfaces = index_arrright.size() / 3;
			TV.resize(pos_arrleft.size() + pos_arrright.size(), 3);
			TF.resize(num_leftfaces + num_rightfaces, 3);
			TFC.resize(num_leftfaces + num_rightfaces, 3);
			for (int i = 0; i < pos_arrleft.size(); i++) {
				TV.row(i) << pos_arrleft[i][0], pos_arrleft[i][1], pos_arrleft[i][2];
			}
			for (int i = 0; i < pos_arrright.size(); i++) {
				TV.row(i + pos_arrleft.size()) << pos_arrright[i][0], pos_arrright[i][1], pos_arrright[i][2];
			}
			for (int i = 0; i < num_leftfaces; i++) {
				TF.row(i) << index_arrleft[3 * i], index_arrleft[3 * i + 1], index_arrleft[3 * i + 2];
				TFC.row(i) << 1, 1, 0;
			}
			for (int i = 0; i < num_rightfaces; i++) {
				TF.row(i + num_leftfaces) << pos_arrleft.size() + index_arrright[3 * i], pos_arrleft.size() + index_arrright[3 * i + 1], pos_arrleft.size() + index_arrright[3 * i + 2];
				TFC.row(i + num_leftfaces) << 0, 1, 1;
			}
			viewer.data(0).clear();
			viewer.data(0).set_mesh(TV, TF);
			viewer.data(0).set_colors(TFC);

			Eigen::MatrixXd TP;
			Eigen::MatrixXi TE;
			int rows = srfleft->m_control_points.size();
			int cols = srfleft->m_control_points[0].size();
			TP.resize(rows * cols, 3);
			TE.resize(rows * (cols - 1) + (rows - 1) * cols, 2);

			for (int i = 0; i < rows; i++) {
				for (int j = 0; j < cols; j++) {
					TP.row(i * rows + j) << srfleft->m_control_points[i][j][0], srfleft->m_control_points[i][j][1], srfleft->m_control_points[i][j][2];
				}
			}
			int index = 0;
			for (int i = 0; i < rows; i++) {
				for (int j = 0; j < cols - 1; j++) {
					TE.row(index++) << i * rows + j, i* rows + j + 1;
				}
			}
			for (int i = 0; i < rows - 1; i++) {
				for (int j = 0; j < cols; j++) {
					TE.row(index++) << (i + 1) * rows + j, i* rows + j;
				}
			}
			TC.resize(1, 3);
			TC.row(0) << 0, 1, 1;
			viewer.data(0).set_edges(TP, TE, TC);
			TC.row(0) << 1, 1, 0;
			viewer.data(0).set_points(TP, TC);
			viewer.data(0).line_width = 2;
			viewer.data(0).point_size = 10;
		}
	}
}