#include <unordered_set>
#include <unordered_map>
#include <queue>
#include "graph.h"
#include "nodes/string_node.h"
#include "nodes/readmesh_node.h"
#include "nodes/smoothmesh_node.h"
#include "nodes/shapes_node.h"
#include "nodes/parametrization_node.h"
#include "nodes/nurbs_arc_node.h"
#include "nodes/nurbscurve_loadnode.h"
#include "nodes/nurbssurface_create_node.h"

void DrawIcon(ImDrawList* drawList, const ImVec2& a, const ImVec2& b, bool filled, ImU32 color, ImU32 innerColor)
{
	auto rect = ImRect(a, b);
	auto rect_x = rect.Min.x;
	auto rect_y = rect.Min.y;
	auto rect_w = rect.Max.x - rect.Min.x;
	auto rect_h = rect.Max.y - rect.Min.y;
	auto rect_center_x = (rect.Min.x + rect.Max.x) * 0.5f;
	auto rect_center_y = (rect.Min.y + rect.Max.y) * 0.5f;
	auto rect_center = ImVec2(rect_center_x, rect_center_y);
	const auto outline_scale = rect_w / 24.0f;
	const auto extra_segments = static_cast<int>(2 * outline_scale); // for full circle
	auto triangleStart = rect_center_x + 0.32f * rect_w;
	auto rect_offset = -static_cast<int>(rect_w * 0.25f * 0.25f);
	rect.Min.x += rect_offset;
	rect.Max.x += rect_offset;
	rect_x += rect_offset;
	rect_center_x += rect_offset * 0.5f;
	rect_center.x += rect_offset * 0.5f;
	const auto c = rect_center;
	if (!filled)
	{
		const auto r = 0.5f * rect_w / 2.0f - 0.5f;

		if (innerColor & 0xFF000000)
			drawList->AddCircleFilled(c, r, innerColor, 12 + extra_segments);
		drawList->AddCircle(c, r, color, 12 + extra_segments, 2.0f * outline_scale);
	}
	else
	{
		drawList->AddCircleFilled(c, 0.5f * rect_w / 2.0f, color, 12 + extra_segments);
	}
	const auto triangleTip = triangleStart + rect_w * (0.45f - 0.32f);

	drawList->AddTriangleFilled(
		ImVec2(ceilf(triangleTip), rect_y + rect_h * 0.5f),
		ImVec2(triangleStart, rect_center_y + 0.15f * rect_h),
		ImVec2(triangleStart, rect_center_y - 0.15f * rect_h),
		color);

}
void DrawPinIcon(const Geomerty::Pin& pin, bool connected, int alpha)
{


	ImColor  color = pin.Kind == Geomerty::PinKind::Input ? ImColor(68, 201, 156) : ImColor(255, 48, 48);
	color.Value.w = alpha / 255.0f;
	const int m_PinIconSize = 24;
	auto size = ImVec2(static_cast<float>(m_PinIconSize), static_cast<float>(m_PinIconSize));
	if (ImGui::IsRectVisible(size))
	{
		auto cursorPos = ImGui::GetCursorScreenPos();
		auto drawList = ImGui::GetWindowDrawList();
		DrawIcon(drawList, cursorPos, cursorPos + size, connected, ImColor(color), ImColor(32, 32, 32, alpha));
	}

	ImGui::Dummy(ImVec2(static_cast<float>(18), static_cast<float>(24)));
};
namespace Geomerty {
	static int uniqueId = 1;
	int GetNextId() {
		return uniqueId++;
	}
	namespace ed = ax::NodeEditor;
	Geomerty::Node* Graph::FindNode(ed::NodeId id)
	{
		if (m_Node_Id_map.find(id.Get()) != m_Node_Id_map.end()) {
			return m_Node_Id_map[id.Get()];
		}
		for (auto& node : m_Nodes)
			if (node->ID == id) {
				m_Node_Id_map[id.Get()] = node;
				return node;
			}

		return nullptr;
	}
	Geomerty::Link* Graph::FindLink(ed::LinkId id)
	{
		if (m_Link_Id_map.find(id.Get()) != m_Link_Id_map.end()) {
			return m_Link_Id_map[id.Get()];
		}
		for (auto& link : m_Links)
			if (link.ID == id) {
				m_Link_Id_map[id.Get()] = &link;
				return &link;
			}
		return nullptr;
	}
	Geomerty::Pin* Graph::FindPin(ed::PinId id)
	{
		if (!id)
			return nullptr;
		if (m_Pin_Id_map.find(id.Get()) != m_Pin_Id_map.end()) {
			return m_Pin_Id_map[id.Get()];
		}
		for (auto& node : m_Nodes)
		{
			for (auto& pin : node->Inputs)
				if (pin.ID == id) {
					m_Pin_Id_map[id.Get()] = &pin;
					return &pin;
				}


			for (auto& pin : node->Outputs)
				if (pin.ID == id) {
					m_Pin_Id_map[id.Get()] = &pin;
					return &pin;
				}

		}
		return nullptr;
	}
	bool Graph::IsPinLinked(ed::PinId id)
	{
		if (!id)
			return false;
		for (auto& link : m_Links)
			if (link.StartPinID == id || link.EndPinID == id)
				return true;
		return false;
	}
	Geomerty::Pin* Graph::FindConnectedStartPin(ed::PinId id) {
		for (auto& link : m_Links) {
			if (link.EndPinID == id)
			{
				return FindPin(link.StartPinID);
			}
		}
		return nullptr;
	}
	bool Graph::CanCreateLink(Geomerty::Pin* a, Geomerty::Pin* b)
	{
		if (!a || !b || a == b || a->Kind == b->Kind || a->Type != b->Type || a->Node == b->Node)
			return false;

		return true;
	}

