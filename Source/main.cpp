#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <readOFF.h>
#include <opengl/glfw/Viewer.h>
static double highdpiw = 1; // High DPI width
static double highdpih = 1; // High DPI height
static double scroll_x = 0;
static double scroll_y = 0;
igl::opengl::glfw::Viewer viewer;
Eigen::MatrixXd V;
Eigen::MatrixXi F;
static void glfw_mouse_press(GLFWwindow* /*window*/, int button, int action, int modifier)
{

    igl::opengl::glfw::Viewer::MouseButton mb;

    if (button == GLFW_MOUSE_BUTTON_1)
        mb = igl::opengl::glfw::Viewer::MouseButton::Left;
    else if (button == GLFW_MOUSE_BUTTON_2)
        mb = igl::opengl::glfw::Viewer::MouseButton::Right;
    else //if (button == GLFW_MOUSE_BUTTON_3)
        mb = igl::opengl::glfw::Viewer::MouseButton::Middle;

    if (action == GLFW_PRESS)
        viewer.mouse_down(mb, modifier);
    else
        viewer.mouse_up(mb, modifier);
}

static void glfw_error_callback(int /*error*/, const char* description)
{
    fputs(description, stderr);
}

static void glfw_char_mods_callback(GLFWwindow* /*window*/, unsigned int codepoint, int modifier)
{
    viewer.key_pressed(codepoint, modifier);
}

static void glfw_key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int modifier)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (action == GLFW_PRESS)
        viewer.key_down(key, modifier);
    else if (action == GLFW_RELEASE)
        viewer.key_up(key, modifier);
}

static void glfw_window_size(GLFWwindow* /*window*/, int width, int height)
{
    int w = width * highdpiw;
    int h = height * highdpih;

    viewer.post_resize(w, h);

}

static void glfw_mouse_move(GLFWwindow* /*window*/, double x, double y)
{
    viewer.mouse_move(x * highdpiw, y * highdpih);
}

static void glfw_mouse_scroll(GLFWwindow* /*window*/, double x, double y)
{
    using namespace std;
    scroll_x += x;
    scroll_y += y;

    viewer.mouse_scroll(y);
}

static void glfw_drop_callback(GLFWwindow* /*window*/, int /*count*/, const char** /*filenames*/)
{
}

#include "DebugDraw.h"
#include  <math.h>

#include<vector>
#define SCR_WIDTH 800
#define SCR_HEIGHT 600
std::vector<Vertex>cparr;

struct CP
{
    Vec2 pos;
    float w;
};
enum SplineType
{
    Standrad,
    Loop,
    Clamped
};
enum KnotType
{
    Uniform,
    OpenUniform
};
class Shared {
public:

    static std::vector<float> GenKnotVectors(int order, int cpsNum, KnotType type) {
        int knotNum = cpsNum + order + 1;
        std::vector<float> knots(knotNum);
        for (int i = 0; i < knotNum; i++) knots[i] = KnotVector(i, order, cpsNum, type);
        return knots;
    }

    static float KnotVector(int j, int order, int cpsNum, KnotType type) {
        int knotNum = cpsNum + order + 1;
        if (type == KnotType::Uniform)     return UniformKnotVector(j, knotNum);
        return OpenUniformKnotVector(j, order, knotNum);
    }

    static float UniformKnotVector(int j, int knotNum) {
        float t0 = 0.0f;
        float t1 = 1.0f;
        return t0 + (t1 - t0) / (knotNum - 1) * j;
    }

    static float OpenUniformKnotVector(int j, int order, int knotNum) {
        if (j <= order) return 0.0f;
        if (j >= knotNum - 1 - order) return 1.0f;
        return (float)j / (knotNum - order + 1);
    }
};
class Spline {
public:
    std::vector<CP>cps;
    SplineType splineType;
    KnotType knotType;
    bool loop;
    int order;
    float min = 0.0;
    float max = 1.0f;
    std::vector<float> knots;
    float shift(float t) { return min + (max - min) * t; }

    Spline(const std::vector<CP>& cps, int order, SplineType t) {
        this->order = order;
        this->loop = t == SplineType::Loop;
        this->knotType = t == SplineType::Clamped ? KnotType::OpenUniform : KnotType::Uniform;

        if (loop) {
            this->cps.resize(cps.size() + order);

            std::copy(cps.begin(), cps.end(), this->cps.begin());
            for (int i = 0; i < order; i++) this->cps[this->cps.size() - order + i] = cps[i];
        }
        else {
            this->cps = cps;
        }

        this->knots = Shared::GenKnotVectors(order, this->cps.size(), knotType);


    }

    void SetCP(int i, CP cp) {
        cps[i] = cp;
        if (loop && i < order) cps[cps.size() - order + i] = cp;
    }


