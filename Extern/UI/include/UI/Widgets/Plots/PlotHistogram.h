#pragma once
#include <Maths/FVector2.h>
#include "UI/Widgets/Plots/APlot.h"

namespace UI::Widgets::Plots
{

	class PlotHistogram : public APlot
	{
	public:

		PlotHistogram
		(
			const std::vector<float>& p_data = std::vector<float>(),
			float p_minScale = std::numeric_limits<float>::min(),
			float p_maxScale = std::numeric_limits<float>::max(),
			const Maths::FVector2& p_size = { 0.0f, 0.0f },
			const std::string& p_overlay = "",
			const std::string& p_label = "",
			int p_forceHover = -1
		);

	protected:
		void _Draw_Impl() override;
	};
}