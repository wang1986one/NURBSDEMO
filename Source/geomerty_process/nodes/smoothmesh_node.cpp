#include"nodes/smoothmesh_node.h"
#include "algorithm/smooth.h"
#include "UI/Panels/PanelsManager.h"
#include "core/ServiceLocator.h"
#include "panels/ControllerView.h"
#include"core/graph.h"
#include"algorithm/mesh_triangulate.h"
namespace Geomerty {
	void Smooth_MeshNode::InstallUi() {
		bool flag = false;
		flag |= ImGui::Checkbox("Use Uniform lapace", &use_uniform);
		flag |= ImGui::SliderInt("Iterations", &iterations, 1, 10);
		if (flag) {
			auto& input_manager = Geomerty::ServiceLocator::Get<UI::Panels::PanelsManager>();
			auto& graph = Geomerty::ServiceLocator::Get<Geomerty::Graph>();
			Geomerty::ExetContex ctx;
			if (graph.GetNodeETX(this, ctx)) {
				//should be delay
				Execute(&ctx);
				Present(input_manager.GetPanelAs<Geomerty::ControllerView>("Scene View").viewer);
			}
		}
	}
	void Smooth_MeshNode::Init(Graph* graph) {

		Inputs.emplace_back(GetNextId(), "InputMesh", typeid(SurfaceMesh).hash_code(), PinKind::Input);
		Inputs.back().Node = this;
		Outputs.emplace_back(GetNextId(), "OutputMesh", typeid(SurfaceMesh).hash_code(), PinKind::Output);
		Outputs.back().Node = this;
		graph->registry[Outputs.back().index] = { nullptr,typeid(SurfaceMesh).hash_code() };
	}
	void Smooth_MeshNode::Execute(ExetContex* ctx) {
		auto InputMesh = ctx->inputs[0]->Get<SurfaceMesh>();
		if (InputMesh) {
			SurfaceMesh* mesh = new SurfaceMesh(*InputMesh);
			explicit_smoothing(*mesh, iterations, use_uniform);
			ctx->graph->registry[Outputs.back().index].Set<SurfaceMesh>(mesh);
			std::cout << "smooth_mesh" << std::endl;
		}
	}
	void Smooth_MeshNode::Present(opengl::glfw::Viewer& viewer) {
		auto& registry = Geomerty::ServiceLocator::Get<Geomerty::Graph>().registry;
		auto mesh = registry[Outputs.back().index].Get<SurfaceMesh>();
		if (mesh != nullptr) {
			bool is_triangle = mesh->is_triangle_mesh();
			SurfaceMesh* present_mesh = mesh;
			if (!is_triangle) {
				present_mesh = new SurfaceMesh(*mesh);
				Geomerty::triangulate(*present_mesh);
			}
			Eigen::MatrixXd SV;
			auto& pos = present_mesh->positions();
			SV.resize(pos.size(), 3);
			for (int i = 0; i < pos.size(); i++) {
				SV.row(i) << pos[i][0], pos[i][1], pos[i][2];
			}
			Eigen::MatrixXi SF;
			SF.resize(present_mesh->faces_size(), 3);
			auto& faces = present_mesh->faces();
			int j = 0, x, y, z;
			for (auto f : faces) {
				auto it = present_mesh->vertices(f).begin();
				x = (*it++).idx();
				y = (*it++).idx();
				z = (*it++).idx();
				SF.row(j++) << x, y, z;
			}
			viewer.data(0).clear();
			viewer.data(0).set_mesh(SV, SF);
		}

	}





}
