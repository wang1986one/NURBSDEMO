#pragma once
#include"nodes.h"

namespace Geomerty {

	class Read_MeshNode :public Node {
	public:
		Read_MeshNode(int id, const char* name, ImColor color = ImColor(255, 255, 255)) :Node(id, name, color) {}
		void InstallUi() override {
		}
		void Init(std::unordered_map<size_t, Geomerty::NodeData>& registry) override {
			Inputs.emplace_back(GetNextId(), "Path", typeid(std::string).hash_code(), PinKind::Input);
			Inputs.back().Node = this;
			Outputs.emplace_back(GetNextId(), "Mesh", typeid(SurfaceMesh).hash_code(), PinKind::Output);
			Outputs.back().Node = this;

			registry[Outputs.back().index] = { nullptr,typeid(SurfaceMesh).hash_code() };
		}
		void Execute(ExetContex* ctx, std::unordered_map<size_t, Geomerty::NodeData>& registry)override {
			auto path = ctx->inputs[0]->Get<std::string>();
			SurfaceMesh* mesh = new SurfaceMesh();
			const std::filesystem::path fp((*path).c_str());
			if (std::filesystem::exists(fp)) {
				read_obj(*mesh, fp);
				registry[Outputs.back().index].Set<SurfaceMesh>(mesh);
				std::cout << "read_mesh" << std::endl;
			}
		}
		void Present(opengl::glfw::Viewer& viewer, std::unordered_map<size_t, Geomerty::NodeData>& registry)override {
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
	};


}