    bool GetCurve(float normT, Vec2& v) {
        float t = shift(normT);
        Vec2 f = { 0.0f,0.0f };
        float d = 0.0f;
        for (int i = 0; i < cps.size(); i++) {
            float b = BasisFunc(i, order, t);
            CP c = cps[i];
            f += c.pos * b * c.w;
            d += b * c.w;
        }
        v = f / (d != 0 ? d : 1e-9f);
        return t >= min && t <= max;
    }


    /*
    bool GetFirstDerivative(float normT, const Vector3 v) {
        var t = shift(normT);
        v = Vector3.zero;
        for (int i = 0; i < cps.Length; i++)
            v += BasisFancFirstDerivative(i, order, t) * cps[i].pos;
        return t >= min && t <= max;
    }

    */

    /*
     public bool GetSecondDerivative(float normT, out Vector3 v) {
        var t = shift(normT);
        v = Vector3.zero;
        for (int i = 0; i < cps.Length; i++)
            v += BasisFancSecondDerivative(i, order, t) * cps[i].pos;
        return t >= min && t <= max;
    }
    */


    float BasisFunc(int j, int k, float t) {
        if (k == 0) { return (t >= knots[j] && t < knots[j + 1]) ? 1.0f : 0.0f; }
        else {
            float d1 = knots[j + k] - knots[j];
            float d2 = knots[j + k + 1] - knots[j + 1];
            float c1 = d1 != 0 ? (t - knots[j]) / d1 : 0;
            float c2 = d2 != 0 ? (knots[j + k + 1] - t) / d2 : 0;
            return c1 * BasisFunc(j, k - 1, t) + c2 * BasisFunc(j + 1, k - 1, t);
        }
    }

    float BasisFancFirstDerivative(int i, int o, float t) {
        float i1 = BasisFunc(i + 1, o - 1, t) / (knots[i + o] - knots[i + 1]);
        float i2 = BasisFunc(i, o - 1, t) / (knots[i + o - 1] - knots[i]);
        return (o - 1) * (-i1 + i2);
    }

    float BasisFancSecondDerivative(int i, int o, float t) {
        float i1 = BasisFancFirstDerivative(i + 1, o - 1, t) / (knots[i + o] - knots[i + 1]);
        float i2 = BasisFancFirstDerivative(i, o - 1, t) / (knots[i + o - 1] - knots[i]);
        return (o - 1) * (-i1 + i2);
    }

    /*
        public Spline hodograph{ get; set; }
    float[] knots_droped;

    public static Spline GenHodograph(Spline s) {
        var cps = new CP[s.cps.Length - 1];
        if (s.knotType == KnotType.OpenUniform) {
            for (var i = 0; i < cps.Length; i++) {
                var q = Vector3.zero;
                if (i == 0) {
                    q = (s.cps[1].pos - s.cps[0].pos) / (s.knots[s.order + 1]);
                }
                else if (i == cps.Length - 1) {
                    q = (s.cps[i + 1].pos - s.cps[i].pos) / (1 - s.knots[s.knots.Length - s.order - 1]);
                }
                else {
                    q = (s.cps[i + 1].pos - s.cps[i].pos) / (s.knots[i + s.order + 1] - s.knots[i + 1]);
                }
                cps[i] = new CP(s.order * q, 1);
            }

        }
        else {
            for (var i = 0; i < cps.Length; i++) {
                var q = (s.cps[i + 1].pos - s.cps[i].pos) / (s.knots[i + s.order + 1] - s.knots[i + 1]);
                cps[i] = new CP(s.order * q, 1);
            }
        }
        return new Spline(cps, s.order - 1, s.splineType);
    }
    */

};
Spline* GetSpline() {
    std::vector<CP>CPS;
    for (auto it : cparr) {
        CPS.push_back({ it,1.0f });
    }
    return new Spline(CPS, 3, SplineType::Standrad);
}
Spline* spline;
std::vector<Vec2>ConvertCPS(Spline* spline, int segments) {

    float add = 1.0f / segments;
    float u = add;
    std::vector<Vec2> ans;
    for (float i = 0; i < segments - 1; i++) {
        Vec2 t;
        if (spline->GetCurve(u, t)) {
            ans.push_back(t);
            u += add;
        }
        else {
            ans.swap(std::vector<Vec2>());
            break;
        }
    }
    return ans;
}

std::vector<Vec2>spline_curve;



