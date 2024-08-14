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
	};


}
