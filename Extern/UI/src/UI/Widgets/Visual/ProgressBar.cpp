

#include "UI/Widgets/Visual/ProgressBar.h"
#include "UI/Internal/Converter.h"

UI::Widgets::Visual::ProgressBar::ProgressBar(float p_fraction, const Maths::FVector2 & p_size, const std::string & p_overlay) :
	fraction(p_fraction), size(p_size), overlay(p_overlay)
{
}

void UI::Widgets::Visual::ProgressBar::_Draw_Impl()
{
	ImGui::ProgressBar(fraction, Internal::Converter::ToImVec2(size), !overlay.empty() ? overlay.c_str() : nullptr);
}
