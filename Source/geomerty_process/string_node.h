#pragma once
#include"nodes.h"

namespace Geomerty {
	class StringNode :public Node {
	public:
		StringNode(int id, const char* name, ImColor color = ImColor(255, 255, 255)) :Node(id, name, color) {}
		void InstallUi()override {
			ImGui::InputText(std::to_string(ID.Get()).c_str(), str.data(), 250);
		}
		void Init(std::unordered_map<size_t, Geomerty::NodeData>& registry)override {
			Outputs.emplace_back(GetNextId(), "Str", typeid(std::string).hash_code(), PinKind::Output);
			Outputs.back().Node = this;
			str.reserve(250);

			registry[Outputs.back().index] = { &str, typeid(std::string).hash_code() };
		}
		void Execute(ExetContex* ctx, std::unordered_map<size_t, Geomerty::NodeData>& registry)override {
			registry[Outputs.back().index].Set<std::string>(&str);
			//td::string path=std::any_cast<std::string>((*ctx->inputs)[0]);
		}
		std::string  str;
	};

}
