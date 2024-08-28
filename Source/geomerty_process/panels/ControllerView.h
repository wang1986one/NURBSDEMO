#pragma once
#include "glviewer/Viewer.h"
#include<UI/Panels/AView.h>
using vec3 = Eigen::Vector3<float>;
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
		void Intersection(std::vector<vec3>& pos_arr);

		std::vector<std::vector<vec3>*>arr;
		void _Render_Impl()override;
		void  _Draw_ImplInWindow()  override;
	public:
		Geomerty::Viewer viewer;
	};
}