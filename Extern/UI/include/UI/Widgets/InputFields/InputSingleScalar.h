

#pragma once

#include <tools/Event.h>

#include "UI/Widgets/DataWidget.h"

namespace UI::Widgets::InputFields
{

	template <typename T>
	class InputSingleScalar : public DataWidget<T>
	{
		static_assert(std::is_scalar<T>::value, "Invalid InputSingleScalar T (Scalar expected)");

	public:

		InputSingleScalar
		(
			ImGuiDataType p_dataType,
			T p_defaultValue,
			T p_step,
			T p_fastStep,
			const std::string& p_label,
			const std::string& p_format,
			bool p_selectAllOnClick
		) : DataWidget<T>(value), m_dataType(p_dataType), value(p_defaultValue), step(p_step), fastStep(p_fastStep), label(p_label), format(p_format), selectAllOnClick(p_selectAllOnClick) {}

	protected:
		void _Draw_Impl() override
		{
			T previousValue = value;

			ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;
			if (selectAllOnClick)
				flags |= ImGuiInputTextFlags_AutoSelectAll;

			bool enterPressed = ImGui::InputScalar((label + this->m_widgetID).c_str(), m_dataType, &value, step != 0.0f ? &step : nullptr, fastStep != 0.0f ? &fastStep : nullptr, format.c_str(), flags);

			if (previousValue != value)
			{
				ContentChangedEvent.Invoke(value);
				this->NotifyChange();
			}

			if (enterPressed)
				EnterPressedEvent.Invoke(value);
		}

	public:
		T value;
		T step;
		T fastStep;
		std::string label;
		std::string format;
		bool selectAllOnClick;
		Tools::Eventing::Event<T> ContentChangedEvent;
		Tools::Eventing::Event<T> EnterPressedEvent;

	private:
		ImGuiDataType m_dataType;
	};
}