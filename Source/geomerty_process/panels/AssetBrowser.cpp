#include <fstream>
#include <iostream>
#include<glad/glad.h>
#include<UI/Widgets/InputFields/InputText.h>
#include <UI/Widgets/Texts/TextClickable.h>
#include <UI/Widgets/Visual/Image.h>
#include <UI/Widgets/Visual/Separator.h>
#include <UI/Widgets/Buttons/Button.h>
#include <UI/Widgets/Layout/Group.h>
#include <UI/Plugins/DDSource.h>
#include <UI/Plugins/DDTarget.h>
#include <UI/Plugins/ContextualMenu.h>
#include <MessageBox.h>
#include <SaveFileDialog.h>
#include <OpenFileDialog.h>
#include"tools/SystemCallls.h"
#include <tools/PathParser.h>
#include <tools/String.h>
#include "core/ServiceLocator.h"
#include "panels/AssetBrowser.h"
#define FILENAMES_CHARS Geomerty::AssetBrowser::__FILENAMES_CHARS
const std::string FILENAMES_CHARS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.-_=+ 0123456789()[]";
void RenameAsset(const std::string& p_prev, const std::string& p_new)
{
	std::filesystem::rename(p_prev, p_new);
}
static bool ImportAssetAtLocation(const std::string& p_destination)
{
	using namespace Windowing::Dialogs;
	std::string modelFormats = "*.fbx;*.obj;";
	std::string textureFormats = "*.png;*.jpeg;*.jpg;*.tga;";
	OpenFileDialog selectAssetDialog("Select an asset to import");
	selectAssetDialog.AddFileType("Any supported format", modelFormats + textureFormats );
	selectAssetDialog.AddFileType("Model (.fbx, .obj)", modelFormats);
	selectAssetDialog.AddFileType("Texture (.png, .jpeg, .jpg, .tga)", textureFormats);
	selectAssetDialog.Show();
	if (selectAssetDialog.HasSucceeded())
	{
		std::string source = selectAssetDialog.GetSelectedFilePath();
		std::string destination = p_destination + selectAssetDialog.GetSelectedFileName();
		if (!std::filesystem::exists(destination) || MessageBox("File already exists", "The destination you have selected already exists, importing this file will erase the previous file content, are you sure about that?", MessageBox::EMessageType::WARNING, MessageBox::EButtonLayout::OK_CANCEL).GetUserAction() == MessageBox::EUserAction::OK)
		{
			std::filesystem::copy(source, destination, std::filesystem::copy_options::overwrite_existing);
			std::cout << ("Asset \"" + destination + "\" imported") << std::endl;
			return true;
		}
	}
	return false;
}
std::pair<GLuint, GLuint> CreateFromMemory()
{
	static std::pair<GLuint, GLuint> textureID;
	static bool flag = false;
	if (!flag) {
		uint32_t p_width = 16 ;
		uint32_t p_height = 16;
		std::vector<uint64_t> raw_folder = { 672380918437235916,672380918506505954,672380918506505954,672380918506505954,672380918506505954,672380918506505954,672380918506505954,374868370703828706,18395743157888929263,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,17386935742081191663,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,18395743158123810543,17953244669062593767,17953244669062593767,17953244669062593767,17953244669062593767,17953244669062593767,17953244669062593767,17953244669062593767,17953244669062593767,18381084451919018731,18381084451919018731,18381084451919018731,18381084451919018731,18381084451919018731,18381084451919018731,18381084451919018731,18381084451919018731,18381084451919018731,18381084451919018731,18381084451919018731,15786137912587238123,15209389057434098371,15209389057434098371,15209389057434098371,13912056523109079747,18381084451919018731,18381084451919018731,18381084451919018731,3248989691069250283,0,0,0,0,18381084451684137706,18381084451919018731,18381084451919018731,2384578968093637355,0,0,0,0,656618319738096844,656618319807039202,656618319807039202,103470335,0,0,0,0 };
		std::vector<uint64_t> raw_model = { 0,0,0,13108830535611575275,3555786492544789245,0,0,0,0,0,10298584367746772975,18441092497958169322,18247081553890420479,1544091451884154109,0,0,0,7488338200050532351,18441092497689799659,18441092498042121195,18390632696796194559,17310895777055811327,403579039149436924,0,4678092032504233984,18441092497253526250,18441092498042121195,18441092498042121195,18390632696796194559,18390632696796194559,15726755707639867135,1246934014,18002812775071481856,18441092498041726437,18441092498042121195,18441092498042121195,18390632696796194559,18390632696796194559,18387811337074419455,4262694642,18072609738842963968,18436006079940578508,18441092498041989609,18441092498042121195,18390632696796194559,18389222019082790655,18375113012858556916,4278552302,18072609738842963968,18432332555757604044,18437136395073933006,18441092498042055402,18390350122307856127,18375959641107437559,18374831537880335341,4278552302,18072609738842963968,18432332555757604044,18432332555757604044,18438549288990659025,18377370323116399866,18374831537880335341,18374831537880335341,4278552302,18072609738842963968,18432332555757604044,18432332555757604044,18438549288990593232,18377088848139623418,18374831537880335341,18374831537880335341,4278552302,18072609738842963968,18432332555757604044,18437136395073933006,18441092498042055402,18390350122307856127,18375678166130661111,18374831537880335341,4278552302,18072609738842963968,18435723501157272780,18441092498041923816,18441092498042121195,18390632696796194559,18389222014787823359,18374831537881780724,4278552302,18074587790326104064,18441092498041726437,18441092498042121195,18441092498042121195,18390632696796194559,18390632696796194559,18387528758291113727,4262629106,4605751859683000320,18441092497236814827,18441092498042121195,18441092498042121195,18390632696796194559,18390632696796194559,15726755707639867135,1246932987,0,7416280606012604415,18441092497689799659,18441092498042121195,18390632696796194559,17310895777055811327,403579039149436156,0,0,0,10298584367746772975,18441092497958169322,18247081553890420479,1544091451884154109,0,0,0,0,0,13108830535611575275,3555786492544789245,0,0,0 };
		glGenTextures(1, &textureID.first);
		glBindTexture(GL_TEXTURE_2D, textureID.first);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, p_width, p_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, reinterpret_cast<uint8_t*>(raw_folder.data()));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		glGenTextures(1, &textureID.second);
		glBindTexture(GL_TEXTURE_2D, textureID.second);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, p_width, p_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, reinterpret_cast<uint8_t*>(raw_model.data()));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		flag = true;
	}
	return textureID;
}
class BrowserItemContextualMenu : public UI::Plugins::ContextualMenu
{
public:
	BrowserItemContextualMenu(const std::string p_filePath, bool p_protected = false) : m_protected(p_protected), filePath(p_filePath) {}
	virtual void CreateList()
	{
		if (!m_protected)
		{
			auto& renameMenu = CreateWidget<UI::Widgets::Menu::MenuList>("Rename to...");
			auto& deleteAction = CreateWidget<UI::Widgets::Menu::MenuItem>("Delete");
			auto& nameEditor = renameMenu.CreateWidget<UI::Widgets::InputFields::InputText>("");
			nameEditor.selectAllOnClick = true;

			renameMenu.ClickedEvent += [this, &nameEditor]
				{
					nameEditor.content = Tools::Utils::PathParser::GetElementName(filePath);

					if (!std::filesystem::is_directory(filePath))
						if (size_t pos = nameEditor.content.rfind('.'); pos != std::string::npos)
							nameEditor.content = nameEditor.content.substr(0, pos);
				};
			deleteAction.ClickedEvent += [this] { DeleteItem(); };
			nameEditor.EnterPressedEvent += [this](std::string p_newName)
				{
					if (!std::filesystem::is_directory(filePath))
						p_newName += '.' + Tools::Utils::PathParser::GetExtension(filePath);
					p_newName.erase(std::remove_if(p_newName.begin(), p_newName.end(), [](auto& c)
						{
							return std::find(FILENAMES_CHARS.begin(), FILENAMES_CHARS.end(), c) == FILENAMES_CHARS.end();
						}), p_newName.end());
					std::string containingFolderPath = Tools::Utils::PathParser::GetContainingFolder(filePath);
					std::string newPath = containingFolderPath + p_newName;
					std::string oldPath = filePath;
					if (filePath != newPath && !std::filesystem::exists(newPath))
						filePath = newPath;
					if (std::filesystem::is_directory(oldPath))
						filePath += '\\';
					RenamedEvent.Invoke(oldPath, newPath);
				};
		}
	}
	virtual void Execute() override
	{
		if (m_widgets.size() > 0)
			UI::Plugins::ContextualMenu::Execute();
	}
	virtual void DeleteItem() = 0;
public:
	bool m_protected;
	std::string filePath;
	Tools::Eventing::Event<std::string> DestroyedEvent;
	Tools::Eventing::Event<std::string, std::string> RenamedEvent;
};

