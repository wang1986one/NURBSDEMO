#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include <imgui_internal.h>
#include <imgui_node_editor.h>
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "ImGuizmo.h"
#include "nodes.h"
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Device.h"
#include "WindowSettings.h"
#include "Window.h"
#include "InputManager.h"
#include "UI/Core/UIManager.h"
#include "UI/Styling/EStyle.h"
//#include "tools/Clock.h"
#include "UI/Panels/PanelsManager.h"
#include "UI/Panels/AView.h"

namespace ed = ax::NodeEditor;
# ifdef _MSC_VER
# define portable_strcpy    strcpy_s
# define portable_sprintf   sprintf_s
# else
# define portable_strcpy    strcpy
# define portable_sprintf   sprintf
# endif
Windowing::Settings::WindowSettings windowSettings;
Windowing::Settings::DeviceSettings deviceSettings;
std::unique_ptr<Windowing::Context::Device>	device;
std::unique_ptr<Windowing::Window> window;
std::unique_ptr<Windowing::Inputs::InputManager>inputManager;
std::unique_ptr<UI::Core::UIManager>uiManager;
UI::Settings::PanelWindowSettings settings;
std::unique_ptr<UI::Panels::PanelsManager>m_panelsManager;

static ed::EditorContext* m_Editor = nullptr;
struct Example {
	std::vector<Geomerty::Node*>    m_Nodes;
	std::vector<Geomerty::Link>    m_Links;
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
		m_Nodes.back()->Init();
		return m_Nodes.back();
	}
	Geomerty::Node* SpawnRead_MeshNode()
	{
		m_Nodes.push_back(new Geomerty::Read_MeshNode(Geomerty::GetNextId(), "Read_MeshNode"));
		m_Nodes.back()->Init();
		return m_Nodes.back();
	}
	void OnStart() {
		//ed::Config config;

		//config.SettingsFile = "Blueprints.json";
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
				if (input.Type ==typeid(bool).hash_code())
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
		for (auto& n:m_Nodes) {
			bool link_all = true;
			std::vector<Geomerty::NodeData*>data_arr;
			for (auto in_put : n->Inputs) {
				if (!IsPinLinked(in_put.ID)) {
					link_all = false;
					break;
				}
				auto start_pin=FindConnectedStartPin(in_put.ID);
				data_arr.push_back(Geomerty::GetPinData(start_pin->index));
			}
			if (link_all) {
				Geomerty::ExetContex ctx{data_arr};
				n->Execute(&ctx);
			}

		}
	}
	template<typename T>
	T* GetNode(int index) {
		auto Node_data = Geomerty::GetPinData(m_Nodes[index]->Outputs[0].index);
		return Node_data->Get<T>();
	}
};
#include"gl/Viewer.h"

