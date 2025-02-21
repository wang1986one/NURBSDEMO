

#pragma once

#include <array>

#include "tools/Event.h"

#include "UI/Widgets/DataWidget.h"

namespace UI::Widgets::InputFields
{

	template <typename T, size_t _Size>
	class InputMultipleScalars : public DataWidget<std::array<T, _Size>>
	{
		static_assert(_Size > 1, "Invalid InputMultipleScalars _Size (2 or more requiered)");
		static_assert(std::is_scalar<T>::value, "Invalid InputMultipleScalars T (Scalar expected)");

	public:

		InputMultipleScalars
		(
			ImGuiDataType p_dataType,
			T p_defaultValue,
			T p_step,
			T p_fastStep,
			const std::string& p_label,
			const std::string& p_format,
			bool p_selectAllOnClick
		) : DataWidget<std::array<T, _Size>>(values), m_dataType(p_dataType), step(p_step), fastStep(p_fastStep), label(p_label), format(p_format), selectAllOnClick(p_selectAllOnClick)
		{
			values.fill(p_defaultValue);
		}

	protected:
		void _Draw_Impl() override
		{
			std::array<T, _Size> previousValue = values;

			ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;
			if (selectAllOnClick)
				flags |= ImGuiInputTextFlags_AutoSelectAll;

			bool enterPressed = ImGui::InputScalarN((label + this->m_widgetID).c_str(), m_dataType, values.data(), _Size, step != 0.0f ? &step : nullptr, fastStep != 0.0f ? &fastStep : nullptr, format.c_str(), flags);

			bool hasChanged = false;

			for (size_t i = 0; i < _Size; ++i)
				if (previousValue[i] != values[i])
					hasChanged = true;

			if (hasChanged)
			{
				ContentChangedEvent.Invoke(values);
				this->NotifyChange();
			}

			if (enterPressed)
				EnterPressedEvent.Invoke(values);
		}

	public:
		std::array<T, _Size> values;
		T step;
		T fastStep;
		std::string label;
		std::string format;
		bool selectAllOnClick;
		Tools::Eventing::Event<std::array<T, _Size>&> ContentChangedEvent;
		Tools::Eventing::Event<std::array<T, _Size>&> EnterPressedEvent;

	private:
		ImGuiDataType m_dataType;
	};
}