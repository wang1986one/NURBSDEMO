#pragma once

#include <filesystem>
#include <unordered_map>
#include <queue>

#include <UI/Panels/PanelWindow.h>
#include <UI/Widgets/Layout/TreeNode.h>
#include<UI/Widgets/Layout/Group.h>


namespace Geomerty
{

	class AssetBrowser : public UI::Panels::PanelWindow
	{
	public:

		AssetBrowser
		(
			const std::string& p_title,
			bool p_opened,
			const UI::Settings::PanelWindowSettings& p_windowSettings,
			const std::string& p_projectAssetFolder = ""
		);


		void Fill();


		void Clear();

	
		void Refresh();

	private:
		void ParseFolder(UI::Widgets::Layout::TreeNode& p_root, const std::filesystem::directory_entry& p_directory, bool p_isEngineItem, bool p_scriptFolder = false);
		void ConsiderItem(UI::Widgets::Layout::TreeNode* p_root, const std::filesystem::directory_entry& p_entry, bool p_isEngineItem, bool p_autoOpen = false, bool p_scriptFolder = false);

	public:
		static const std::string __FILENAMES_CHARS;

	private:

		std::string m_projectAssetFolder;

		UI::Widgets::Layout::Group* m_assetList;
		std::unordered_map<UI::Widgets::Layout::TreeNode*, std::string> m_pathUpdate;
	};
}