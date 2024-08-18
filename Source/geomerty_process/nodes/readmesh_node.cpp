#include "core/ServiceLocator.h"
#include "nodes/readmesh_node.h"
#include "core/graph.h"
#include "algorithm/mesh_triangulate.h"
namespace Geomerty {

	void Read_MeshNode::InstallUi() {
	}
	void Read_MeshNode::Init(Graph* graph) {
		Inputs.emplace_back(GetNextId(), "Path", typeid(std::string).hash_code(), PinKind::Input);
		Inputs.back().Node = this;
		Outputs.emplace_back(GetNextId(), "Mesh", typeid(SurfaceMesh).hash_code(), PinKind::Output);
		Outputs.back().Node = this;

		graph->registry[Outputs.back().index] = { nullptr,typeid(SurfaceMesh).hash_code() };
	}
	void Read_MeshNode::Execute(ExetContex* ctx) {
		auto path = ctx->inputs[0]->Get<std::string>();
		SurfaceMesh* mesh = new SurfaceMesh();
		const std::filesystem::path fp((*path).c_str());
		if (std::filesystem::exists(fp)) {
			read_obj(*mesh, fp);

			ctx->graph->registry[Outputs.back().index].Set<SurfaceMesh>(mesh);
			std::cout << "read_mesh" << std::endl;
		}
	}
	void Read_MeshNode::Present(opengl::glfw::Viewer& viewer) {
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