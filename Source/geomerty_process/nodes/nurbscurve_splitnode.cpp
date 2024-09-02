#include<glad/glad.h>
#include "panels/ControllerView.h"
#include "core/graph.h"
#include "UI/Panels/PanelsManager.h"
#include "core/ServiceLocator.h"
#include "nodes/nurbscurve_splitnode.h"
#include "nurbs/nurbs_evalute_curve.h"

namespace Geomerty {
	void NurbsCurve_SplitNode::InstallUi()
	{
		if (ImGui::SliderFloat("split_u", &u, 0.01f, 0.99f)) {
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
	void NurbsCurve_SplitNode::Init(Graph* graph)
	{
		Inputs.emplace_back(GetNextId(), "InputCurve", typeid(Geomerty::nurbs::RationalCurve).hash_code(), PinKind::Input);
		Inputs.back().Node = this;
		Outputs.emplace_back(GetNextId(), "SplitLeft", typeid(Geomerty::nurbs::RationalCurve).hash_code(), PinKind::Output);
		Outputs.back().Node = this;
		graph->registry[Outputs.back().index] = { nullptr,typeid(Geomerty::nurbs::RationalCurve).hash_code() };
		Outputs.emplace_back(GetNextId(), "SplitRight", typeid(Geomerty::nurbs::RationalCurve).hash_code(), PinKind::Output);
		Outputs.back().Node = this;
		graph->registry[Outputs.back().index] = { nullptr,typeid(Geomerty::nurbs::RationalCurve).hash_code() };

	}
	void NurbsCurve_SplitNode::Execute(ExetContex* ctx)
	{
		auto InputCrv = ctx->inputs[0]->Get<Geomerty::nurbs::RationalCurve>();
		if (InputCrv != nullptr) {
			Geomerty::nurbs::RationalCurve* crv = new Geomerty::nurbs::RationalCurve();
			auto [crvleft, crvright] = nurbs::util::curve_split(*InputCrv, u);
			*crv = crvleft;
			ctx->graph->registry[Outputs[0].index].Set<Geomerty::nurbs::RationalCurve>(crv);
			crv = new Geomerty::nurbs::RationalCurve();
			*crv = crvright;
			ctx->graph->registry[Outputs[1].index].Set<Geomerty::nurbs::RationalCurve>(crv);
		}
	}
	void NurbsCurve_SplitNode::Present(Geomerty::Viewer& viewer)
	{
		auto& registry = Geomerty::ServiceLocator::Get<Geomerty::Graph>().registry;
		auto crvleft = registry[Outputs[0].index].Get<Geomerty::nurbs::RationalCurve>();
		auto crvright = registry[Outputs[1].index].Get<Geomerty::nurbs::RationalCurve>();
		if (crvleft != nullptr && crvright != nullptr) {
			auto posleft_arr = Geomerty::nurbs::util::sample_curve(*crvleft, 80);
			auto posright_arr = Geomerty::nurbs::util::sample_curve(*crvright, 80);
			Eigen::MatrixXd TV;
			Eigen::MatrixXi TE;
			Eigen::MatrixXd TC;
			TV.resize(posleft_arr.size() + posright_arr.size(), 3);
			TE.resize(posleft_arr.size() + posright_arr.size() - 2, 2);
			TC.resize(posleft_arr.size() + posright_arr.size() - 2, 3);
			for (int i = 0; i < posleft_arr.size(); i++) {
				TV.row(i) << posleft_arr[i][0], posleft_arr[i][1], posleft_arr[i][2];
				if (i > 0) {
					TE.row(i - 1) << i - 1, i;
					TC.row(i - 1) << 1, 1, 0;
				}
			}
			for (int i = 0; i < posright_arr.size(); i++) {
				TV.row(i + posleft_arr.size()) << posright_arr[i][0], posright_arr[i][1], posright_arr[i][2];
				if (i > 0) {
					TE.row(i - 2 + posleft_arr.size()) << i - 1 + posleft_arr.size(), i + posleft_arr.size();
					TC.row(i - 2 + posleft_arr.size()) << 0, 1, 1;
				}
			}
			Eigen::MatrixXd TTV;
			Eigen::MatrixXd cc;
			cc.resize(crvleft->m_control_points.size() + crvright->m_control_points.size(), 3);
			TTV.resize(crvleft->m_control_points.size() + crvright->m_control_points.size(), 3);
			for (int i = 0; i < crvleft->m_control_points.size(); i++) {
				TTV.row(i) << crvleft->m_control_points[i][0], crvleft->m_control_points[i][1], crvleft->m_control_points[i][2];
				cc.row(i) << 1, 1, 0;
			}
			for (int i = 0; i < crvright->m_control_points.size(); i++) {
				TTV.row(i + crvleft->m_control_points.size()) << crvright->m_control_points[i][0], crvright->m_control_points[i][1], crvright->m_control_points[i][2];
				cc.row(i + crvleft->m_control_points.size()) << 0, 1, 1;
			}
			viewer.data(0).clear();
			viewer.data(0).set_points(TTV, cc);
			viewer.data(0).point_size = 10;
			viewer.data(0).set_edges(TV, TE, TC);
			viewer.data(0).line_width = 1;
		}
	}
}