bool rightMouseDown = false;
bool leftMouseDown = false;
Vertex clickPointWS;
std::vector<Vertex*>v_capture;
struct DrawOptions {
    int segments = 15;
}drawoption;
void g_mainWindow(GLFWwindow* window, int w, int h) {
    glViewport(0, 0, w, h);
    g_camera.m_width = w;
    g_camera.m_height = h;
    glfw_window_size(window,  w,  h);
    
}
void Scorllcallback(GLFWwindow* window, double dx, double dy) {
    if (dy > 0)
    {
        g_camera.m_zoom /= 1.1f;
    }
    else
    {
        g_camera.m_zoom *= 1.1f;
    }
    glfw_mouse_scroll(window,  dx,  dy);
}
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_ESCAPE:
            // Quit
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;
        case GLFW_KEY_LEFT:
            // Pan left 
            g_camera.m_center.x -= 0.5f;
            break;
        case GLFW_KEY_RIGHT:
            // Pan left 
            g_camera.m_center.x += 0.5f;
            break;
        case GLFW_KEY_UP:
            g_camera.m_center.y += 0.5f;
            break;
        case GLFW_KEY_DOWN:
            g_camera.m_center.y -= 0.5f;
            break;
        default:break;
        }
    }
    glfw_key_callback(window, key,  scancode,  action,  mods);
}
void mouseButton(GLFWwindow* window, int B, int S, int F) {
    double xd, yd;
    glfwGetCursorPos(window, &xd, &yd);
    Vec2 ps((float)xd, (float)yd);
    if (B == GLFW_MOUSE_BUTTON_2) {//鼠标右键回调事件
        if (S == GLFW_PRESS) {
            rightMouseDown = true;
            clickPointWS = g_camera.ConvertScreenToWorld(ps);
        }
        if (S == GLFW_RELEASE) {
            rightMouseDown = false;
        }
    }
    if (B == GLFW_MOUSE_BUTTON_1) {//鼠标左键回调事件
        if (S == GLFW_PRESS) {
            leftMouseDown = true;
            clickPointWS = g_camera.ConvertScreenToWorld(ps);
            for (auto& it : cparr) {
                float deltax = it.x - clickPointWS.x;
                float deltay = it.y - clickPointWS.y;
                float delta = deltax * deltax + deltay * deltay;
                if (delta < 0.1 * g_camera.m_zoom) {
                    v_capture.push_back(&it);
                    break;
                }
            }
        }
        if (S == GLFW_RELEASE) {
            leftMouseDown = false;
            v_capture.clear();
        }
    }
    glfw_mouse_press(window,  B,  S,  F);
}
void mouseMove(GLFWwindow* window, double x, double y) {
    Vec2 ps((float)x, (float)y);
    Vec2 pw = g_camera.ConvertScreenToWorld(ps);

    if (rightMouseDown)
    {
        Vec2 diff = pw - clickPointWS;
        g_camera.m_center.x -= diff.x;
        g_camera.m_center.y -= diff.y;
    }
    if (leftMouseDown)
    {
        Vec2 diff = pw - clickPointWS;
        for (auto it : v_capture) {
            it->x += diff.x;
            it->y += diff.y;
        }
        spline = GetSpline();
        spline_curve = ConvertCPS(spline, drawoption.segments);
    }
    clickPointWS = g_camera.ConvertScreenToWorld(ps);
    glfw_mouse_move(window,  x,  y);
}

void Draw() {
    for (auto it : cparr) {

        maindraw.DrawSolidCircle({ it.x,it.y }, 0.5 * g_camera.m_zoom, { 0,0 }, { 1.0,0.0,0.0,1.0 });
    }
    maindraw.DrawMulLine(cparr.data(), cparr.size(), { 1.0,1.0,0.0,1.0 });
    maindraw.DrawMulLine(spline_curve.data(), spline_curve.size(), { 0.0,1.0,1.0,1.0 });

    maindraw.Flush();
}
int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(g_camera.m_width, g_camera.m_height, "NURBS test", NULL, NULL);

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

    cparr = { {0,0},{1,1},{2,2},{3,3},{4,4},{5,5} };
    spline = GetSpline();
    spline_curve = ConvertCPS(spline, drawoption.segments);
    maindraw.Create();

    // Load a mesh in OFF format
    igl::readOFF("D:/Project/C++/libigl/Build/_deps/libigl_tutorial_data-src" "/bunny.off", V, F);
    viewer.data().set_mesh(V, F);
    viewer.core().align_camera_center(V, F);
    viewer.init();
    viewer.window = window;
    viewer.post_resize(g_camera.m_width, g_camera.m_height);
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        if (ImGui::SliderInt("segment", &drawoption.segments, 2, 450)) {
            spline_curve = ConvertCPS(spline, drawoption.segments);
        }
        if (ImGui::Button("Add cp")) {
            cparr.push_back({ 0,0 });
            spline = GetSpline();
        }
        if (ImGui::Button("Sub cp")) {
            cparr.pop_back();
            spline = GetSpline();
        }
        viewer.draw();
        Draw();
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

