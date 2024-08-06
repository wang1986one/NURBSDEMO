#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include <imgui_internal.h>
#include <imgui_node_editor.h>
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "ImGuizmo.h"
#include "nodes.h"
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace ed = ax::NodeEditor;
# ifdef _MSC_VER
# define portable_strcpy    strcpy_s
# define portable_sprintf   sprintf_s
# else
# define portable_strcpy    strcpy
# define portable_sprintf   sprintf
# endif

#pragma region  igl

static double highdpiw = 1; // High DPI width
static double highdpih = 1; // High DPI height
static double scroll_x = 0;
static double scroll_y = 0;

static void glfw_mouse_press(GLFWwindow* /*window*/, int button, int action, int modifier)
{

}

static void glfw_error_callback(int /*error*/, const char* description)
{
	fputs(description, stderr);
}

static void glfw_char_mods_callback(GLFWwindow* /*window*/, unsigned int codepoint, int modifier)
{

}

static void glfw_key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int modifier)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

static void glfw_window_size(GLFWwindow* /*window*/, int width, int height)
{

}

static void glfw_mouse_move(GLFWwindow* /*window*/, double x, double y)
{

}

static void glfw_mouse_scroll(GLFWwindow* /*window*/, double x, double y)
{
	using namespace std;
	scroll_x += x;
	scroll_y += y;
}

static void glfw_drop_callback(GLFWwindow* /*window*/, int /*count*/, const char** /*filenames*/)
{
}
#pragma endregion
#include  <math.h>
#include<vector>
#define SCR_WIDTH 800
#define SCR_HEIGHT 600
void g_mainWindow(GLFWwindow* window, int w, int h) {
	glfw_window_size(window, w, h);
}
void Scorllcallback(GLFWwindow* window, double dx, double dy) {
	glfw_mouse_scroll(window, dx, dy);
}
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	glfw_key_callback(window, key, scancode, action, mods);
}
void mouseButton(GLFWwindow* window, int B, int S, int F) {
	glfw_mouse_press(window, B, S, F);
}
void mouseMove(GLFWwindow* window, double x, double y) {
	glfw_mouse_move(window, x, y);
}
static ed::EditorContext* m_Editor = nullptr;
struct Example {
	
	int m_NextId = 1;
	std::vector<Geomerty::Node*>    m_Nodes;
	std::vector<Geomerty::Link>    m_Links;
	int GetNextId()
	{
		return m_NextId++;
	}
	ed::LinkId GetNextLinkId()
	{
		return ed::LinkId(GetNextId());
	}
	Geomerty::Node* FindNode(ed::NodeId id)
	{
		for (auto& node : m_Nodes)
			if (node->ID == id)
				return node;
		return nullptr;
	}
	Geomerty::Link* FindLink(ed::LinkId id)
	{
		for (auto& link : m_Links)
			if (link.ID == id)
				return &link;
		return nullptr;
	}
	Geomerty::Pin* FindPin(ed::PinId id)
	{
		if (!id)
			return nullptr;

		for (auto& node : m_Nodes)
		{
			for (auto& pin : node->Inputs)
				if (pin.ID == id)
					return &pin;

			for (auto& pin : node->Outputs)
				if (pin.ID == id)
					return &pin;
		}

		return nullptr;
	}
	bool IsPinLinked(ed::PinId id)
	{
		if (!id)
			return false;

		for (auto& link : m_Links)
			if (link.StartPinID == id || link.EndPinID == id)
				return true;

		return false;
	}