	int Graph::GetSelectNode() {
		std::vector<ed::NodeId> selectedNodes;
		selectedNodes.resize(ed::GetSelectedObjectCount());
		int nodeCount = ed::GetSelectedNodes(selectedNodes.data(), static_cast<int>(selectedNodes.size()));
		selectedNodes.resize(nodeCount);
		for (int i = 0; i < m_Nodes.size(); i++) {
			bool isSelected = std::find(selectedNodes.begin(), selectedNodes.end(), m_Nodes[i]->ID) != selectedNodes.end();
			if (isSelected) {
				return i;
			}
		}
		return -1;
	}
	void Graph::OnFrame(float deltaTime)
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
			auto HeaderMin = ImGui::GetItemRectMin();
			auto HeaderMax = ImGui::GetItemRectMax();
			for (auto& input : node->Inputs)
			{
				auto alpha = ImGui::GetStyle().Alpha;
				if (newLinkPin && !CanCreateLink(newLinkPin, &input) && &input != newLinkPin)
					alpha = alpha * (48.0f / 255.0f);
				ed::BeginPin(input.ID, ax::NodeEditor::PinKind::Input);
				DrawPinIcon(input, IsPinLinked(input.ID), (int)(alpha * 255));
				ImGui::SameLine();

				if (!input.Name.empty())
				{
					ImGui::TextUnformatted(input.Name.c_str());
				}
				if (input.Type == typeid(bool).hash_code())
				{
					ImGui::Button("Hello");

				}
				ed::EndPin();


			}

