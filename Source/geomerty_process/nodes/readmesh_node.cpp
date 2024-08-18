#include "core/ServiceLocator.h"
#include "nodes/readmesh_node.h"
#include "core/graph.h"
namespace Geomerty {

	void Read_MeshNode::InstallUi() {
	}
	void Read_MeshNode::Init(Graph*graph)  {
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
	void Read_MeshNode::Present(opengl::glfw::Viewer& viewer){
		auto& registry = Geomerty::ServiceLocator::Get<Geomerty::Graph>().registry;
		auto mesh = registry[Outputs.back().index].Get<SurfaceMesh>();
		if (mesh != nullptr) {
			Eigen::MatrixXd SV;
			auto& pos = mesh->positions();
			SV.resize(pos.size(), 3);
			for (int i = 0; i < pos.size(); i++) {
				SV.row(i) << pos[i][0], pos[i][1], pos[i][2];
			}
			//"D:\Project\C++\opengl\DX11\data\SHREC2011\alien-1.obj"
			Eigen::MatrixXi SF;
			if (mesh->is_quad_mesh())
				SF.resize(mesh->faces_size(), 4);
			else
				SF.resize(mesh->faces_size(), 3);
			auto& faces = mesh->faces();
			int j = 0;
			for (auto f : faces) {
				auto it = mesh->vertices(f).begin();
				int x = (*it++).idx();
				int y = (*it++).idx();
				int z = (*it++).idx();
				//int w = (*it++).idx();
				SF.row(j) << x, y, z;

				j++;
			}
			viewer.data(0).clear();
			viewer.data(0).set_mesh(SV, SF);
		}

	}



}