
#include "SaveFileDialog.h"

#include <Windows.h>

Windowing::Dialogs::SaveFileDialog::SaveFileDialog(const std::string & p_dialogTitle) : FileDialog(GetSaveFileNameA, p_dialogTitle)
{
}

void Windowing::Dialogs::SaveFileDialog::Show(EExplorerFlags p_flags)
{
	FileDialog::Show(p_flags);

	if (m_succeeded)
		AddExtensionToFilePathAndName();
}

void Windowing::Dialogs::SaveFileDialog::DefineExtension(const std::string & p_label, const std::string & p_extension)
{
	m_filter = p_label + '\0' + '*' + p_extension + '\0';
	m_extension = p_extension;
}

void Windowing::Dialogs::SaveFileDialog::AddExtensionToFilePathAndName()
{
	if (m_filename.size() >= m_extension.size())
	{
		std::string fileEnd(m_filename.data() + m_filename.size() - m_extension.size(), m_filename.data() + m_filename.size());

		if (fileEnd != m_extension)
		{
			m_filepath += m_extension;
			m_filename += m_extension;
		}
	}
	else
	{
		m_filepath += m_extension;
		m_filename += m_extension;
	}
}
