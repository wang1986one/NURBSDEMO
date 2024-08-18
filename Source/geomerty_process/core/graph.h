#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include <imgui_internal.h>
#include <imgui_node_editor.h>
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "ImGuizmo.h"
#include "core/nodes.h"

namespace Geomerty {

	namespace ed = ax::NodeEditor;
	class Graph {
	public:
		ed::EditorContext* m_Editor;
		std::vector<Geomerty::Node*>    m_Nodes;
		std::vector<Geomerty::Link>    m_Links;
		std::unordered_map<uintptr_t, Geomerty::Node*>m_Node_Id_map;
		std::unordered_map<uintptr_t, Geomerty::Link*>m_Link_Id_map;
		std::unordered_map<uintptr_t, Geomerty::Pin*>m_Pin_Id_map;
		std::unordered_map<size_t, Geomerty::NodeData>registry;
		Geomerty::NodeData* GetPinData(size_t index) {
			return &registry[index];
		}
		ed::LinkId GetNextLinkId()
		{
			return ed::LinkId(Geomerty::GetNextId());
		}
		Geomerty::Node* FindNode(ed::NodeId id);

		Geomerty::Link* FindLink(ed::LinkId id);
		Geomerty::Pin* FindPin(ed::PinId id);

		bool IsPinLinked(ed::PinId id);
		Geomerty::Pin* FindConnectedStartPin(ed::PinId id);
		bool CanCreateLink(Geomerty::Pin* a, Geomerty::Pin* b);

		bool GetNodeETX(Geomerty::Node* n, Geomerty::ExetContex& ctx);
		Geomerty::Node* SpawnStringNode();
		Geomerty::Node* SpawnRead_MeshNode();
		Geomerty::Node* SpawnSmoothMesh_Node();
		Geomerty::Node* SpawnSpape_Node();
		int GetSelectNode();
		void OnStart() {

			m_Editor = ed::CreateEditor(nullptr);
			ed::SetCurrentEditor(m_Editor);
			Geomerty::Node* node;
			node = SpawnStringNode(); ed::SetNodePosition(node->ID, ImVec2(-252, 220));
			node = SpawnRead_MeshNode(); ed::SetNodePosition(node->ID, ImVec2(-300, 351));
			node = SpawnSmoothMesh_Node(); ed::SetNodePosition(node->ID, ImVec2(-350, 351));
			node = SpawnSpape_Node(); ed::SetNodePosition(node->ID, ImVec2(-350, 351));
			ed::NavigateToContent();
		}
		void OnStop()
		{
			if (m_Editor)
			{
				ed::DestroyEditor(m_Editor);
				m_Editor = nullptr;
			}
		}
		ImColor GetIconColor(Geomerty::PinType type)
		{
			return ImColor(255, 255, 255);
		};
		void OnFrame(float deltaTime);
		void Execute(int to_index);
		template<typename T>
		T* GetNode(int index) {
			auto Node_data = GetPinData(m_Nodes[index]->Outputs[0].index);
			return Node_data->Get<T>();
		}
	};


}
