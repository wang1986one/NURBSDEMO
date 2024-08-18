#include "nodes/string_node.h"
#include "core/ServiceLocator.h"
#include <UI/Widgets/Layout/Group.h>
#include "core/graph.h"
namespace Geomerty {
	
		void StringNode::InstallUi() {
			ImGui::InputText("Path", str.data(), 250);
			if (ImGui::BeginDragDropTarget())
			{
				ImGuiDragDropFlags target_flags = 0;
				target_flags |= ImGuiDragDropFlags_AcceptNoDrawDefaultRect; // Don't display the yellow rectangle
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("File", target_flags))
				{
					auto data = *(std::pair<std::string, UI::Widgets::Layout::Group*>*)payload->Data;
					str = data.first;
				}
				ImGui::EndDragDropTarget();
			}
		}
		void StringNode::Init(Graph* graph) {
			Outputs.emplace_back(GetNextId(), "Str", typeid(std::string).hash_code(), PinKind::Output);
			Outputs.back().Node = this;
			str.reserve(250);
			graph->registry[Outputs.back().index] = { &str, typeid(std::string).hash_code() };
		}
		void StringNode::Execute(ExetContex* ctx) {
			ctx->graph->registry[Outputs.back().index].Set<std::string>(&str);

		}

}