#include<glad/glad.h>
#include "panels/ControllerView.h"
#include "core/graph.h"
#include "UI/Panels/PanelsManager.h"
#include "core/ServiceLocator.h"
#include "nodes/nurbssurface_loadnode.h"
#include "algorithm/mesh_triangulate.h"
#include "nurbs/nurbs_evalute_surface.h"
#include "nurbs/nurbs_io.h"
namespace Geomerty {

	void NurbsSurface_LoadNode::InstallUi()
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
		if (ImGui::Button("Present")) {
			auto& input_manager = Geomerty::ServiceLocator::Get<UI::Panels::PanelsManager>();
			Present(input_manager.GetPanelAs<Geomerty::ControllerView>("Scene View").viewer);
		}
		if (ImGui::Button("Save")) {
			auto& registry = Geomerty::ServiceLocator::Get<Geomerty::Graph>().registry;
			auto crv = registry[Outputs.back().index].Get<Geomerty::nurbs::RationalSurface>();
			Geomerty::nurbs::util::surface_write_obj(path, *crv);
		}

	}
	void NurbsSurface_LoadNode::Init(Graph* graph)
	{
		path.reserve(250);
		Outputs.emplace_back(GetNextId(), "OutputSurface", typeid(Geomerty::nurbs::RationalSurface).hash_code(), PinKind::Output);
		Outputs.back().Node = this;
		graph->registry[Outputs.back().index] = { nullptr,typeid(Geomerty::nurbs::RationalSurface).hash_code() };

	}
	void NurbsSurface_LoadNode::Execute(ExetContex* ctx)
	{
		if (std::filesystem::exists(path)) {

			Geomerty::nurbs::RationalSurface* srf = new Geomerty::nurbs::RationalSurface();
			*srf = Geomerty::nurbs::util::surfaceReadOBJ(path);
			ctx->graph->registry[Outputs.back().index].Set<Geomerty::nurbs::RationalSurface>(srf);

			auto& input_manager = Geomerty::ServiceLocator::Get<UI::Panels::PanelsManager>();
			auto& arr = input_manager.GetPanelAs<Geomerty::ControllerView>("Scene View").arr;
			arr.clear();
			for (int i = 0; i < srf->m_control_points.size(); i++) {
				for (auto& it : srf->m_control_points[i]) {
					arr.emplace_back(&it);
				}
			}
		}
	}
	void NurbsSurface_LoadNode::Present(Geomerty::Viewer& viewer)
	{
		auto& registry = Geomerty::ServiceLocator::Get<Geomerty::Graph>().registry;
		auto srf = registry[Outputs.back().index].Get<Geomerty::nurbs::RationalSurface>();
		if (srf != nullptr) {
			auto&& [pos_arr, index_arr] = Geomerty::nurbs::util::sample_nurbs_surface(*srf, 80);
			Eigen::MatrixXd TV;
			Eigen::MatrixXi TF;
			Eigen::MatrixXd TC;
			int num_faces = index_arr.size() / 3;
			TV.resize(pos_arr.size(), 3);
			TF.resize(num_faces, 3);
			for (int i = 0; i < pos_arr.size(); i++) {
				TV.row(i) << pos_arr[i][0], pos_arr[i][1], pos_arr[i][2];
			}
			for (int i = 0; i < num_faces; i++) {
				TF.row(i) << index_arr[3 * i], index_arr[3 * i + 1], index_arr[3 * i + 2];
			}
			viewer.data(0).clear();
			viewer.data(0).set_mesh(TV, TF);

			Eigen::MatrixXd TP;
			Eigen::MatrixXi TE;
			int rows = srf->m_control_points.size();
			int cols = srf->m_control_points[0].size();
			TP.resize(rows * cols, 3);
			TE.resize(rows * (cols - 1) + (rows - 1) * cols, 2);

			for (int i = 0; i < rows; i++) {
				for (int j = 0; j < cols; j++) {
					TP.row(i * rows + j) << srf->m_control_points[i][j][0], srf->m_control_points[i][j][1], srf->m_control_points[i][j][2];
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