class FolderContextualMenu : public BrowserItemContextualMenu
{
public:
	FolderContextualMenu(const std::string& p_filePath, bool p_protected = false) : BrowserItemContextualMenu(p_filePath, p_protected) {}
	virtual void CreateList() override
	{
		auto& showInExplorer = CreateWidget<UI::Widgets::Menu::MenuItem>("Show in explorer");
		showInExplorer.ClickedEvent += [this]
			{
				Tools::Utils::SystemCalls::ShowInExplorer(filePath);
			};
		if (!m_protected)
		{
			auto& importAssetHere = CreateWidget<UI::Widgets::Menu::MenuItem>("Import Here...");
			importAssetHere.ClickedEvent += [this]
				{
					if (ImportAssetAtLocation(filePath))
					{
						UI::Widgets::Layout::TreeNode* pluginOwner = reinterpret_cast<UI::Widgets::Layout::TreeNode*>(userData);
						pluginOwner->Close();
						pluginOwner->Open();
					}
				};
			auto& createMenu = CreateWidget<UI::Widgets::Menu::MenuList>("Create..");
			auto& createFolderMenu = createMenu.CreateWidget<UI::Widgets::Menu::MenuList>("Folder");
			auto& createFolder = createFolderMenu.CreateWidget<UI::Widgets::InputFields::InputText>("");
			createFolderMenu.ClickedEvent += [&createFolder] { createFolder.content = ""; };
			createFolder.EnterPressedEvent += [this](std::string newFolderName)
				{
					size_t fails = 0;
					std::string finalPath;
					do
					{
						finalPath = filePath + (!fails ? newFolderName : newFolderName + " (" + std::to_string(fails) + ')');
						++fails;
					} while (std::filesystem::exists(finalPath));
					std::filesystem::create_directory(finalPath);
					ItemAddedEvent.Invoke(finalPath);
					Close();
				};
			BrowserItemContextualMenu::CreateList();
		}
	}
	virtual void DeleteItem() override
	{
		using namespace Windowing::Dialogs;
		MessageBox message("Delete folder", "Deleting a folder (and all its content) is irreversible, are you sure that you want to delete \"" + filePath + "\"?", MessageBox::EMessageType::WARNING, MessageBox::EButtonLayout::YES_NO);
		if (message.GetUserAction() == MessageBox::EUserAction::YES)
		{
			if (std::filesystem::exists(filePath) == true)
			{
				std::filesystem::remove_all(filePath);
				DestroyedEvent.Invoke(filePath);
			}
		}
	}

public:
	Tools::Eventing::Event<std::string> ItemAddedEvent;
};
class FileContextualMenu : public BrowserItemContextualMenu
{
public:
	FileContextualMenu(const std::string& p_filePath, bool p_protected = false) : BrowserItemContextualMenu(p_filePath, p_protected) {}

