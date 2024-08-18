#include "panels/ControllerView.h"
#include "InputManager.h"
#include "core/ServiceLocator.h"

namespace Geomerty {
	ControllerView::ControllerView(const std::string& p_title, bool p_opened, const UI::Settings::PanelWindowSettings& p_windowSettings):AView(p_title, p_opened,p_windowSettings)
	{
		viewer.append_mesh();
		viewer.init();
		viewer.post_resize(800,900);
		ResizeEvent+= [&](int p_width, int p_height) {
			viewer.post_resize(p_width,p_height);
	    };

	}
	void ControllerView::Update(float p_deltaTime)
	{
		AView::Update(p_deltaTime);
		if (IsHovered()) {
			auto& inputManager =Geomerty::ServiceLocator::Get<Windowing::Inputs::InputManager>();
			viewer.mouse_scroll(inputManager.GetMouseScrollOffset());
			if (inputManager.IsMouseButtonPressed(Windowing::Inputs::EMouseButton::MOUSE_BUTTON_LEFT))
				viewer.mouse_down(opengl::glfw::Viewer::MouseButton::Left);
			if (inputManager.IsMouseButtonReleased(Windowing::Inputs::EMouseButton::MOUSE_BUTTON_LEFT))
				viewer.mouse_up(opengl::glfw::Viewer::MouseButton::Left);
			if (inputManager.IsMouseButtonPressed(Windowing::Inputs::EMouseButton::MOUSE_BUTTON_MIDDLE))
				viewer.mouse_down(opengl::glfw::Viewer::MouseButton::Middle);
			if (inputManager.IsMouseButtonReleased(Windowing::Inputs::EMouseButton::MOUSE_BUTTON_MIDDLE))
				viewer.mouse_up(opengl::glfw::Viewer::MouseButton::Middle);
			if (inputManager.IsMouseButtonPressed(Windowing::Inputs::EMouseButton::MOUSE_BUTTON_RIGHT))
				viewer.mouse_down(opengl::glfw::Viewer::MouseButton::Right);
			if (inputManager.IsMouseButtonReleased(Windowing::Inputs::EMouseButton::MOUSE_BUTTON_RIGHT))
				viewer.mouse_up(opengl::glfw::Viewer::MouseButton::Right);
			auto [x, y] = inputManager.GetMousePosition();
			auto [lx,ly]=GetPosition();
			viewer.mouse_move(x-lx, y-ly);
		}

	}
	void ControllerView::_Render_Impl()
	{
		Bind();
		viewer.draw();
		UnBind();
	}
}