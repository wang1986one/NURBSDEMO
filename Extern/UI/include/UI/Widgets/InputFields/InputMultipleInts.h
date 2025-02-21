

#pragma once

#include "UI/Widgets/InputFields/InputMultipleScalars.h"

namespace UI::Widgets::InputFields
{

	template <size_t _Size>
	class InputMultipleInts : public InputMultipleScalars<int, _Size>
	{
	public:

		InputMultipleInts
		(
			int p_defaultValue = 0,
			int p_step = 1,
			int p_fastStep = 0,
			const std::string& p_label = "",
			const std::string& p_format = "%d",
			bool p_selectAllOnClick = true
		) : InputMultipleScalars<int, _Size>(ImGuiDataType_::ImGuiDataType_S32, p_defaultValue, p_step, p_fastStep, p_label, p_format, p_selectAllOnClick) {}
	};
}