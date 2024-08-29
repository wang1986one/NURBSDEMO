#include<glad/glad.h>
#include "panels/ControllerView.h"
#include "core/graph.h"
#include "UI/Panels/PanelsManager.h"
#include "core/ServiceLocator.h"
#include "nurbscurve_loadnode.h"
#include "algorithm/mesh_triangulate.h"
#include "nurbs/nurbs_evalute_curve.h"
#include "nurbs/nurbs_make.h"
#include "nurbs/nurbs_io.h"
namespace Geomerty {
	void NurbsCurve_LoadNode::InstallUi()
	{
		ImGui::InputText("Path", path.data(), 250);
		if (ImGui::BeginDragDropTarget())
		{
			ImGuiDragDropFlags target_flags = 0;
			target_flags |= ImGuiDragDropFlags_AcceptNoDrawDefaultRect; // Don't display the yellow rectangle
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("File", target_flags))
			{
				auto data = *(std::pair<std::string, UI::Widgets::Layout::Group*>*)payload->Data;
				path = data.first;
			}
			ImGui::EndDragDropTarget();
		}

	}
	void NurbsCurve_LoadNode::Init(Graph* graph)
	{
		path.reserve(250);

		Outputs.emplace_back(GetNextId(), "Nurbs_Curve", typeid(Geomerty::nurbs::RationalCurve).hash_code(), PinKind::Output);
		Outputs.back().Node = this;
		graph->registry[Outputs.back().index] = { nullptr,typeid(Geomerty::nurbs::RationalCurve).hash_code() };

	}
	void NurbsCurve_LoadNode::Execute(ExetContex* ctx)
	{

		if (std::filesystem::exists(path)) {
			Geomerty::nurbs::RationalCurve* crv = new Geomerty::nurbs::RationalCurve();
			*crv = Geomerty::nurbs::util::curve_read_obj(path);
			ctx->graph->registry[Outputs.back().index].Set<Geomerty::nurbs::RationalCurve>(crv);

			auto& input_manager = Geomerty::ServiceLocator::Get<UI::Panels::PanelsManager>();
			auto& arr = input_manager.GetPanelAs<Geomerty::ControllerView>("Scene View").arr;
			arr.clear();
			for (auto& it : crv->m_control_points) {
				arr.emplace_back(&it);
			}
		}

	}
	void NurbsCurve_LoadNode::Present(Geomerty::Viewer& viewer)
	{
		auto& registry = Geomerty::ServiceLocator::Get<Geomerty::Graph>().registry;
		auto crv = registry[Outputs.back().index].Get<Geomerty::nurbs::RationalCurve>();
		if (crv != nullptr) {
			auto pos_arr = Geomerty::nurbs::util::sample_curve(*crv, 80);
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
			Eigen::MatrixXd TTV;
			TTV.resize(crv->m_control_points.size(), 3);
			for (int i = 0; i < crv->m_control_points.size(); i++) {
				TTV.row(i) << crv->m_control_points[i][0], crv->m_control_points[i][1], crv->m_control_points[i][2];
			}
			Eigen::MatrixXd cc;
			cc.resize(1, 3);
			cc.row(0) << 0, 1, 1;
			viewer.data(0).clear();
			viewer.data(0).set_points(TTV, cc);
			viewer.data(0).point_size = 10;
			viewer.data(0).set_edges(TV, TE, TC);
			viewer.data(0).line_width = 2;

		}
	}
}