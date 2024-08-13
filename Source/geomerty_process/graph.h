#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include <imgui_internal.h>
#include <imgui_node_editor.h>
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "ImGuizmo.h"
#include "nodes.h"
#include"string_node.h"
#include"readmesh_node.h"
namespace ed = ax::NodeEditor;
namespace Geomerty {


	struct Graph {
		ed::EditorContext* m_Editor;
		std::vector<Geomerty::Node*>    m_Nodes;
		std::vector<Geomerty::Link>    m_Links;
		std::unordered_map<size_t, Geomerty::NodeData>registry;
		Geomerty::NodeData* GetPinData(size_t index) {
			return &registry[index];
		}
		ed::LinkId GetNextLinkId()
		{
			return ed::LinkId(Geomerty::GetNextId());
		}
		Geomerty::Node* FindNode(ed::NodeId id)
		{
			for (auto& node : m_Nodes)
				if (node->ID == id)
					return node;
			return nullptr;
		}
		Geomerty::Link* FindLink(ed::LinkId id)
		{
			for (auto& link : m_Links)
				if (link.ID == id)
					return &link;
			return nullptr;
		}
		Geomerty::Pin* FindPin(ed::PinId id)
		{
			if (!id)
				return nullptr;

			for (auto& node : m_Nodes)
			{
				for (auto& pin : node->Inputs)
					if (pin.ID == id)
						return &pin;

				for (auto& pin : node->Outputs)
					if (pin.ID == id)
						return &pin;
			}

			return nullptr;
		}
		bool IsPinLinked(ed::PinId id)
		{
			if (!id)
				return false;

			for (auto& link : m_Links)
				if (link.StartPinID == id || link.EndPinID == id)
					return true;

			return false;
		}
		Geomerty::Pin* FindConnectedStartPin(ed::PinId id) {
			for (auto& link : m_Links) {
				if (link.EndPinID == id)
				{
					return FindPin(link.StartPinID);
				}
			}
			return nullptr;
		}
		bool CanCreateLink(Geomerty::Pin* a, Geomerty::Pin* b)
		{
			if (!a || !b || a == b || a->Kind == b->Kind || a->Type != b->Type || a->Node == b->Node)
				return false;

			return true;
		}
		void BuildNode(Geomerty::Node* node)
		{
			for (auto& input : node->Inputs)
			{
				input.Node = node;
				input.Kind = Geomerty::PinKind::Input;
			}

			for (auto& output : node->Outputs)
			{
				output.Node = node;
				output.Kind = Geomerty::PinKind::Output;
			}
		}
		Geomerty::Node* SpawnStringNode()
		{
			m_Nodes.push_back(new Geomerty::StringNode(Geomerty::GetNextId(), "StringNode", ImColor(255, 255, 128)));
			
			m_Nodes.back()->Init(registry);
			return m_Nodes.back();
		}
		Geomerty::Node* SpawnRead_MeshNode()
		{
			m_Nodes.push_back(new Geomerty::Read_MeshNode(Geomerty::GetNextId(), "Read_MeshNode"));
			
			m_Nodes.back()->Init(registry);
			return m_Nodes.back();
		}
		void OnStart() {

			m_Editor = ed::CreateEditor(nullptr);
			ed::SetCurrentEditor(m_Editor);
			Geomerty::Node* node;
			node = SpawnStringNode(); ed::SetNodePosition(node->ID, ImVec2(-252, 220));
			node = SpawnRead_MeshNode(); ed::SetNodePosition(node->ID, ImVec2(-300, 351));
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
		void OnFrame(float deltaTime)
		{
			static ed::NodeId contextNodeId = 0;
			static ed::LinkId contextLinkId = 0;
			static ed::PinId  contextPinId = 0;
			static bool createNewNode = false;
			static Geomerty::Pin* newNodeLinkPin = nullptr;
			static Geomerty::Pin* newLinkPin = nullptr;
			ed::SetCurrentEditor(m_Editor);


			ed::Begin("Node editor", ImVec2(0.0, 0.0f));
			auto cursorTopLeft = ImGui::GetCursorScreenPos();
			//blueprint and simple
			for (auto& node : m_Nodes)
			{

				ed::BeginNode(node->ID);
				ImGui::Text(node->Name.c_str());
				for (auto& input : node->Inputs)
				{
					auto alpha = ImGui::GetStyle().Alpha;
					if (newLinkPin && !CanCreateLink(newLinkPin, &input) && &input != newLinkPin)
						alpha = alpha * (48.0f / 255.0f);
					ed::BeginPin(input.ID, ax::NodeEditor::PinKind::Input);
					//ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);

					//ImGui::Spring(0);
					if (!input.Name.empty())
					{
						ImGui::TextUnformatted(input.Name.c_str());
						//ImGui::Spring(0);
					}
					if (input.Type == typeid(bool).hash_code())
					{
						ImGui::Button("Hello");
						//ImGui::Spring(0);
					}
					//ImGui::PopStyleVar();
					ed::EndPin();

				}
				node->InstallUi();
				for (auto& output : node->Outputs)
				{
					auto alpha = ImGui::GetStyle().Alpha;
					if (newLinkPin && !CanCreateLink(newLinkPin, &output) && &output != newLinkPin)
						alpha = alpha * (48.0f / 255.0f);
					//ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
					ed::BeginPin(output.ID, ax::NodeEditor::PinKind::Output);

					if (!output.Name.empty())
					{
						// ImGui::Spring(0);
						ImGui::TextUnformatted(output.Name.c_str());
					}
					//ImGui::PopStyleVar();
					ed::EndPin();
				}
				ed::EndNode();
			}

			for (auto& link : m_Links)
				ed::Link(link.ID, link.StartPinID, link.EndPinID, link.Color, 2.0f);

			if (!createNewNode)
			{
				if (ed::BeginCreate(ImColor(255, 255, 255), 2.0f))
				{
					auto showLabel = [](const char* label, ImColor color)
						{
							ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
							auto size = ImGui::CalcTextSize(label);

							auto padding = ImGui::GetStyle().FramePadding;
							auto spacing = ImGui::GetStyle().ItemSpacing;

							ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));

							auto rectMin = ImGui::GetCursorScreenPos() - padding;
							auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

							auto drawList = ImGui::GetWindowDrawList();
							drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
							ImGui::TextUnformatted(label);
						};

					ed::PinId startPinId = 0, endPinId = 0;
					if (ed::QueryNewLink(&startPinId, &endPinId))
					{
						auto startPin = FindPin(startPinId);
						auto endPin = FindPin(endPinId);

						newLinkPin = startPin ? startPin : endPin;

						if (startPin->Kind == Geomerty::PinKind::Input)
						{
							std::swap(startPin, endPin);
							std::swap(startPinId, endPinId);
						}

						if (startPin && endPin)
						{
							if (endPin == startPin)
							{
								ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
							}
							else if (endPin->Kind == startPin->Kind)
							{
								showLabel("x Incompatible Pin Kind", ImColor(45, 32, 32, 180));
								ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
							}
							else if (endPin->Node == startPin->Node)
							{
								showLabel("x Cannot connect to self", ImColor(45, 32, 32, 180));
								ed::RejectNewItem(ImColor(255, 0, 0), 1.0f);
							}
							else if (endPin->Type != startPin->Type)
							{
								showLabel("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
								ed::RejectNewItem(ImColor(255, 128, 128), 1.0f);
							}
							else
							{
								showLabel("+ Create Link", ImColor(32, 45, 32, 180));
								if (ed::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
								{
									m_Links.emplace_back(Geomerty::Link(Geomerty::GetNextId(), startPinId, endPinId));
									m_Links.back().Color = GetIconColor(startPin->Type);
								}
							}
						}
					}

					ed::PinId pinId = 0;
					if (ed::QueryNewNode(&pinId))
					{
						newLinkPin = FindPin(pinId);
						if (newLinkPin)
							showLabel("+ Create Node", ImColor(32, 45, 32, 180));

						if (ed::AcceptNewItem())
						{
							createNewNode = true;
							newNodeLinkPin = FindPin(pinId);
							newLinkPin = nullptr;
							ed::Suspend();
							ImGui::OpenPopup("Create New Node");
							ed::Resume();
						}
					}
				}
				else
					newLinkPin = nullptr;

				ed::EndCreate();

				if (ed::BeginDelete())
				{
					ed::NodeId nodeId = 0;
					while (ed::QueryDeletedNode(&nodeId))
					{
						if (ed::AcceptDeletedItem())
						{
							auto id = std::find_if(m_Nodes.begin(), m_Nodes.end(), [nodeId](auto& node) { return node->ID == nodeId; });
							if (id != m_Nodes.end())
								m_Nodes.erase(id);
						}
					}

					ed::LinkId linkId = 0;
					while (ed::QueryDeletedLink(&linkId))
					{
						if (ed::AcceptDeletedItem())
						{
							auto id = std::find_if(m_Links.begin(), m_Links.end(), [linkId](auto& link) { return link.ID == linkId; });
							if (id != m_Links.end())
								m_Links.erase(id);
						}
					}
				}
				ed::EndDelete();
			}
			ImGui::SetCursorScreenPos(cursorTopLeft);
			auto openPopupPosition = ImGui::GetMousePos();
			ed::Suspend();
			if (ed::ShowNodeContextMenu(&contextNodeId))
				ImGui::OpenPopup("Node Context Menu");
			else if (ed::ShowPinContextMenu(&contextPinId))
				ImGui::OpenPopup("Pin Context Menu");
			else if (ed::ShowLinkContextMenu(&contextLinkId))
				ImGui::OpenPopup("Link Context Menu");
			else if (ed::ShowBackgroundContextMenu())
			{
				ImGui::OpenPopup("Create New Node");
				newNodeLinkPin = nullptr;
			}
			ed::Resume();
			ed::Suspend();
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
			if (ImGui::BeginPopup("Node Context Menu"))
			{
				auto node = FindNode(contextNodeId);

				ImGui::TextUnformatted("Node Context Menu");
				ImGui::Separator();
				if (node)
				{
					ImGui::Text("ID: %p", node->ID.AsPointer());
					ImGui::Text("Type: %s", node->Type == Geomerty::NodeType::Blueprint ? "Blueprint" : (node->Type == Geomerty::NodeType::Tree ? "Tree" : "Comment"));
					ImGui::Text("Inputs: %d", (int)node->Inputs.size());
					ImGui::Text("Outputs: %d", (int)node->Outputs.size());
				}
				else
					ImGui::Text("Unknown node: %p", contextNodeId.AsPointer());
				ImGui::Separator();
				if (ImGui::MenuItem("Delete"))
					ed::DeleteNode(contextNodeId);
				ImGui::EndPopup();
			}
			if (ImGui::BeginPopup("Pin Context Menu"))
			{
				auto pin = FindPin(contextPinId);

				ImGui::TextUnformatted("Pin Context Menu");
				ImGui::Separator();
				if (pin)
				{
					ImGui::Text("ID: %p", pin->ID.AsPointer());
					if (pin->Node)
						ImGui::Text("Node: %p", pin->Node->ID.AsPointer());
					else
						ImGui::Text("Node: %s", "<none>");
				}
				else
					ImGui::Text("Unknown pin: %p", contextPinId.AsPointer());

				ImGui::EndPopup();
			}
			if (ImGui::BeginPopup("Link Context Menu"))
			{
				auto link = FindLink(contextLinkId);

				ImGui::TextUnformatted("Link Context Menu");
				ImGui::Separator();
				if (link)
				{
					ImGui::Text("ID: %p", link->ID.AsPointer());
					ImGui::Text("From: %p", link->StartPinID.AsPointer());
					ImGui::Text("To: %p", link->EndPinID.AsPointer());
				}
				else
					ImGui::Text("Unknown link: %p", contextLinkId.AsPointer());
				ImGui::Separator();
				if (ImGui::MenuItem("Delete"))
					ed::DeleteLink(contextLinkId);
				ImGui::EndPopup();
			}

			if (ImGui::BeginPopup("Create New Node"))
			{
				auto newNodePostion = openPopupPosition;
				//ImGui::SetCursorScreenPos(ImGui::GetMousePosOnOpeningCurrentPopup());

				//auto drawList = ImGui::GetWindowDrawList();
				//drawList->AddCircleFilled(ImGui::GetMousePosOnOpeningCurrentPopup(), 10.0f, 0xFFFF00FF);

				Geomerty::Node* node = nullptr;
				if (ImGui::MenuItem("String"))
					node = SpawnStringNode();
				if (ImGui::MenuItem("ReadMesh"))
					node = SpawnRead_MeshNode();
				if (node)
				{
					createNewNode = false;
					ed::SetNodePosition(node->ID, newNodePostion);

					if (auto startPin = newNodeLinkPin)
					{
						auto& pins = startPin->Kind == Geomerty::PinKind::Input ? node->Outputs : node->Inputs;

						for (auto& pin : pins)
						{
							if (CanCreateLink(startPin, &pin))
							{
								auto endPin = &pin;
								if (startPin->Kind == Geomerty::PinKind::Input)
									std::swap(startPin, endPin);

								m_Links.emplace_back(Geomerty::Link(Geomerty::GetNextId(), startPin->ID, endPin->ID));
								m_Links.back().Color = GetIconColor(startPin->Type);

								break;
							}
						}
					}
				}

				ImGui::EndPopup();
			}
			else
				createNewNode = false;
			ImGui::PopStyleVar();
			ed::Resume();
			ed::End();
			ed::SetCurrentEditor(nullptr);
		}
		void Execute() {
			for (auto& n : m_Nodes) {
				bool link_all = true;
				std::vector<Geomerty::NodeData*>data_arr;
				for (auto in_put : n->Inputs) {
					if (!IsPinLinked(in_put.ID)) {
						link_all = false;
						break;
					}
					auto start_pin = FindConnectedStartPin(in_put.ID);
					data_arr.push_back(GetPinData(start_pin->index));
				}
				if (link_all) {
					Geomerty::ExetContex ctx{ data_arr };
					n->Execute(&ctx,registry);
				}

			}
		}
		template<typename T>
		T* GetNode(int index) {
			auto Node_data = GetPinData(m_Nodes[index]->Outputs[0].index);
			return Node_data->Get<T>();
		}
	};


}