			for (auto& output : node->Outputs)
			{
				auto alpha = ImGui::GetStyle().Alpha;
				if (newLinkPin && !CanCreateLink(newLinkPin, &output) && &output != newLinkPin)
					alpha = alpha * (48.0f / 255.0f);
				//ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
				ed::BeginPin(output.ID, ax::NodeEditor::PinKind::Output);
				DrawPinIcon(output, IsPinLinked(output.ID), (int)(alpha * 255));
				ImGui::SameLine();
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
			Geomerty::Node* node = nullptr;
			if (ImGui::MenuItem("String"))
				node = SpawnStringNode();
			if (ImGui::MenuItem("ReadMesh"))
				node = SpawnRead_MeshNode();
			if (ImGui::MenuItem("SmoothMesh"))
				node = SpawnSmoothMesh_Node();
			if (ImGui::MenuItem("ShapeMesh"))
				node = SpawnSpape_Node();
			if (ImGui::MenuItem("Parametrization"))
				node = SpawnParametrization_Node();
			if (ImGui::MenuItem("NurbsArc"))
				node = SpawnNurbsArc_Node();
			if (ImGui::MenuItem("SpawnNurbsCurve_LoadNode"))
				node = SpawnNurbsCurve_LoadNode();
			if (ImGui::MenuItem("SpawnNurbsSurface_Node"))
				node = SpawnNurbsSurface_Node();

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
		//ed::SetCurrentEditor(nullptr);
	}
	bool Graph::GetNodeETX(Geomerty::Node* n, Geomerty::ExetContex& ctx)
	{
		bool link_all = true;
		std::vector<Geomerty::NodeData*>data_arr;
		for (auto in_put : n->Inputs) {
			//may not need
			if (!IsPinLinked(in_put.ID)) {
				link_all = false;
				break;
			}
			auto start_pin = FindConnectedStartPin(in_put.ID);
			data_arr.push_back(GetPinData(start_pin->index));
		}
		if (link_all) {
			ctx.graph = this;
			ctx.inputs = data_arr;
			return true;
		}
		return false;
	}
	std::vector<Node*> Graph::GetNodeExecutuePath(int index)
	{
		assert(index < m_Nodes.size() && index >= 0);
		std::unordered_set<Node*>vis;
		std::unordered_map<Node*, std::vector<Node*>>node_connect;
		std::vector<Node*>stack;
		stack.push_back(m_Nodes[index]);
		bool link_all = true;
		while (!stack.empty())
		{
			auto node = stack.back();
			stack.pop_back();
			if (vis.find(node) == vis.end()) {
				vis.insert(node);
			}
			for (auto input_pin : node->Inputs) {
				auto start_pin = FindConnectedStartPin(input_pin.ID);
				if (start_pin) {
					stack.push_back(start_pin->Node);
					node_connect[start_pin->Node].push_back(node);
				}
				else
				{
					link_all = false;
					break;
				}
			}
			if (!link_all) {
				break;
			}
		}
		if (link_all) {
			std::unordered_map<Node*, int>in_degree;
			for (auto it = vis.begin(); it != vis.end(); it++) {
				in_degree[*it] = 0;
			}
			std::queue<Node*> que;
			for (auto it : node_connect) {

				for (auto child : it.second) {

					in_degree[child]++;
				}
			}
			for (auto it = vis.begin(); it != vis.end(); it++) {
				if (in_degree[*it] == 0) {
					que.push(*it);
				}
			}
			std::vector<Node*>path;
			while (!que.empty())
			{
				Node* cur = que.front(); que.pop();
				path.push_back(cur);
				for (auto it : node_connect[cur]) {
					in_degree[it]--;
					if (in_degree[it] == 0) {
						que.push(it);
					}
				}
			}
			return path;
		}
		return std::vector<Node*>{};
	}
	void Graph::Execute(int to_index) {
		for (auto node : GetNodeExecutuePath(to_index)) {
			Geomerty::ExetContex ctx;
			if (GetNodeETX(node, ctx)) {
				node->Execute(&ctx);
			}
		}
	}
	Geomerty::Node* Graph::SpawnStringNode()
	{
		m_Nodes.push_back(new Geomerty::StringNode(Geomerty::GetNextId(), "StringNode", ImColor(255, 255, 128)));
		m_Nodes.back()->Init(this);
		return m_Nodes.back();
	}
	Geomerty::Node* Graph::SpawnRead_MeshNode()
	{
		m_Nodes.push_back(new Geomerty::Read_MeshNode(Geomerty::GetNextId(), "Read_MeshNode"));
		m_Nodes.back()->Init(this);
		return m_Nodes.back();
	}
	Geomerty::Node* Graph::SpawnSmoothMesh_Node()
	{
		m_Nodes.push_back(new Geomerty::Smooth_MeshNode(Geomerty::GetNextId(), "Smooth_MeshNode"));
		m_Nodes.back()->Init(this);
		return m_Nodes.back();
	}
	Geomerty::Node* Graph::SpawnSpape_Node()
	{
		m_Nodes.push_back(new Geomerty::ShapeNode(Geomerty::GetNextId(), "Shape_MeshNode"));
		m_Nodes.back()->Init(this);
		return m_Nodes.back();
	}
	Geomerty::Node* Graph::SpawnParametrization_Node()
	{
		m_Nodes.push_back(new Geomerty::Parametrization_Node(Geomerty::GetNextId(), "Parametrization_MeshNode"));
		m_Nodes.back()->Init(this);
		return m_Nodes.back();

	}
	Geomerty::Node* Graph::SpawnNurbsArc_Node()
	{
		m_Nodes.push_back(new Geomerty::NurbsArc_Node(Geomerty::GetNextId(), "Nurbs_ArcNode"));
		m_Nodes.back()->Init(this);
		return m_Nodes.back();
	}
	Geomerty::Node* Graph::SpawnNurbsCurve_LoadNode()
	{
		m_Nodes.push_back(new Geomerty::NurbsCurve_LoadNode(Geomerty::GetNextId(), "NurbsCurve_LoadNode"));
		m_Nodes.back()->Init(this);
		return m_Nodes.back();
	}
	Geomerty::Node* Graph::SpawnNurbsSurface_Node()
	{
		m_Nodes.push_back(new Geomerty::NurbsSurface_Node(Geomerty::GetNextId(), "NurbsSurface_Node"));
		m_Nodes.back()->Init(this);
		return m_Nodes.back();
	}
}