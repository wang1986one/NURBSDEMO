#include<glad/glad.h>
#include "panels/ControllerView.h"
#include "core/graph.h"
#include "UI/Panels/PanelsManager.h"
#include "core/ServiceLocator.h"
#include "nodes/nurbs_arc_node.h"
#include "algorithm/mesh_triangulate.h"
#include "nurbs/nurbs_evalute_curve.h"
#include "nurbs/nurbs_make.h"
#include "nurbs/nurbs_io.h"
namespace Geomerty {
	void NurbsArc_Node::InstallUi()
	{
		const char* items[] = { "Ellipse", "Polyline", "Bezier" };
		const char* combo_preview_value = items[item_current_idx];
		if (ImGui::BeginCombo("Shape Kind", combo_preview_value))
		{
			for (int n = 0; n < IM_ARRAYSIZE(items); n++)
			{
				const bool is_selected = (item_current_idx == n);
				if (ImGui::Selectable(items[n], is_selected))
				{
					item_current_idx = n;
				}
				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		bool flag = false;
		if (item_current_idx == 0) {
			flag |= ImGui::DragFloat3("center", center.data(), 0.01f);
			flag |= ImGui::DragFloat3("xaxis", xaxis.data(), 0.01f);
			flag |= ImGui::DragFloat3("yaxis", yaxis.data(), 0.01f);
			flag |= ImGui::DragFloat("start_angle", &start_angle, 0.001f);
			flag |= ImGui::DragFloat("end_angle", &end_angle, 0.001f);
		}
		else if (item_current_idx == 1) {
			bool localflag = false;
			localflag |= ImGui::SliderInt("Point num", &num_points, 2, 10);
			flag |= localflag;
			if (localflag) {
				polyline.clear();
				for (int i = 0; i < num_points; i++) {
					polyline.emplace_back(Eigen::Vector3<float>(i, i % 2, 0));
				}
			}
		}
		else if (item_current_idx == 2) {
			bool localflag = false;
			localflag |= ImGui::SliderInt("Point num", &num_Berzier_points, 3, 5);
			flag |= localflag;
			if (localflag) {
				Beizer_points.clear();
				weights.clear();
				for (int i = 0; i < num_Berzier_points; i++) {
					Beizer_points.emplace_back(Eigen::Vector3<float>(i, i % 2, 0));
					weights.emplace_back(1.0f);
				}
			}
			for (int i = 0; i < weights.size(); i++) {
				std::string temp = "weight" + std::to_string(i);
				flag |= ImGui::SliderFloat(temp.c_str(), &weights[i], 1.0f, 100.0f);
			}
		}
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
		if (ImGui::Button("Present")) {
			auto& input_manager = Geomerty::ServiceLocator::Get<UI::Panels::PanelsManager>();
			Present(input_manager.GetPanelAs<Geomerty::ControllerView>("Scene View").viewer);
		}
		if (ImGui::Button("Save")) {
			auto& registry = Geomerty::ServiceLocator::Get<Geomerty::Graph>().registry;
			auto crv = registry[Outputs.back().index].Get<Geomerty::nurbs::RationalCurve>();
			Geomerty::nurbs::util::curve_write_obj(path, *crv);
		}
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
	void NurbsArc_Node::Init(Graph* graph)
	{

		Outputs.emplace_back(GetNextId(), "OutputArc", typeid(Geomerty::nurbs::RationalCurve).hash_code(), PinKind::Output);
		Outputs.back().Node = this;
		graph->registry[Outputs.back().index] = { nullptr,typeid(Geomerty::nurbs::RationalCurve).hash_code() };

	}
	void NurbsArc_Node::Execute(ExetContex* ctx)
	{
		using Vec3 = Geomerty::nurbs::util::vec3;
		Geomerty::nurbs::RationalCurve* crv = new Geomerty::nurbs::RationalCurve();
		if (item_current_idx == 0) {
			Geomerty::nurbs::util::CreateArc(center, xaxis, yaxis, start_angle, end_angle, xaxis.norm(), yaxis.norm(), *crv);
			//*crv = Geomerty::nurbs::util::rational_ellipse_arc_curve(center, xaxis, yaxis, start_angle, end_angle);
		}
		else if (item_current_idx == 1) {
			*crv = Geomerty::nurbs::util::rational_polyline_curve(polyline);
		}
		else if (item_current_idx == 2) {
			*crv = Geomerty::nurbs::util::rational_bezier_curve(Beizer_points, weights);
		}

		ctx->graph->registry[Outputs.back().index].Set<Geomerty::nurbs::RationalCurve>(crv);
		auto& input_manager = Geomerty::ServiceLocator::Get<UI::Panels::PanelsManager>();
		auto& arr = input_manager.GetPanelAs<Geomerty::ControllerView>("Scene View").arr;
		input_manager.GetPanelAs<Geomerty::ControllerView>("Scene View").node = this;
		arr.clear();
		for (auto& it : crv->m_control_points) {
			arr.emplace_back(&it);
		}


	}
	void NurbsArc_Node::Present(Geomerty::Viewer& viewer)
	{
		auto& registry = Geomerty::ServiceLocator::Get<Geomerty::Graph>().registry;
		auto crv = registry[Outputs.back().index].Get<Geomerty::nurbs::RationalCurve>();
		if (crv != nullptr) {
			auto pos_arr = Geomerty::nurbs::util::sample_curve(*crv, 80);
			Eigen::MatrixXd TV;
			Eigen::MatrixXi TE;
			Eigen::MatrixXd TC;
			TV.resize(pos_arr.size()+ crv->m_control_points.size(), 3);
			TE.resize(pos_arr.size() + crv->m_control_points.size() - 2, 2);
			TC.resize(pos_arr.size() + crv->m_control_points.size() - 2, 3);
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
				TV.row(i+ pos_arr.size()) << crv->m_control_points[i][0], crv->m_control_points[i][1], crv->m_control_points[i][2];
				if (i > 0) {
					TE.row(pos_arr.size()+i - 2) << i - 1+ pos_arr.size(), i+ pos_arr.size();
					TC.row(pos_arr.size()+i - 2) << 0, 1, 1;
				}
			}
			
			Eigen::MatrixXd cc;
			cc.resize(1, 3);
			cc.row(0) << 0, 1, 1;
			viewer.data(0).clear();
			viewer.data(0).set_points(TTV, cc);
			viewer.data(0).point_size = 10;
			viewer.data(0).set_edges(TV, TE, TC);
			viewer.data(0).line_width = 1;

		}
	}
}