	virtual void CreateList() override
	{
		auto& editAction = CreateWidget<UI::Widgets::Menu::MenuItem>("Open");

		editAction.ClickedEvent += [this]
			{
				Tools::Utils::SystemCalls::OpenFile(filePath);
			};
		if (!m_protected)
		{
			auto& duplicateAction = CreateWidget<UI::Widgets::Menu::MenuItem>("Duplicate");
			duplicateAction.ClickedEvent += [this]
				{
					std::string filePathWithoutExtension = filePath;
					if (size_t pos = filePathWithoutExtension.rfind('.'); pos != std::string::npos)
						filePathWithoutExtension = filePathWithoutExtension.substr(0, pos);
					std::string extension = "." + Tools::Utils::PathParser::GetExtension(filePath);
					auto filenameAvailable = [&extension](const std::string& target)
						{
							return !std::filesystem::exists(target + extension);
						};
					const auto newNameWithoutExtension = Tools::Utils::String::GenerateUnique(filePathWithoutExtension, filenameAvailable);
					std::string finalPath = newNameWithoutExtension + extension;
					std::filesystem::copy(filePath, finalPath);
					DuplicateEvent.Invoke(finalPath);
				};
		}
		BrowserItemContextualMenu::CreateList();
	}
	virtual void DeleteItem() override
	{
		using namespace Windowing::Dialogs;
		MessageBox message("Delete file", "Deleting a file is irreversible, are you sure that you want to delete \"" + filePath + "\"?", MessageBox::EMessageType::WARNING, MessageBox::EButtonLayout::YES_NO);

		if (message.GetUserAction() == MessageBox::EUserAction::YES)
		{
			std::filesystem::remove(filePath);
			DestroyedEvent.Invoke(filePath);

		}
	}

public:
	Tools::Eventing::Event<std::string> DuplicateEvent;
};
namespace Geomerty {
	AssetBrowser::AssetBrowser
	(
		const std::string& p_title,
		bool p_opened,
		const UI::Settings::PanelWindowSettings& p_windowSettings,
		const std::string& p_projectAssetFolder
	):
		PanelWindow(p_title, p_opened, p_windowSettings),
		m_projectAssetFolder(p_projectAssetFolder)
	{
		if (!std::filesystem::exists(m_projectAssetFolder))
		{
			std::filesystem::create_directories(m_projectAssetFolder);

			Windowing::Dialogs::MessageBox message
			(
				"Assets folder not found",
				"The \"Assets/\" folders hasn't been found in your project directory.\nIt has been automatically generated",
				Windowing::Dialogs::MessageBox::EMessageType::WARNING,
				Windowing::Dialogs::MessageBox::EButtonLayout::OK
			);
		}
		auto& refreshButton = CreateWidget<UI::Widgets::Buttons::Button>("Rescan assets");
		refreshButton.ClickedEvent += std::bind(&AssetBrowser::Refresh, this);
		refreshButton.lineBreak = false;
		refreshButton.idleBackgroundColor = { 0.f, 0.5f, 0.0f };
		auto& importButton = CreateWidget<UI::Widgets::Buttons::Button>("Import asset");
		importButton.ClickedEvent += [&]() {
			ImportAssetAtLocation(m_projectAssetFolder);
			};
		importButton.idleBackgroundColor = { 0.7f, 0.5f, 0.0f };
		m_assetList = &CreateWidget<UI::Widgets::Layout::Group>();
		Fill();
	}
	void AssetBrowser::Fill()
	{
		m_assetList->CreateWidget<UI::Widgets::Visual::Separator>();
		ConsiderItem(nullptr, std::filesystem::directory_entry(m_projectAssetFolder), false);
	}
	void AssetBrowser::Clear()
	{
		m_assetList->RemoveAllWidgets();
	}

