#include "core/graph.h"
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Device.h"
#include "WindowSettings.h"
#include "Window.h"
#include "InputManager.h"
#include "core/ServiceLocator.h"
#include "UI/Core/UIManager.h"
#include "UI/Styling/EStyle.h"
#include "glviewer/Viewer.h"
#include "UI/Panels/PanelsManager.h"
#include "panels/ControllerView.h"
#include "panels/AssetBrowser.h"
Windowing::Settings::WindowSettings windowSettings;
Windowing::Settings::DeviceSettings deviceSettings;
std::unique_ptr<Windowing::Context::Device>	device;
std::unique_ptr<Windowing::Window> window;
std::unique_ptr<Windowing::Inputs::InputManager>inputManager;
std::unique_ptr<UI::Core::UIManager>uiManager;
UI::Settings::PanelWindowSettings settings;
std::unique_ptr<UI::Panels::PanelsManager>m_panelsManager;
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
	Geomerty::ServiceLocator::Provide<Windowing::Inputs::InputManager>(*inputManager);
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
	Geomerty::ServiceLocator::Provide<UI::Panels::PanelsManager>(*m_panelsManager);
	m_panelsManager->CreatePanel<UI::Panels::MenuBar>("Menu Bar");
	m_panelsManager->CreatePanel<Geomerty::ControllerView>("Scene View", true, settings);
	m_panelsManager->CreatePanel<Geomerty::AssetBrowser>("Asset Browser", true, settings, "Res\\");
	m_canvas.MakeDockspace(true);
	uiManager->SetCanvas(m_canvas);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glEnable(GL_BLEND);
	glfwSwapInterval(0);
	Geomerty::Graph ex;
	Geomerty::ServiceLocator::Provide<Geomerty::Graph>(ex);
	ex.OnStart();
	while (!window->ShouldClose())
	{
		device->PollEvents();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		m_panelsManager->GetPanelAs<Geomerty::ControllerView>("Scene View").Update(1);
		m_panelsManager->GetPanelAs<Geomerty::ControllerView>("Scene View").Render();

		uiManager->Render();
		if (ImGui::Begin("NodeGraph", nullptr, ImGuiWindowFlags_None)) {
			ex.OnFrame(0.01f);
		}
		ImGui::End();

		if (ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_None)) {
			int index = ex.GetSelectNode();
			if (index != -1) {
				ex.m_Nodes[index]->InstallUi();
			}
			if (ImGui::Button("Execute")) {
				ex.Execute(index);
				if (index != -1)
					ex.m_Nodes[index]->Present(m_panelsManager->GetPanelAs<Geomerty::ControllerView>("Scene View").viewer);
			}
		}
		ImGui::End();
		//ImGui::ShowDemoWindow();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		window->SwapBuffers();
		inputManager->ClearEvents();

	}
	//回收资源
	device.reset();
	uiManager.reset();
	m_panelsManager.reset();
	inputManager.reset();
	window.reset();
}

