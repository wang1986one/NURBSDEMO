#pragma once
#include <any>
#include <iostream>
#include<vector>
#include<string>
#include<type_traits>
#include <imgui_node_editor.h>
#include"surface_mesh.h"
namespace Geomerty {
	static int uniqueId = 0;
	struct NodeData {
		NodeData() : m_type(0) {};
		explicit NodeData(uint64_t type) : m_type(type) {}
		NodeData(const NodeData& other) = default;
		NodeData(NodeData&& other) noexcept : m_data(std::move(other.m_data)), m_type(other.m_type) {}
		template<class T> explicit NodeData(const T& data) : m_type(typeid(T).hash_code()) { set(data); }

		NodeData& operator=(const NodeData& other)
		{
			m_type = other.m_type;
			m_data = other.m_data;
			return *this;
		}

		NodeData& operator=(NodeData&& other) noexcept
		{
			m_type = other.m_type;
			m_data = std::move(other.m_data);
			return *this;
		}

		template<typename T> NodeData& operator=(const T& data)
		{
			set(data);
			return *this;
		}

		template<typename T> NodeData& operator=(T&& data)
		{
			set(std::forward<T>(data));
			return *this;
		}

		template<typename T> bool set(const T& data)
		{
			if (is_type<T>())
			{
				m_data = data;
				return true;
			}
			std::cout << "[set node data] error data type" << std::endl;
			return false;
		};

		template<typename T> bool set(T&& data)
		{
			if (is_type<std::remove_reference_t<T>>())
			{
				m_data = std::forward<T>(data);
				return true;
			}
			std::cout << "[set node data] error data type" << std::endl;
			return false;
		};

		template<typename T> T get(bool* ok = nullptr) const
		{
			if (auto data_ptr = ptr<T>())
			{
				if (ok != nullptr)
				{
					*ok = true;
				}
				return *data_ptr;
			}
			if (ok != nullptr)
			{
				*ok = false;
			}
			else
			{
				std::cout << "[get node data] error data type]" << std::endl;;
			}
			return T();
		};

		template<typename T> T* ptr()
		{
			if (is_type<T>() && m_data.has_value())
			{
				return std::any_cast<T>(&m_data);
			}
			return nullptr;
		};

		template<typename T> const T* ptr() const
		{
			if (is_type<T>() && m_data.has_value())
			{
				return std::any_cast<T>(&m_data);
			}
			return nullptr;
		};

		template<typename T> bool is_type() const
		{
			auto type = typeid(T).hash_code();
			return m_type == type || m_type == 0;
		}

		uint64_t type() const { return m_type; }
		const std::any& data() const { return m_data; }
		bool has_value() const { return m_data.has_value(); }
		void reset() { m_data.reset(); }
		void reset(uint64_t type)
		{
			m_data.reset();
			m_type = type;
		}
	private:
		std::any m_data;
		uint64_t m_type;
	};
	class GeomertyNodeBase;
	class Node;
	struct ExetContex {
		GeomertyNodeBase* node;
		std::vector<NodeData*> inputs;
		std::vector<NodeData>& outputs;
	};
	class GeomertyNodeBase {
	public:
		GeomertyNodeBase() = default;
		virtual void InstallUi() = 0;
		virtual void Init() = 0;
		virtual void Execute(ExetContex* ctx) = 0;
	};
	class GeomertyNode :public GeomertyNodeBase {
	public:
		GeomertyNode() = default;
		std::vector<NodeData>output_data;
		void InstallUi()override {
			ax::NodeEditor::BeginPin(uniqueId++, ax::NodeEditor::PinKind::Input);
			ImGui::Text("-> In");
			ax::NodeEditor::EndPin();
			ImGui::SameLine();
			ImGui::Dummy(ImVec2(250, 0)); // Hacky magic number to space out the output pin.
			ImGui::SameLine();
			ax::NodeEditor::BeginPin(uniqueId++, ax::NodeEditor::PinKind::Output);
			ImGui::Text("Out ->");
			ax::NodeEditor::EndPin();
		}
		void Init()override {

		}
		void Execute(ExetContex* ctx)override {


		}

	};
	using PinType = uint8_t;
	enum class PinKind
	{
		Output,
		Input
	};
	enum class NodeType
	{
		Blueprint,
		Simple,
		Tree,
		Comment,
		Houdini
	};
	struct Pin
	{
		ax::NodeEditor::PinId   ID;
		Node* Node;
		std::string Name;
		PinType Type;
		PinKind Kind;
		Pin(int id, const char* name, PinType type) :
			ID(id), Node(nullptr), Name(name), Type(type), Kind(PinKind::Input)
		{
		}
	};
	struct Node
	{
		ax::NodeEditor::NodeId ID;
		std::string Name;
		std::vector<Pin> Inputs;
		std::vector<Pin> Outputs;
		ImColor Color;
		NodeType Type;
		ImVec2 Size;
		std::string State;
		std::string SavedState;
		GeomertyNodeBase* Gnode = nullptr;
		Node(int id, const char* name, ImColor color = ImColor(255, 255, 255)) :
			ID(id), Name(name), Color(color), Type(NodeType::Blueprint), Size(0, 0)
		{
		}
		void Exectue() {

		}
	};
	struct Link
	{
		ax::NodeEditor::LinkId ID;
		ax::NodeEditor::PinId StartPinID;
		ax::NodeEditor::PinId EndPinID;
		ImColor Color;
		Link(ax::NodeEditor::LinkId id, ax::NodeEditor::PinId startPinId, ax::NodeEditor::PinId endPinId) :
			ID(id), StartPinID(startPinId), EndPinID(endPinId), Color(255, 255, 255)
		{
		}
	};
	struct NodeIdLess
	{
		bool operator()(const ax::NodeEditor::NodeId& lhs, const ax::NodeEditor::NodeId& rhs) const
		{
			return lhs.AsPointer() < rhs.AsPointer();
		}
	};
	class StringNode :public GeomertyNode {
	public:
		void InstallUi() override {

			ImGui::Text("Path");
		}
		void Init()override {
			//outputs.push_back(std::make_shared<std::any>(str));
		}
		void Execute(ExetContex* ctx)override {

			//td::string path=std::any_cast<std::string>((*ctx->inputs)[0]);
		}
		StringNode() {
		}
		std::string str;
	};
	class Read_MeshNode :public GeomertyNode {
		void InstallUi() override {


		}
		void Init() override {

		}
		void Execute(ExetContex* ctx)override {

			//td::string path=std::any_cast<std::string>((*ctx->inputs)[0]);
		}

	};

}
