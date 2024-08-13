#include"graph.h"
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Device.h"
#include "WindowSettings.h"
#include "Window.h"
#include "InputManager.h"
#include "UI/Core/UIManager.h"
#include "UI/Styling/EStyle.h"
#include "gl/Viewer.h"
#include "UI/Panels/PanelsManager.h"
#include "UI/Panels/AView.h"

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
opengl::glfw::Viewer viewer;
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
	Geomerty::Graph ex;
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