	bool CanCreateLink(Geomerty::Pin* a, Geomerty::Pin* b)
	{
		if (!a || !b || a == b || a->Kind == b->Kind || a->Type != b->Type || a->Node == b->Node)
			return false;

		return true;
	}
	void BuildNode(Geomerty::Node* node)
	{
		for (auto& input : node->Inputs)
		{
			input.Node = node;
			input.Kind = Geomerty::PinKind::Input;
		}

		for (auto& output : node->Outputs)
		{
			output.Node = node;
			output.Kind = Geomerty::PinKind::Output;
		}
	}
	Geomerty::Node* SpawnStringNode()
	{
		m_Nodes.push_back(new Geomerty::StringNode(GetNextId(), "StringNode", ImColor(255, 255, 128)));
		m_Nodes.back()->Init();
		return m_Nodes.back();
	}
	Geomerty::Node* SpawnRead_MeshNode()
	{
		m_Nodes.push_back(new Geomerty::Read_MeshNode(GetNextId(), "Read_MeshNode"));
		m_Nodes.back()->Init();
		return m_Nodes.back();
	}
	void OnStart() {
		ed::Config config;

		config.SettingsFile = "Blueprints.json";
		m_Editor = ed::CreateEditor(&config);
		ed::SetCurrentEditor(m_Editor);
		Geomerty::Node* node;
		node = SpawnStringNode(); ed::SetNodePosition(node->ID, ImVec2(-252, 220));
		node = SpawnRead_MeshNode(); ed::SetNodePosition(node->ID, ImVec2(-300, 351));
		ed::NavigateToContent();
	}
	void OnStop()
	{
		if (m_Editor)
		{
			ed::DestroyEditor(m_Editor);
			m_Editor = nullptr;
		}
	}
	void OnFrame(float deltaTime)
	{
		ed::SetCurrentEditor(m_Editor);
		static ed::NodeId contextNodeId = 0;
		static ed::LinkId contextLinkId = 0;
		static ed::PinId  contextPinId = 0;
		static bool createNewNode = false;
		static Geomerty::Pin* newNodeLinkPin = nullptr;
		static Geomerty::Pin* newLinkPin = nullptr;
		ed::Begin("Node editor");
		//blueprint and siample
		for (auto& node : m_Nodes)
		{
			ed::BeginNode(node->ID);
			for (auto& input : node->Inputs)
			{
				auto alpha = ImGui::GetStyle().Alpha;
				if (newLinkPin && !CanCreateLink(newLinkPin, &input) && &input != newLinkPin)
					alpha = alpha * (48.0f / 255.0f);
				ed::BeginPin(input.ID, ax::NodeEditor::PinKind::Input);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
			
				//ImGui::Spring(0);
				if (!input.Name.empty())
				{
					ImGui::TextUnformatted(input.Name.c_str());
					//ImGui::Spring(0);
				}
				if (input.Type ==typeid(bool).hash_code())
				{
					ImGui::Button("Hello");
					//ImGui::Spring(0);
				}
				ImGui::PopStyleVar();
				ed::EndPin();
			}
			for (auto& output : node->Outputs)
			{
				auto alpha = ImGui::GetStyle().Alpha;
				if (newLinkPin && !CanCreateLink(newLinkPin, &output) && &output != newLinkPin)
					alpha = alpha * (48.0f / 255.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
				ed::BeginPin(output.ID, ax::NodeEditor::PinKind::Output);
				if (output.Type == typeid(std::string).hash_code())
				{
					static char buffer[128] = "Edit Me\nMultiline!";
					static bool wasActive = false;

					ImGui::PushItemWidth(100.0f);
					ImGui::InputText("##edit", buffer, 127);
					ImGui::PopItemWidth();
					if (ImGui::IsItemActive() && !wasActive)
					{
						ed::EnableShortcuts(false);
						wasActive = true;
					}
					else if (!ImGui::IsItemActive() && wasActive)
					{
						ed::EnableShortcuts(true);
						wasActive = false;
					}
					//ImGui::Spring(0);
				}
				if (!output.Name.empty())
				{
					// ImGui::Spring(0);
					ImGui::TextUnformatted(output.Name.c_str());
				}
				ImGui::PopStyleVar();
				ed::EndPin();
			}
			ed::EndNode();
		}
		ed::End();
	}
};
int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "NURBS test", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glfwSetWindowSizeCallback(window, g_mainWindow);
	glfwSetScrollCallback(window, Scorllcallback);
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetMouseButtonCallback(window, mouseButton);
	glfwSetCursorPosCallback(window, mouseMove);
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	glEnable(GL_DEPTH_TEST);

	Example ex;
	ex.OnStart();
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		if (ImGui::Begin("widgets", nullptr, ImGuiWindowFlags_None)) {
			ex.OnFrame(0.01f);
		}
		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();
}

