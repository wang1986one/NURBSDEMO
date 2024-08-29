#include<glad/glad.h>
#include "panels/ControllerView.h"
#include "core/graph.h"
#include "UI/Panels/PanelsManager.h"
#include "core/ServiceLocator.h"
#include "nodes/nurbssurface_create_node.h"
#include "algorithm/mesh_triangulate.h"
#include "nurbs/nurbs_evalute_surface.h"
#include "nurbs/nurbs_make.h"
#include "nurbs/nurbs_io.h"
namespace Geomerty {
	nurbs::RationalSurface* get_loft() {
		nurbs::RationalSurface* srf = new nurbs::RationalSurface();
		std::vector<nurbs::RationalCurve>sections;
		sections.emplace_back(2, std::vector<float>{ 0, 0, 0, 1, 1, 1 }, std::vector<nurbs::util::vec3>{ { 0, 0, 0 }, { 10, 0, 0 }, { 40, 0, 0 } }, std::vector<float>{ 1, 1, 1 });
		sections.emplace_back(3, std::vector<float>{ 0, 0, 0, 0, 1, 1, 1, 1 }, std::vector<nurbs::util::vec3>{ { 0, 10, 10 }, { 10, 5, 10 }, { 20, -5, 10 }, { 40, 10, 10 } },
			std::vector<float>{ 1, 1, 1, 1 });
		sections.emplace_back(3, std::vector<float>{ 0, 0, 0, 0, 1, 1, 1, 1 }, std::vector<nurbs::util::vec3>{ { 0, 0, 20 }, { 10, 0, 20 }, { 20, 5, 20 }, { 40, 0, 20 } },
			std::vector<float>{ 1, 1, 1, 1, 1 });
		sections.emplace_back(3, std::vector<float>{ 0, 0, 0, 0, 1, 1, 1, 1 }, std::vector<nurbs::util::vec3>{ { 0, 3, 30 }, { 10, -4, 30 }, { 20, 10, 30 }, { 40, 0, 30 } },
			std::vector<float>{ 1, 1, 1, 1, 1 });

		nurbs::util::loft_surface(sections, *srf);
		return srf;
	}

	void NurbsSurface_Node::InstallUi()
	{
		//(const vec3& center, vec3 xaxis, vec3 yaxis, float start_angle, float end_angle)
		bool flag = false;

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
			auto crv = registry[Outputs.back().index].Get<Geomerty::nurbs::Surface>();
			Geomerty::nurbs::util::surface_write_obj("C:/Users/Administrator/Desktop/temp/ressurface", *crv);
		}

	}
	void NurbsSurface_Node::Init(Graph* graph)
	{

		Outputs.emplace_back(GetNextId(), "OutputSurface", typeid(Geomerty::nurbs::RationalSurface).hash_code(), PinKind::Output);
		Outputs.back().Node = this;
		graph->registry[Outputs.back().index] = { nullptr,typeid(Geomerty::nurbs::RationalSurface).hash_code() };

	}
	void NurbsSurface_Node::Execute(ExetContex* ctx)
	{
		using Vec3 = Geomerty::nurbs::util::vec3;
		Geomerty::nurbs::RationalSurface* crv = get_loft();

		ctx->graph->registry[Outputs.back().index].Set<Geomerty::nurbs::RationalSurface>(crv);

		auto& input_manager = Geomerty::ServiceLocator::Get<UI::Panels::PanelsManager>();
		auto& arr = input_manager.GetPanelAs<Geomerty::ControllerView>("Scene View").arr;
		arr.clear();
		for (int i = 0; i < crv->m_control_points.size(); i++) {
			for (auto& it : crv->m_control_points[i]) {
				arr.emplace_back(&it);
			}
		}
	}
	void NurbsSurface_Node::Present(Geomerty::Viewer& viewer)
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