#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include <imgui_internal.h>
#include <imgui_node_editor.h>

# include <imgui_canvas.h>
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "ImGuizmo.h"
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

struct LinkInfo
{
    ed::LinkId Id;
    ed::PinId  InputId;
    ed::PinId  OutputId;
};
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
    int w = width * highdpiw;
    int h = height * highdpih;
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
    glfw_window_size(window,  w,  h);
}
void Scorllcallback(GLFWwindow* window, double dx, double dy) {
    glfw_mouse_scroll(window,  dx,  dy);
}
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

    glfw_key_callback(window, key,  scancode,  action,  mods);
}
void mouseButton(GLFWwindow* window, int B, int S, int F) {
    

    glfw_mouse_press(window,  B,  S,  F);
}
void mouseMove(GLFWwindow* window, double x, double y) {
    glfw_mouse_move(window,  x,  y);
}

static void DrawScale(const ImVec2& from, const ImVec2& to, float majorUnit, float minorUnit, float labelAlignment, float sign = 1.0f)
{
    auto drawList = ImGui::GetWindowDrawList();
    auto direction = (to - from) * ImInvLength(to - from, 0.0f);
    auto normal = ImVec2(-direction.y, direction.x);
    auto distance = sqrtf(ImLengthSqr(to - from));

    if (ImDot(direction, direction) < FLT_EPSILON)
        return;

    auto minorSize = 5.0f;
    auto majorSize = 10.0f;
    auto labelDistance = 8.0f;

    drawList->AddLine(from, to, IM_COL32(255, 255, 255, 255));

    auto p = from;
    for (auto d = 0.0f; d <= distance; d += minorUnit, p += direction * minorUnit)
        drawList->AddLine(p - normal * minorSize, p + normal * minorSize, IM_COL32(255, 255, 255, 255));

    for (auto d = 0.0f; d <= distance + majorUnit; d += majorUnit)
    {
        p = from + direction * d;

        drawList->AddLine(p - normal * majorSize, p + normal * majorSize, IM_COL32(255, 255, 255, 255));

        if (d == 0.0f)
            continue;

        char label[16];
        snprintf(label, 15, "%g", d * sign);
        auto labelSize = ImGui::CalcTextSize(label);

        auto labelPosition = p + ImVec2(fabsf(normal.x), fabsf(normal.y)) * labelDistance;
        auto labelAlignedSize = ImDot(labelSize, direction);
        labelPosition += direction * (-labelAlignedSize + labelAlignment * labelAlignedSize * 2.0f);
        labelPosition = ImFloor(labelPosition + ImVec2(0.5f, 0.5f));

        drawList->AddText(labelPosition, IM_COL32(255, 255, 255, 255), label);
    }
}

static bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f)
{
    using namespace ImGui;
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    ImGuiID id = window->GetID("##Splitter");
    ImRect bb;
    bb.Min = window->DC.CursorPos + (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
    bb.Max = bb.Min + CalcItemSize(split_vertically ? ImVec2(thickness, splitter_long_axis_size) : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f);
    return SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, min_size1, min_size2, 0.0f);
}

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

    //load fonts
    ImGuiIO& io = ImGui::GetIO();
    IM_DELETE(io.Fonts);
    io.Fonts = IM_NEW(ImFontAtlas);
    ImFontConfig config;
    config.OversampleH = 4;
    config.OversampleV = 4;
    config.PixelSnapH = false;
    io.Fonts->AddFontFromFileTTF("Play-Regular.ttf", 18.0f, &config);
    io.Fonts->AddFontFromFileTTF("Cuprum-Bold.ttf", 20.0f, &config);
    io.Fonts->Build();
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    glEnable(GL_DEPTH_TEST);

    auto m_Context_a= ax::NodeEditor::CreateEditor();
    auto ui_sample_call = [&]() { 
        static ImVector<LinkInfo>   m_Links;                // List of live links. It is dynamic unless you want to create read-only view over nodes.
        static int m_NextLinkId = 100;     // Counter to help generate link ids. In real application this will probably based on pointer to user data structure.

        ed::SetCurrentEditor(m_Context_a);
        ed::Begin("My Editor A", ImVec2(0.0, 0.0f));
        int uniqueId = 1;
        // Start drawing nodes.
        ed::BeginNode(uniqueId++);
        ImGui::Text("Node A");
        ed::BeginPin(uniqueId++, ed::PinKind::Input);
        ImGui::Text("->In");
        ed::EndPin();
        ImGui::SameLine();
        ed::BeginPin(uniqueId++, ed::PinKind::Output);
        ImGui::Text("Out->");
        ed::EndPin();
        ed::EndNode();
        ed::End();
        ed::SetCurrentEditor(nullptr);
        };

    auto m_Context_b = ax::NodeEditor::CreateEditor();
    auto ui_widgets_call = [&]() {
        static ImVector<LinkInfo>   m_Links;                // List of live links. It is dynamic unless you want to create read-only view over nodes.
        static int m_NextLinkId = 100;
        static bool firstframe = true; // Used to position the nodes on startup

        // Node Editor Widget
        ed::SetCurrentEditor(m_Context_b);
        ed::Begin("My Editor B", ImVec2(0.0, 0.0f));
        int uniqueId = 1;


        // Basic Widgets Demo  ==============================================================================================
        auto basic_id = uniqueId++;
        ed::BeginNode(basic_id);
        ImGui::Text("Basic Widget Demo");
        ed::BeginPin(uniqueId++, ed::PinKind::Input);
        ImGui::Text("-> In");
        ed::EndPin();
        ImGui::SameLine();
        ImGui::Dummy(ImVec2(250, 0)); // Hacky magic number to space out the output pin.
        ImGui::SameLine();
        ed::BeginPin(uniqueId++, ed::PinKind::Output);
        ImGui::Text("Out ->");
        ed::EndPin();

        // Widget Demo from imgui_demo.cpp...
        // Normal Button
        static int clicked = 0;
        if (ImGui::Button("Button"))
            clicked++;
        // Checkbox
        static bool check = true;
        ImGui::Checkbox("checkbox", &check);

        // Radio buttons
        static int e = 0;
        ImGui::RadioButton("radio a", &e, 0); ImGui::SameLine();
        ImGui::RadioButton("radio b", &e, 1); ImGui::SameLine();
        ImGui::RadioButton("radio c", &e, 2);

        // Color buttons, demonstrate using PushID() to add unique identifier in the ID stack, and changing style.
        for (int i = 0; i < 7; i++)
        {
            if (i > 0)
                ImGui::SameLine();
            ImGui::PushID(i);
            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i / 7.0f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(i / 7.0f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(i / 7.0f, 0.8f, 0.8f));
            ImGui::Button("Click");
            ImGui::PopStyleColor(3);
            ImGui::PopID();
        }

        // Use AlignTextToFramePadding() to align text baseline to the baseline of framed elements (otherwise a Text+SameLine+Button sequence will have the text a little too high by default)
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Hold to repeat:");
        ImGui::SameLine();

        // Arrow buttons with Repeater
        static int counter = 0;
        float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
        ImGui::PushButtonRepeat(true);
        if (ImGui::ArrowButton("##left", ImGuiDir_Left)) { counter--; }
        ImGui::SameLine(0.0f, spacing);
        if (ImGui::ArrowButton("##right", ImGuiDir_Right)) { counter++; }
        ImGui::PopButtonRepeat();
        ImGui::SameLine();
        ImGui::Text("%d", counter);

        // The input widgets also require you to manually disable the editor shortcuts so the view doesn't fly around.
        // (note that this is a per-frame setting, so it disables it for all text boxes.  I left it here so you could find it!)
        ed::EnableShortcuts(!io.WantTextInput);
        // The input widgets require some guidance on their widths, or else they're very large. (note matching pop at the end).
        ImGui::PushItemWidth(200);
        static char str1[128] = "";
        ImGui::InputTextWithHint("input text (w/ hint)", "enter text here", str1, IM_ARRAYSIZE(str1));

        static float f0 = 0.001f;
        ImGui::InputFloat("input float", &f0, 0.01f, 1.0f, "%.3f");

        static float f1 = 1.00f, f2 = 0.0067f;
        ImGui::DragFloat("drag float", &f1, 0.005f);
        ImGui::DragFloat("drag small float", &f2, 0.0001f, 0.0f, 0.0f, "%.06f ns");
        ImGui::PopItemWidth();

        ed::EndNode();
        if (firstframe)
        {
            ed::SetNodePosition(basic_id, ImVec2(20, 20));
        }

        // Headers and Trees Demo =======================================================================================================
        // TreeNodes and Headers streatch to the entire remaining work area. To put them in nodes what we need to do is to tell
        // ImGui out work area is shorter. We can achieve that right now only by using columns API.
        //
        // Relevent bugs: https://github.com/thedmd/imgui-node-editor/issues/30
        auto header_id = uniqueId++;
        ed::BeginNode(header_id);
        ImGui::Text("Tree Widget Demo");

        // Pins Row
        ed::BeginPin(uniqueId++, ed::PinKind::Input);
        ImGui::Text("-> In");
        ed::EndPin();
        ImGui::SameLine();
        ImGui::Dummy(ImVec2(35, 0)); //  magic number - Crude & simple way to nudge over the output pin. Consider using layout and springs
        ImGui::SameLine();
        ed::BeginPin(uniqueId++, ed::PinKind::Output);
        ImGui::Text("Out ->");
        ed::EndPin();

        // Tree column startup -------------------------------------------------------------------
        // Push dummy widget to extend node size. Columns do not do that.
        float width = 135; // bad magic numbers. used to define width of tree widget
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
        ImGui::Dummy(ImVec2(width, 0));
        ImGui::PopStyleVar();

        // Start columns, but use only first one.
        
        ImGui::BeginColumns("##TreeColumns", 2,
            ImGuiOldColumnFlags_NoBorder |
            ImGuiOldColumnFlags_NoResize |
            ImGuiOldColumnFlags_NoPreserveWidths |
            ImGuiOldColumnFlags_NoForceWithinWindow);

        // Adjust column width to match requested one.
        ImGui::SetColumnWidth(0, width
            + ImGui::GetStyle().WindowPadding.x
            + ImGui::GetStyle().ItemSpacing.x);
        // End of tree column startup --------------------------------------------------------------

        // Back to normal ImGui drawing, in our column.
        if (ImGui::CollapsingHeader("Open Header"))
        {
            ImGui::Text("Hello There");
            if (ImGui::TreeNode("Open Tree")) {
                static bool OP1_Bool = false;
                ImGui::Text("Checked: %s", OP1_Bool ? "true" : "false");
                ImGui::Checkbox("Option 1", &OP1_Bool);
                ImGui::TreePop();
            }
        }
        // Tree Column Shutdown
        ImGui::EndColumns();
        ed::EndNode(); // End of Tree Node Demo

        if (firstframe)
        {
            ed::SetNodePosition(header_id, ImVec2(420, 20));
        }

        // Tool Tip & Pop-up Demo =====================================================================================
        // Tooltips, combo-boxes, drop-down menus need to use a work-around to place the "overlay window" in the canvas.
        // To do this, we must defer the popup calls until after we're done drawing the node material.
        //
        // Relevent bugs:  https://github.com/thedmd/imgui-node-editor/issues/48
        auto popup_id = uniqueId++;
        ed::BeginNode(popup_id);
        ImGui::Text("Tool Tip & Pop-up Demo");
        ed::BeginPin(uniqueId++, ed::PinKind::Input);
        ImGui::Text("-> In");
        ed::EndPin();
        ImGui::SameLine();
        ImGui::Dummy(ImVec2(85, 0)); // Hacky magic number to space out the output pin.
        ImGui::SameLine();
        ed::BeginPin(uniqueId++, ed::PinKind::Output);
        ImGui::Text("Out ->");
        ed::EndPin();

        // Tooltip example
        ImGui::Text("Hover over me");
        static bool do_tooltip = false;
        do_tooltip = ImGui::IsItemHovered() ? true : false;
        ImGui::SameLine();
        ImGui::Text("- or me");
        static bool do_adv_tooltip = false;
        do_adv_tooltip = ImGui::IsItemHovered() ? true : false;

        // Use AlignTextToFramePadding() to align text baseline to the baseline of framed elements
        // (otherwise a Text+SameLine+Button sequence will have the text a little too high by default)
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Option:");
        ImGui::SameLine();
        static char popup_text[128] = "Pick one!";
        static bool do_popup = false;
        if (ImGui::Button(popup_text)) {
            do_popup = true;	// Instead of saying OpenPopup() here, we set this bool, which is used later in the Deferred Pop-up Section
        }
        ed::EndNode();
        if (firstframe) {
            ed::SetNodePosition(popup_id, ImVec2(610, 20));
        }

        // --------------------------------------------------------------------------------------------------
        // Deferred Pop-up Section

        // This entire section needs to be bounded by Suspend/Resume!  These calls pop us out of "node canvas coordinates"
        // and draw the popups in a reasonable screen location.
        ed::Suspend();
        // There is some stately stuff happening here.  You call "open popup" exactly once, and this
        // causes it to stick open for many frames until the user makes a selection in the popup, or clicks off to dismiss.
        // More importantly, this is done inside Suspend(), so it loads the popup with the correct screen coordinates!
        if (do_popup) {
            ImGui::OpenPopup("popup_button"); // Cause openpopup to stick open.
            do_popup = false; // disable bool so that if we click off the popup, it doesn't open the next frame.
        }

        // This is the actual popup Gui drawing section.
        if (ImGui::BeginPopup("popup_button")) {
            // Note: if it weren't for the child window, we would have to PushItemWidth() here to avoid a crash!
            ImGui::TextDisabled("Pick One:");
            ImGui::BeginChild("popup_scroller", ImVec2(100, 100), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
            if (ImGui::Button("Option 1")) {
                portable_strcpy(popup_text, "Option 1");
                ImGui::CloseCurrentPopup();  // These calls revoke the popup open state, which was set by OpenPopup above.
            }
            if (ImGui::Button("Option 2")) {
                portable_strcpy(popup_text, "Option 2");
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::Button("Option 3")) {
                portable_strcpy(popup_text, "Option 3");
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::Button("Option 4")) {
                portable_strcpy(popup_text, "Option 4");
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndChild();
            ImGui::EndPopup(); // Note this does not do anything to the popup open/close state. It just terminates the content declaration.
        }

        // Handle the simple tooltip
        if (do_tooltip)
            ImGui::SetTooltip("I am a tooltip");

        // Handle the advanced tooltip
        if (do_adv_tooltip) {
            ImGui::BeginTooltip();
            ImGui::Text("I am a fancy tooltip");
            static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
            ImGui::PlotLines("Curve", arr, IM_ARRAYSIZE(arr));
            ImGui::EndTooltip();
        }

        ed::Resume();
        // End of "Deferred Pop-up section"



        // Plot Widgets =========================================================================================
        // Note: most of these plots can't be used in nodes missing, because they spawn tooltips automatically,
        // so we can't trap them in our deferred pop-up mechanism.  This causes them to fly into a random screen
        // location.
        auto plot_id = uniqueId++;
        ed::BeginNode(plot_id);
        ImGui::Text("Plot Demo");
        ed::BeginPin(uniqueId++, ed::PinKind::Input);
        ImGui::Text("-> In");
        ed::EndPin();
        ImGui::SameLine();
        ImGui::Dummy(ImVec2(250, 0)); // Hacky magic number to space out the output pin.
        ImGui::SameLine();
        ed::BeginPin(uniqueId++, ed::PinKind::Output);
        ImGui::Text("Out ->");
        ed::EndPin();

        ImGui::PushItemWidth(300);

        // Animate a simple progress bar
        static float progress = 0.0f, progress_dir = 1.0f;
        progress += progress_dir * 0.4f * ImGui::GetIO().DeltaTime;
        if (progress >= +1.1f) { progress = +1.1f; progress_dir *= -1.0f; }
        if (progress <= -0.1f) { progress = -0.1f; progress_dir *= -1.0f; }


        // Typically we would use ImVec2(-1.0f,0.0f) or ImVec2(-FLT_MIN,0.0f) to use all available width,
        // or ImVec2(width,0.0f) for a specified width. ImVec2(0.0f,0.0f) uses ItemWidth.
        ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        ImGui::Text("Progress Bar");

        float progress_saturated = (progress < 0.0f) ? 0.0f : (progress > 1.0f) ? 1.0f : progress;
        char buf[32];
        portable_sprintf(buf, "%d/%d", (int)(progress_saturated * 1753), 1753);
        ImGui::ProgressBar(progress, ImVec2(0.f, 0.f), buf);

        ImGui::PopItemWidth();
        ed::EndNode();
        if (firstframe) {
            ed::SetNodePosition(plot_id, ImVec2(850, 20));
        }
        // ==================================================================================================
        // Link Drawing Section

        for (auto& linkInfo : m_Links)
            ed::Link(linkInfo.Id, linkInfo.InputId, linkInfo.OutputId);

        // ==================================================================================================
        // Interaction Handling Section
        // This was coppied from BasicInteration.cpp. See that file for commented code.

        // Handle creation action ---------------------------------------------------------------------------
        if (ed::BeginCreate())
        {
            ed::PinId inputPinId, outputPinId;
            if (ed::QueryNewLink(&inputPinId, &outputPinId))
            {
                if (inputPinId && outputPinId)
                {
                    if (ed::AcceptNewItem())
                    {
                        m_Links.push_back({ ed::LinkId(m_NextLinkId++), inputPinId, outputPinId });
                        ed::Link(m_Links.back().Id, m_Links.back().InputId, m_Links.back().OutputId);
                    }
                }
            }
        }
        ed::EndCreate();

        // Handle deletion action ---------------------------------------------------------------------------
        if (ed::BeginDelete())
        {
            ed::LinkId deletedLinkId;
            while (ed::QueryDeletedLink(&deletedLinkId))
            {
                if (ed::AcceptDeletedItem())
                {
                    for (auto& link : m_Links)
                    {
                        if (link.Id == deletedLinkId)
                        {
                            m_Links.erase(&link);
                            break;
                        }
                    }
                }
            }
        }
        ed::EndDelete();

        ed::End();
        ed::SetCurrentEditor(nullptr);
        firstframe = false;
        //ImGui::ShowMetricsWindow();
        //ImGui::ShowDemoWindow();

        };

    auto m_Context_c= ed::CreateEditor();
    auto ui_basicinteracton_call = [&]() {
        static ImVector<LinkInfo>   m_Links;                // List of live links. It is dynamic unless you want to create read-only view over nodes.
        static int m_NextLinkId = 100;     // Counter to help generate link ids. In real application this will probably based on pointer to user data structure.
        static bool m_FirstFrame = true;

        ed::SetCurrentEditor(m_Context_c);

        // Start interaction with editor.
        ed::Begin("My Editor C", ImVec2(0.0, 0.0f));

        int uniqueId = 1;

        //
        // 1) Commit known data to editor
        //

        // Submit Node A
        ed::NodeId nodeA_Id = uniqueId++;
        ed::PinId  nodeA_InputPinId = uniqueId++;
        ed::PinId  nodeA_OutputPinId = uniqueId++;

        if (m_FirstFrame)
            ed::SetNodePosition(nodeA_Id, ImVec2(10, 10));
        ed::BeginNode(nodeA_Id);
        ImGui::Text("Node A");
        ed::BeginPin(nodeA_InputPinId, ed::PinKind::Input);
        ImGui::Text("-> In");
        ed::EndPin();
        ImGui::SameLine();
        ed::BeginPin(nodeA_OutputPinId, ed::PinKind::Output);
        ImGui::Text("Out ->");
        ed::EndPin();
        ed::EndNode();

        // Submit Node B
        ed::NodeId nodeB_Id = uniqueId++;
        ed::PinId  nodeB_InputPinId1 = uniqueId++;
        ed::PinId  nodeB_InputPinId2 = uniqueId++;
        ed::PinId  nodeB_OutputPinId = uniqueId++;

        if (m_FirstFrame)
            ed::SetNodePosition(nodeB_Id, ImVec2(210, 60));
        ed::BeginNode(nodeB_Id);
        ImGui::Text("Node B");
        ImGui::BeginGroup();
        ed::BeginPin(nodeB_InputPinId1, ed::PinKind::Input);
        ImGui::Text("-> In1");
        ed::EndPin();
        ed::BeginPin(nodeB_InputPinId2, ed::PinKind::Input);
        ImGui::Text("-> In2");
        ed::EndPin();
        ImGui::EndGroup();
        ImGui::SameLine();
        ImGui::BeginGroup();
        ed::BeginPin(nodeB_OutputPinId, ed::PinKind::Output);
        ImGui::Text("Out ->");
        ed::EndPin();
        ImGui::EndGroup();
        ed::EndNode();

        // Submit Links
        for (auto& linkInfo : m_Links)
            ed::Link(linkInfo.Id, linkInfo.InputId, linkInfo.OutputId);

        //
        // 2) Handle interactions
        //

        // Handle creation action, returns true if editor want to create new object (node or link)
        if (ed::BeginCreate())
        {
            ed::PinId inputPinId, outputPinId;
            if (ed::QueryNewLink(&inputPinId, &outputPinId))
            {
                // QueryNewLink returns true if editor want to create new link between pins.
                //
                // Link can be created only for two valid pins, it is up to you to
                // validate if connection make sense. Editor is happy to make any.
                //
                // Link always goes from input to output. User may choose to drag
                // link from output pin or input pin. This determine which pin ids
                // are valid and which are not:
                //   * input valid, output invalid - user started to drag new ling from input pin
                //   * input invalid, output valid - user started to drag new ling from output pin
                //   * input valid, output valid   - user dragged link over other pin, can be validated

                if (inputPinId && outputPinId) // both are valid, let's accept link
                {
                    // ed::AcceptNewItem() return true when user release mouse button.
                    if (ed::AcceptNewItem())
                    {
                        // Since we accepted new link, lets add one to our list of links.
                        m_Links.push_back({ ed::LinkId(m_NextLinkId++), inputPinId, outputPinId });

                        // Draw new link.
                        ed::Link(m_Links.back().Id, m_Links.back().InputId, m_Links.back().OutputId);
                    }

                    // You may choose to reject connection between these nodes
                    // by calling ed::RejectNewItem(). This will allow editor to give
                    // visual feedback by changing link thickness and color.
                }
            }
        }
        ed::EndCreate(); // Wraps up object creation action handling.


        // Handle deletion action
        if (ed::BeginDelete())
        {
            // There may be many links marked for deletion, let's loop over them.
            ed::LinkId deletedLinkId;
            while (ed::QueryDeletedLink(&deletedLinkId))
            {
                // If you agree that link can be deleted, accept deletion.
                if (ed::AcceptDeletedItem())
                {
                    // Then remove link from your data.
                    for (auto& link : m_Links)
                    {
                        if (link.Id == deletedLinkId)
                        {
                            m_Links.erase(&link);
                            break;
                        }
                    }
                }

                // You may reject link deletion by calling:
                // ed::RejectDeletedItem();
            }
        }
        ed::EndDelete(); // Wrap up deletion action



        // End of interaction with editor.
        ed::End();

        if (m_FirstFrame)
            ed::NavigateToContent(0.0f);

        ed::SetCurrentEditor(nullptr);

        m_FirstFrame = false;

        // ImGui::ShowMetricsWindow();
        };

    auto ui_canvas_call = [&]() {
        auto& io = ImGui::GetIO();

        ImGui::Text("FPS: %.2f (%.2gms)", io.Framerate, io.Framerate ? 1000.0f / io.Framerate : 0.0f);

        ImGui::Separator();

        auto availableRegion = ImGui::GetContentRegionAvail();

        static float s_SplitterSize = 6.0f;
        static float s_SplitterArea = 0.0f;
        static float s_LeftPaneSize = 0.0f;
        static float s_RightPaneSize = 0.0f;

        if (s_SplitterArea != availableRegion.x)
        {
            if (s_SplitterArea == 0.0f)
            {
                s_SplitterArea = availableRegion.x;
                s_LeftPaneSize = ImFloor(availableRegion.x * 0.25f);
                s_RightPaneSize = availableRegion.x - s_LeftPaneSize - s_SplitterSize;
            }
            else
            {
                auto ratio = availableRegion.x / s_SplitterArea;
                s_SplitterArea = availableRegion.x;
                s_LeftPaneSize = s_LeftPaneSize * ratio;
                s_RightPaneSize = availableRegion.x - s_LeftPaneSize - s_SplitterSize;
            }
        }

        static ImGuiEx::Canvas canvas;
        static ImVec2 drawStartPoint;
        static bool isDragging = false;
        static ImRect panelRect;

        Splitter(true, s_SplitterSize, &s_LeftPaneSize, &s_RightPaneSize, 100.0f, 100.0f);

        auto canvasRect = canvas.Rect();
        auto viewRect = canvas.ViewRect();
        auto viewOrigin = canvas.ViewOrigin();
        auto viewScale = canvas.ViewScale();

        ImGui::BeginChild("##top", ImVec2(s_LeftPaneSize, -1), false, ImGuiWindowFlags_NoScrollWithMouse);

        ImGui::TextUnformatted("Rect:");
        ImGui::BeginColumns("rect", 2, ImGuiOldColumnFlags_NoBorder);
        ImGui::SetColumnWidth(0, ImGui::CalcTextSize("\t\tL: 0000.00\t").x);
        ImGui::Text("\tL: %.2f", canvasRect.Min.x);       ImGui::NextColumn();
        ImGui::Text("\tT: %.2f", canvasRect.Min.y);       ImGui::NextColumn();
        ImGui::Text("\tR: %.2f", canvasRect.Max.x);       ImGui::NextColumn();
        ImGui::Text("\tB: %.2f", canvasRect.Max.y);       ImGui::NextColumn();
        ImGui::Text("\tW: %.2f", canvasRect.GetWidth());  ImGui::NextColumn();
        ImGui::Text("\tH: %.2f", canvasRect.GetHeight()); ImGui::NextColumn();
        ImGui::EndColumns();

        ImGui::TextUnformatted("View Rect:");
        ImGui::BeginColumns("viewrect", 2, ImGuiOldColumnFlags_NoBorder);
        ImGui::SetColumnWidth(0, ImGui::CalcTextSize("\t\tL: 0000.00\t").x);
        ImGui::Text("\tL: %.2f", viewRect.Min.x);       ImGui::NextColumn();
        ImGui::Text("\tT: %.2f", viewRect.Min.y);       ImGui::NextColumn();
        ImGui::Text("\tR: %.2f", viewRect.Max.x);       ImGui::NextColumn();
        ImGui::Text("\tB: %.2f", viewRect.Max.y);       ImGui::NextColumn();
        ImGui::Text("\tW: %.2f", viewRect.GetWidth());  ImGui::NextColumn();
        ImGui::Text("\tH: %.2f", viewRect.GetHeight()); ImGui::NextColumn();
        ImGui::EndColumns();

        ImGui::TextUnformatted("Origin:");
        ImGui::Indent();
        auto originChanged = false;
        ImGui::PushItemWidth(-ImGui::GetStyle().IndentSpacing);
        originChanged |= ImGui::DragFloat("##originx", &viewOrigin.x, 1.0f);
        originChanged |= ImGui::DragFloat("##originy", &viewOrigin.y, 1.0f);
        if (originChanged) canvas.SetView(viewOrigin, viewScale);
        ImGui::PopItemWidth();
        ImGui::Unindent();

        ImGui::TextUnformatted("Scale:");
        ImGui::Indent();
        ImGui::PushItemWidth(-ImGui::GetStyle().IndentSpacing);
        if (ImGui::DragFloat("##scale", &viewScale, 0.01f, 0.01f, 15.0f))
            canvas.SetView(viewOrigin, viewScale);
        ImGui::PopItemWidth();
        ImGui::Unindent();

        ImGui::Separator();

        if (ImGui::Button("Center over Panel", ImVec2(s_LeftPaneSize, 0)))
            canvas.CenterView(panelRect.GetCenter());

        if (ImGui::Button("Center and zoom to Panel", ImVec2(s_LeftPaneSize, 0)))
            canvas.CenterView(panelRect);

        ImGui::TextUnformatted("Panel Rect:");
        ImGui::BeginColumns("panelrect", 2, ImGuiOldColumnFlags_NoBorder);
        ImGui::SetColumnWidth(0, ImGui::CalcTextSize("\t\tL: 0000.00\t").x);
        ImGui::Text("\tL: %.2f", panelRect.Min.x);       ImGui::NextColumn();
        ImGui::Text("\tT: %.2f", panelRect.Min.y);       ImGui::NextColumn();
        ImGui::Text("\tR: %.2f", panelRect.Max.x);       ImGui::NextColumn();
        ImGui::Text("\tB: %.2f", panelRect.Max.y);       ImGui::NextColumn();
        ImGui::Text("\tW: %.2f", panelRect.GetWidth());  ImGui::NextColumn();
        ImGui::Text("\tH: %.2f", panelRect.GetHeight()); ImGui::NextColumn();
        ImGui::EndColumns();

        ImGui::EndChild();

        ImGui::SameLine(0.0f, s_SplitterSize);


        if (canvas.Begin("##mycanvas", ImVec2(s_RightPaneSize, 0.0f)))
        {
            //auto drawList = ImGui::GetWindowDrawList();

            if ((isDragging || ImGui::IsItemHovered()) && ImGui::IsMouseDragging(1, 0.0f))
            {
                if (!isDragging)
                {
                    isDragging = true;
                    drawStartPoint = viewOrigin;
                }

                canvas.SetView(drawStartPoint + ImGui::GetMouseDragDelta(1, 0.0f) * viewScale, viewScale);
            }
            else if (isDragging)
                isDragging = false;

            viewRect = canvas.ViewRect();

            if (viewRect.Max.x > 0.0f)
                DrawScale(ImVec2(0.0f, 0.0f), ImVec2(viewRect.Max.x, 0.0f), 100.0f, 10.0f, 0.6f);
            if (viewRect.Min.x < 0.0f)
                DrawScale(ImVec2(0.0f, 0.0f), ImVec2(viewRect.Min.x, 0.0f), 100.0f, 10.0f, 0.6f, -1.0f);
            if (viewRect.Max.y > 0.0f)
                DrawScale(ImVec2(0.0f, 0.0f), ImVec2(0.0f, viewRect.Max.y), 100.0f, 10.0f, 0.6f);
            if (viewRect.Min.y < 0.0f)
                DrawScale(ImVec2(0.0f, 0.0f), ImVec2(0.0f, viewRect.Min.y), 100.0f, 10.0f, 0.6f, -1.0f);

            ImGui::Text("Hovered:     %d", ImGui::IsItemHovered() ? 1 : 0);

            ImGui::TextUnformatted("Hello World!");

            ImGui::Bullet();

            ImGui::Button("Panel", ImVec2(s_RightPaneSize * 0.75f, availableRegion.y * 0.5f) * 0.5f);
            panelRect.Min = ImGui::GetItemRectMin();
            panelRect.Max = ImGui::GetItemRectMax();

            canvas.End();
        }

        };
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        ImGui::Begin("##dockspace", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking);
        ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
        ImGui::SetWindowPos({ 0.f, 0.f });
        ImVec2 displaySize = ImGui::GetIO().DisplaySize;
        ImGui::SetWindowSize({ (float)displaySize.x, (float)displaySize.y });
        ImGui::End();
        ImGui::PopStyleVar(3);
        if (ImGui::Begin("Simple")) {
            ui_sample_call();
        }
        ImGui::End();
        if (ImGui::Begin("widgets", nullptr, ImGuiWindowFlags_None)) {
            ui_widgets_call();
        }
        ImGui::End();
        if (ImGui::Begin("Canvas", nullptr, ImGuiWindowFlags_None)) {
            ui_canvas_call();
        }
        ImGui::End();
        if (ImGui::Begin("Basicinteracton", nullptr, ImGuiWindowFlags_None)) {
            ui_basicinteracton_call();
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

