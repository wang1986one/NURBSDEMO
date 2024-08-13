#pragma once
#include <any>
#include <iostream>
#include<vector>
#include<string>
#include<unordered_map>
#include<filesystem>
#include<type_traits>
#include <imgui_node_editor.h>
#include"surface_mesh.h"
#include"read_mesh.h"
namespace Geomerty {
	struct Graph;
	static int uniqueId = 1;
	int GetNextId() {
		return uniqueId++;
	}
	struct NodeData {
		void* m_data=nullptr;
		uint64_t m_type;
		template<typename T>
		T* Get() {
			if (typeid(T).hash_code() == m_type && m_data) {
				return static_cast<T*>(m_data);
			}
			return nullptr;
		}
		template<typename T>
		void Set(void* ptr) {
			if (typeid(T).hash_code() == m_type) {
				if (m_data&& ptr!=m_data) {
					delete m_data;
					m_data = nullptr;
				}
				m_data = ptr;
				return;
			}
			throw std::runtime_error("type error");
		}
	};
	
	class Node;
	struct ExetContex {
		std::vector<NodeData*> inputs;
	
		
	};
	class NodeBase {
	public:
		NodeBase() = default;
		virtual void InstallUi() = 0;
		virtual void Init(std::unordered_map<size_t, Geomerty::NodeData>&registry) = 0;
		virtual void Execute(ExetContex* ctx, std::unordered_map<size_t, Geomerty::NodeData>& registry) = 0;
	};

	using PinType = size_t;
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
		size_t index;
		Pin(int id, const char* name, PinType type, PinKind k= PinKind::Input) :
			ID(id), Node(nullptr), Name(name), Type(type), Kind(k),index(id)
		{
		}

	};
	
	struct Node:public NodeBase
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
	   

		Node() = default;	
		Node(int id, const char* name, ImColor color = ImColor(255, 255, 255)) :
			ID(id), Name(name), Color(color), Type(NodeType::Blueprint), Size(0, 0)
		{
		}

		void InstallUi()override {

		}
		void Init(std::unordered_map<size_t, Geomerty::NodeData>& registry)override {

		}
		void Execute(ExetContex* ctx, std::unordered_map<size_t, Geomerty::NodeData>& registry)override {


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

}
