

#include "UI/Widgets/Buttons/ButtonColored.h"
#include "UI/Internal/Converter.h"

UI::Widgets::Buttons::ButtonColored::ButtonColored(const std::string & p_label, const Types::Color& p_color, const Maths::FVector2& p_size, bool p_enableAlpha) :
	label(p_label), color(p_color), size(p_size), enableAlpha(p_enableAlpha)
{
}

void UI::Widgets::Buttons::ButtonColored::_Draw_Impl()
{
	ImVec4 imColor = Internal::Converter::ToImVec4(color);

	if (ImGui::ColorButton((label + m_widgetID).c_str(), imColor, !enableAlpha ? ImGuiColorEditFlags_NoAlpha : 0, Internal::Converter::ToImVec2(size)))
		ClickedEvent.Invoke();

	color = Internal::Converter::ToColor(imColor);
}
