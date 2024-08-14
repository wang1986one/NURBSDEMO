#pragma once
#include"nodes.h"
#include"smooth.h"
namespace Geomerty {
	class Smooth_MeshNode :public Node {
	public:
		Smooth_MeshNode(int id, const char* name, ImColor color = ImColor(255, 255, 255)) :Node(id, name, color) {}
		void InstallUi() override {
		}
		void Init(std::unordered_map<size_t, Geomerty::NodeData>& registry) override {
			Inputs.emplace_back(GetNextId(), "InputMesh", typeid(SurfaceMesh).hash_code(), PinKind::Input);
			Inputs.back().Node = this;
			Outputs.emplace_back(GetNextId(), "OutputMesh", typeid(SurfaceMesh).hash_code(), PinKind::Output);
			Outputs.back().Node = this;
			registry[Outputs.back().index] = { nullptr,typeid(SurfaceMesh).hash_code() };
		}
		void Execute(ExetContex* ctx, std::unordered_map<size_t, Geomerty::NodeData>& registry)override {
			auto InputMesh = ctx->inputs[0]->Get<SurfaceMesh>();
			explicit_smoothing(*InputMesh);
			SurfaceMesh* mesh = new SurfaceMesh(*InputMesh);
			registry[Outputs.back().index].Set<SurfaceMesh>(mesh);
			std::cout << "smooth_mesh" << std::endl;

		}
	};


}