	void AssetBrowser::Refresh()
	{
		Clear();
		Fill();
	}

	void AssetBrowser::ParseFolder(UI::Widgets::Layout::TreeNode& p_root, const std::filesystem::directory_entry& p_directory, bool p_isEngineItem, bool p_scriptFolder)
	{
		for (auto& item : std::filesystem::directory_iterator(p_directory))
			if (item.is_directory())
				ConsiderItem(&p_root, item, p_isEngineItem, false, p_scriptFolder);
		for (auto& item : std::filesystem::directory_iterator(p_directory))
			if (!item.is_directory())
				ConsiderItem(&p_root, item, p_isEngineItem, false, p_scriptFolder);
	}
	void AssetBrowser::ConsiderItem(UI::Widgets::Layout::TreeNode* p_root, const std::filesystem::directory_entry& p_entry, bool p_isEngineItem, bool p_autoOpen, bool p_scriptFolder)
	{
		bool isDirectory = p_entry.is_directory();
		std::string itemname = Tools::Utils::PathParser::GetElementName(p_entry.path().string());
		std::string path = p_entry.path().string();
		if (isDirectory && path.back() != '\\') // Add '\\' if is directory and backslash is missing
			path += '\\';
		std::string resourceFormatPath = path;
		bool protectedItem = !p_root || p_isEngineItem;
		Tools::Utils::PathParser::EFileType fileType = Tools::Utils::PathParser::GetFileType(itemname);
		if (fileType == Tools::Utils::PathParser::EFileType::UNKNOWN && !isDirectory)
		{
			return;
		}
		auto& itemGroup = p_root ? p_root->CreateWidget<UI::Widgets::Layout::Group>() : m_assetList->CreateWidget<UI::Widgets::Layout::Group>();
		auto tid=CreateFromMemory();
		uint32_t iconTextureID = isDirectory ? tid.first : tid.second;
		itemGroup.CreateWidget<UI::Widgets::Visual::Image>(iconTextureID, Maths::FVector2{ 16, 16 }).lineBreak = false;
		if (isDirectory)
		{
			auto& treeNode = itemGroup.CreateWidget<UI::Widgets::Layout::TreeNode>(itemname);

			if (p_autoOpen)
				treeNode.Open();

			auto& ddSource = treeNode.AddPlugin<UI::Plugins::DDSource<std::pair<std::string, UI::Widgets::Layout::Group*>>>("Folder", resourceFormatPath, std::make_pair(resourceFormatPath, &itemGroup));

			if (!p_root || p_scriptFolder)
				treeNode.RemoveAllPlugins();

			auto& contextMenu = treeNode.AddPlugin<FolderContextualMenu>(path, protectedItem && resourceFormatPath != "") ;
			contextMenu.userData = static_cast<void*>(&treeNode);

			contextMenu.ItemAddedEvent += [this, &treeNode, path, p_isEngineItem, p_scriptFolder](std::string p_string)
				{
					treeNode.Open();
					treeNode.RemoveAllWidgets();
					ParseFolder(treeNode, std::filesystem::directory_entry(Tools::Utils::PathParser::GetContainingFolder(p_string)), p_isEngineItem, p_scriptFolder);
				};


	
			treeNode.AddPlugin<UI::Plugins::DDTarget<std::pair<std::string, UI::Widgets::Layout::Group*>>>("Folder").DataReceivedEvent += [this, &treeNode, path, p_isEngineItem](std::pair<std::string, UI::Widgets::Layout::Group*> p_data)
			{
				if (!p_data.first.empty())
				{
					std::string folderReceivedPath = m_projectAssetFolder+p_data.first;

					std::string folderName = Tools::Utils::PathParser::GetElementName(folderReceivedPath) + '\\';
					std::string prevPath = folderReceivedPath;
					std::string correctPath = m_pathUpdate.find(&treeNode) != m_pathUpdate.end() ? m_pathUpdate.at(&treeNode) : path;
					std::string newPath = correctPath + folderName;

					if (!(newPath.find(prevPath) != std::string::npos) || prevPath == newPath)
					{
						if (!std::filesystem::exists(newPath))
						{
						    RenameAsset(prevPath, newPath);
							treeNode.Open();
							treeNode.RemoveAllWidgets();
							ParseFolder(treeNode, std::filesystem::directory_entry(correctPath), p_isEngineItem);
						    p_data.second->Destroy();
						}
						else if (prevPath == newPath)
						{
							// Ignore
						}
						else
						{
							using namespace Windowing::Dialogs;

							MessageBox errorMessage("Folder already exists", "You can't move this folder to this location because the name is already taken", MessageBox::EMessageType::ERROR, MessageBox::EButtonLayout::OK);
						}
					}
					else
					{
						using namespace Windowing::Dialogs;

						MessageBox errorMessage("Wow!", "Crazy boy!", MessageBox::EMessageType::ERROR, MessageBox::EButtonLayout::OK);
					}
				}
			};

			treeNode.AddPlugin<UI::Plugins::DDTarget<std::pair<std::string, UI::Widgets::Layout::Group*>>>("File").DataReceivedEvent += [this, &treeNode, path, p_isEngineItem](std::pair<std::string, UI::Widgets::Layout::Group*> p_data)
						{
							if (!p_data.first.empty())
							{
								std::string fileReceivedPath =p_data.first;

								std::string fileName = Tools::Utils::PathParser::GetElementName(fileReceivedPath);
								std::string prevPath = fileReceivedPath;
								std::string correctPath = m_pathUpdate.find(&treeNode) != m_pathUpdate.end() ? m_pathUpdate.at(&treeNode) : path;
								std::string newPath = correctPath + fileName;

								if (!std::filesystem::exists(newPath))
								{
									bool isEngineFile = p_data.first.at(0) == ':';
			                        RenameAsset(prevPath, newPath);
									treeNode.Open();
									treeNode.RemoveAllWidgets();
									ParseFolder(treeNode, std::filesystem::directory_entry(correctPath), p_isEngineItem);
									p_data.second->Destroy();
								}
								else if (prevPath == newPath)
								{
									// Ignore
								}
								else
								{
									using namespace Windowing::Dialogs;

									MessageBox errorMessage("File already exists", "You can't move this file to this location because the name is already taken", MessageBox::EMessageType::ERROR, MessageBox::EButtonLayout::OK);
								}
							}
						};
				

				contextMenu.DestroyedEvent += [&itemGroup](std::string p_deletedPath) { itemGroup.Destroy(); };

				contextMenu.RenamedEvent += [this, &treeNode, path, &ddSource, p_isEngineItem](std::string p_prev, std::string p_newPath)
					{
						p_newPath += '\\';

						if (!std::filesystem::exists(p_newPath)) // Do not rename a folder if it already exists
						{
							RenameAsset(p_prev, p_newPath);
		
							std::string elementName = Tools::Utils::PathParser::GetElementName(p_newPath);
							std::string data = Tools::Utils::PathParser::GetContainingFolder(ddSource.data.first) + elementName + "\\";
							ddSource.data.first = data;
							ddSource.tooltip = data;
							treeNode.name = elementName;
							treeNode.Open();
							treeNode.RemoveAllWidgets();
							ParseFolder(treeNode, std::filesystem::directory_entry(p_newPath), p_isEngineItem);
							m_pathUpdate[&treeNode] = p_newPath;
						}
						else
						{
							using namespace Windowing::Dialogs;

							MessageBox errorMessage("Folder already exists", "You can't rename this folder because the given name is already taken", MessageBox::EMessageType::ERROR, MessageBox::EButtonLayout::OK);
						}
					};

				contextMenu.ItemAddedEvent += [this, &treeNode, p_isEngineItem](std::string p_path)
					{
						treeNode.RemoveAllWidgets();
						ParseFolder(treeNode, std::filesystem::directory_entry(Tools::Utils::PathParser::GetContainingFolder(p_path)), p_isEngineItem);
					};

			

			contextMenu.CreateList();

			treeNode.OpenedEvent += [this, &treeNode, path, p_isEngineItem, p_scriptFolder]
				{
					treeNode.RemoveAllWidgets();
					std::string updatedPath = Tools::Utils::PathParser::GetContainingFolder(path) + treeNode.name;
					ParseFolder(treeNode, std::filesystem::directory_entry(updatedPath), p_isEngineItem, p_scriptFolder);
				};

			treeNode.ClosedEvent += [this, &treeNode]
				{
					treeNode.RemoveAllWidgets();
				};
		}
		else
		{
			auto& clickableText = itemGroup.CreateWidget<UI::Widgets::Texts::TextClickable>(itemname);

			FileContextualMenu* contextMenu = nullptr;

			switch (fileType)
			{
				//case Tools::Utils::PathParser::EFileType::MODEL: contextMenu = &clickableText.AddPlugin<ModelContextualMenu>(path, protectedItem);break;
				default: contextMenu = &clickableText.AddPlugin<FileContextualMenu>(path, protectedItem); break;
			}
			contextMenu->CreateList();
			contextMenu->DestroyedEvent += [&itemGroup](std::string p_deletedPath)
				{
					itemGroup.Destroy();
				};
			auto& ddSource = clickableText.AddPlugin<UI::Plugins::DDSource<std::pair<std::string, UI::Widgets::Layout::Group*>>>
				(
					"File",
					resourceFormatPath,
					std::make_pair(resourceFormatPath, &itemGroup)
				);
			contextMenu->RenamedEvent += [&ddSource, &clickableText, p_scriptFolder](std::string p_prev, std::string p_newPath)
				{
					if (p_newPath != p_prev)
					{
						if (!std::filesystem::exists(p_newPath))
						{
							RenameAsset(p_prev, p_newPath);
							std::string elementName = Tools::Utils::PathParser::GetElementName(p_newPath);
							ddSource.data.first = Tools::Utils::PathParser::GetContainingFolder(ddSource.data.first) + elementName;
							clickableText.content = elementName;
						}
						else
						{
							using namespace Windowing::Dialogs;
							MessageBox errorMessage("File already exists", "You can't rename this file because the given name is already taken", MessageBox::EMessageType::ERROR, MessageBox::EButtonLayout::OK);
						}
					}
				};
		    contextMenu->DuplicateEvent += [this, &clickableText, p_root, path, p_isEngineItem](std::string newItem)
				{
					ConsiderItem(p_root, std::filesystem::directory_entry{ newItem }, p_isEngineItem, false, false);
				};
		}
	}

}