#pragma once
#include "core/nodes.h"

namespace Geomerty {
	class StringNode :public Node {
	public:
		StringNode(int id, const char* name, ImColor color = ImColor(255, 255, 255)) :Node(id, name, color) {}
		void InstallUi()override;
		void Init(Graph* graph)override;
		void Execute(ExetContex* ctx)override;
		std::string  str;
	};

}