opengl::glfw::Viewer viewer;
Eigen::MatrixXd V;
Eigen::MatrixXi F;
int main()
{
	deviceSettings.contextMajorVersion = 4;
	deviceSettings.contextMinorVersion = 6;
	windowSettings.title = "Geomery";
	windowSettings.width = 1600;
	windowSettings.height = 900;
	windowSettings.maximized = true;
	device = std::make_unique<Windowing::Context::Device>(deviceSettings);
	window = std::make_unique<Windowing::Window>(*device, windowSettings);
	window->SetIcon("Res/Texture/awesomeface.png");
	inputManager = std::make_unique<Windowing::Inputs::InputManager>(*window);;
	window->MakeCurrentContext();

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		return -1;
	}
	device->SetVsync(true);

	//初始化UI
	uiManager = std::make_unique<UI::Core::UIManager>(window->GetGlfwWindow(), UI::Styling::EStyle::ALTERNATIVE_DARK);;
	uiManager->LoadFont("Ruda_Big", "Res/Font/Ruda-Bold.ttf", 25);
	uiManager->LoadFont("Ruda_Small", "Res/Font/Ruda-Bold.ttf", 12);
	uiManager->LoadFont("Ruda_Medium", "Res/Font/Ruda-Bold.ttf", 14);
	uiManager->UseFont("Ruda_Big");
	uiManager->EnableDocking(true);
	settings.closable = true;
	settings.collapsable = true;
	settings.dockable = true;
	UI::Modules::Canvas m_canvas;
	m_panelsManager = std::make_unique<UI::Panels::PanelsManager>(m_canvas);
	m_panelsManager->CreatePanel<UI::Panels::MenuBar>("Menu Bar");
	m_panelsManager->CreatePanel<UI::Panels::AView>("Scene View", true, settings);
	m_panelsManager->GetPanelAs<UI::Panels::AView>("Scene View").ResizeEvent += [](int p_width, int p_height) {
		viewer.post_resize(p_width, p_height);
		};
	m_panelsManager->GetPanelAs<UI::Panels::AView>("Scene View").DrawInWindow += []() {
		};
	m_canvas.MakeDockspace(true);
	uiManager->SetCanvas(m_canvas);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glEnable(GL_BLEND);
	glfwSwapInterval(0);
	Example ex;
	ex.OnStart();
	viewer.append_mesh();
    viewer.init();
	viewer.post_resize(800, 600);
	while (!window->ShouldClose())
	{
		glClearColor(0., 0., 0., 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		m_panelsManager->GetPanelAs<UI::Panels::AView>("Scene View").Update(1);
		m_panelsManager->GetPanelAs<UI::Panels::AView>("Scene View").Bind();
		viewer.draw();
		m_panelsManager->GetPanelAs<UI::Panels::AView>("Scene View").UnBind();
		uiManager->Render();
		if (ImGui::Begin("widgets", nullptr, ImGuiWindowFlags_None)) {
			ex.OnFrame(0.01f);
		}
		ImGui::End();
		if (ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_None)) {
			if (ImGui::Button("Execute")) {
				ex.Execute();
				auto mesh=ex.GetNode<Geomerty::SurfaceMesh>(1);
				Eigen::MatrixXd SV;
				auto& pos=mesh->positions();
				SV.resize(pos.size(),3);
				for (int i = 0; i < pos.size(); i++) {
					SV.row(i) << pos[i][0], pos[i][1], pos[i][2];
				}
				//"D:\Project\C++\opengl\DX11\data\SHREC2011\alien-1.obj"
				Eigen::MatrixXi SF;
				if(mesh->is_quad_mesh())
				   SF.resize(mesh->faces_size(), 4);
				else
				   SF.resize(mesh->faces_size(), 3);
				auto& faces=mesh->faces();
				int j = 0;
				for (auto f : faces) {
					auto it = mesh->vertices(f).begin();
					int x = (*it++).idx();
					int y = (*it++).idx();
					int z = (*it++).idx();
					//int w = (*it++).idx();
					SF.row(j) << x,y,z;

					j++;
				}
				viewer.data(0).clear();
				viewer.data(0).set_mesh(SV, SF);
			}
		}
		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		device->PollEvents();
		window->SwapBuffers();
		if (m_panelsManager->GetPanelAs<UI::Panels::AView>("Scene View").IsHovered()) {
			viewer.mouse_scroll(inputManager->GetMouseScrollOffset());
			if (inputManager->IsMouseButtonPressed(Windowing::Inputs::EMouseButton::MOUSE_BUTTON_LEFT))
				viewer.mouse_down(opengl::glfw::Viewer::MouseButton::Left);
			if (inputManager->IsMouseButtonReleased(Windowing::Inputs::EMouseButton::MOUSE_BUTTON_LEFT))
				viewer.mouse_up(opengl::glfw::Viewer::MouseButton::Left);
			if (inputManager->IsMouseButtonPressed(Windowing::Inputs::EMouseButton::MOUSE_BUTTON_MIDDLE))
				viewer.mouse_down(opengl::glfw::Viewer::MouseButton::Middle);
			if (inputManager->IsMouseButtonReleased(Windowing::Inputs::EMouseButton::MOUSE_BUTTON_MIDDLE))
				viewer.mouse_up(opengl::glfw::Viewer::MouseButton::Middle);
			if (inputManager->IsMouseButtonPressed(Windowing::Inputs::EMouseButton::MOUSE_BUTTON_RIGHT))
				viewer.mouse_down(opengl::glfw::Viewer::MouseButton::Right);
			if (inputManager->IsMouseButtonReleased(Windowing::Inputs::EMouseButton::MOUSE_BUTTON_RIGHT))
				viewer.mouse_up(opengl::glfw::Viewer::MouseButton::Right);
			auto [x,y]=inputManager->GetMousePosition();
			viewer.mouse_move(x,y);
		}

		
		inputManager->ClearEvents();
	}
	//回收资源
	device.reset();
	uiManager.reset();
	m_panelsManager.reset();
	inputManager.reset();
	window.reset();
}

