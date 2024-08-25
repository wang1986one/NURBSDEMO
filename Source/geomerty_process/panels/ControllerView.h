#pragma once
#include "glviewer/Viewer.h"
#include<UI/Panels/AView.h>
namespace Geomerty {
	class ControllerView : public UI::Panels::AView
	{
	public:

		ControllerView
		(
			const std::string& p_title,
			bool p_opened,
			const UI::Settings::PanelWindowSettings& p_windowSettings
		);
		void Update(float p_deltaTime)override;
		void _Render_Impl()override;
	public:
		Geomerty::Viewer viewer;
	};
}