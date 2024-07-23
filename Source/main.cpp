#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "ImGuizmo.h"
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include"mat_vec.h"
#include "Ray.h"
#include"spline.h"
#include"surface.h"
#pragma region  igl
#include <readOFF.h>
#include<readOBJ.h>
#include <unproject_ray.h>
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
#pragma endregion
#include  <math.h>
#include<vector>
#define SCR_WIDTH 800
#define SCR_HEIGHT 600
struct DrawOptions {
    int segments = 150;
    int k = 3;
    bool view_control = true;
}drawoption;
void g_mainWindow(GLFWwindow* window, int w, int h) {
    glfw_window_size(window,  w,  h);
}
void Scorllcallback(GLFWwindow* window, double dx, double dy) {
    if(drawoption.view_control)
    glfw_mouse_scroll(window,  dx,  dy);
}
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (drawoption.view_control)
    glfw_key_callback(window, key,  scancode,  action,  mods);
}
void mouseButton(GLFWwindow* window, int B, int S, int F) {
    
    if (drawoption.view_control)
    glfw_mouse_press(window,  B,  S,  F);
}
void mouseMove(GLFWwindow* window, double x, double y) {
    if (drawoption.view_control)
    glfw_mouse_move(window,  x,  y);
}

NURBS::Spline* spline=nullptr;
std::vector<NURBS::CP>pos_arr;
NURBS::Surface* surface=nullptr;
std::vector<NURBS::CP>surface_pos_arr;
int Intersection(const MathN::Ray& ray,const float radius ) {
    float dis= std::numeric_limits<float>::infinity();
    int index = -1;
    for (int i = 0; i < pos_arr.size();i++) {
        float tm=0.0f;
        if (ray.HitDistance(pos_arr[i].pos, radius, tm)) {
            if (tm < dis) {
                index = i;
                dis = tm;
            }
        }
    }
    return index;
}
bool Guizmo() {

    auto mpos=ImGui::GetMousePos();
    const Eigen::Vector2f pos(
        mpos.x, viewer.core().viewport(3) - mpos.y);
    const auto model = viewer.core().view;
    const auto proj = viewer.core().proj;
    const auto viewport = viewer.core().viewport;
    Eigen::RowVector3d src;
    Eigen::RowVector3d dir;
    {
        Eigen::Vector3f src_f, dir_f;
        igl::unproject_ray(pos, model, proj, viewport, src_f, dir_f);
        src = src_f.cast<double>().transpose();
        dir = dir_f.cast<double>().transpose();
    }
    bool flag = false;
    MathN::Ray ray(src,dir);
    static int index = -1;
    if(ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Right))
        index = Intersection(ray, 0.1f);
    if (index != -1) {
        
        float xform[16] = { 

        };
        float matrixRotation[3] = { 0,0,0 }, matrixScale[3] = {1,1,1};
        ImGuizmo::RecomposeMatrixFromComponents(pos_arr[index].pos.data(), matrixRotation, matrixScale,xform);
        ImGuiIO& io = ImGui::GetIO();
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
        ImGuizmo::BeginFrame();
        ImGui::PopStyleVar();
        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
        ImGuizmo::SetOrthographic(false);

        if (ImGuizmo::Manipulate(model.data(), proj.data(), ImGuizmo::TRANSLATE, ImGuizmo::LOCAL, xform, NULL, NULL)) {
            flag = true;
        }
        ImGuizmo::DecomposeMatrixToComponents(xform, pos_arr[index].pos.data(), matrixRotation, matrixScale);
    }       
    return flag;

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
    ImGuiIO& io = ImGui::GetIO();
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    glEnable(GL_DEPTH_TEST);
    pos_arr = { {0,0,0},{1,0,0},{1,1,1},{2,1,1},{2,0,0},{3,0,0},{3,1,1} };
    /*
    
    surface_pos_arr = {
        {4.60128021,0,-1.26708055},{5.50103045,2.58807635,2.07579565},{-1.50000024,2.58807611,-8.77062988},
        {-3,-2.62268316e-07,0},{-1.49999976,-2.59807634,0},{1.50000107,-2.59807563,0},
        {5.94173336,-0.696736932,-4.70421982},{1.24999988,2.16506362,0.866025448},{-1.25000012,1.18786144,0.86559689},
        {-2.5,-2.18556949e-07,0.238941669},{-1.24999976,-2.16506362,0.866025448},{1.39073896,-2.1650629,-2.00983953},
        {-2.17055154,-0.683442593,12.9277067},{0.74999994,1.29903817,0.866025388},{-0.750000119,1.29903805,0.866025388},
        {2.94000006,-1.31134158e-07,4.98983479},{-0.749999881,-1.29903817,0.866025388},{0.750000536,-1.29903781,0.866025388},
        {1,0,-8.74227766e-08},{0.49999997,0.866025448,-8.74227766e-08},{-0.50000006,0.866025388,-8.74227766e-08},
        {-1,-8.74227766e-08,-8.74227766e-08},{-0.499999911,-0.866025448,-8.74227766e-08},{0.500000358,-0.866025209,-8.74227766e-08},
        {1.50000012,0,-0.866025448},{4.11833334,1.2890383,0.0910527706},{-0.750000179,1.29903817,-0.866025448},
        {-1.50000012,-1.31134172e-07,-0.866025448},{-0.74999994,-1.29903829,-0.866025448},{0.750000596,-1.29903793,-0.866025448},
        {3.6006763,1.17056704,-5.11826801},{1.25000012,2.1650641,-0.866025209},{-1.25000036,2.16506386,-0.866025209},
        {-2.64777708,-2.18556977e-07,-2.43669486},{-1.25,-2.1650641,-0.866025209},{6.89189672,-2.16506338,-15.1219845}
    };    
    */
    surface_pos_arr = {
    {-2,0,-2},{-1,0,-2},{0,0,-2},{1,0,-2},{2,0,-2},
    {-2,0,-1},{-1,0,-1},{-0,0,-1},{1,0,-1},{2,0,-1},
    {-2,0,0},{-1,0,0},{-0,0,0},{1,0,0},{2,0,0},
    {-2,0,1},{-1,0,1},{-0,0,1},{1,0,1},{2,0,1},
    {-2,0,2},{-1,0,2},{-0,0,2},{1,0,2},{2,0,2},
    };

    auto GetSplineCurve = [&]() {
        Eigen::MatrixXd ans;
        ans.resize(drawoption.segments - 1,3);
        float seg = 1.0f / drawoption.segments;
        float u = seg;
        for (int i = 0; i < drawoption.segments-1; i++) {
            MathN::vec3 v;
            if (spline->GetCurve(u, v)) {
                u += seg;
                ans.row(i) << v[0], v[1], v[2];
            }
            else
            {
                ans.setZero();
                break;
            }
        }
        return ans;
    };

    auto update_curve = [&]() {
        if (spline) {
            delete spline;
            spline = nullptr;
        }
        spline = new NURBS::Spline(pos_arr, drawoption.k, NURBS::Standrad);
        Eigen::MatrixXd TV;
        Eigen::MatrixXi TE;
        Eigen::MatrixXd TC;
        TV.resize(pos_arr.size(), 3);
        TE.resize(pos_arr.size() - 1, 2);
        TC.resize(pos_arr.size() - 1, 3);
        for (int i = 0; i < pos_arr.size(); i++) {
            float* p = pos_arr[i].pos.data();
            TV.row(i) << p[0], p[1], p[2];
            if (i > 0) {
               TE.row(i - 1) << i - 1, i;
               TC.row(i-1) << 1, 1, 1;
            }
                
        }

        viewer.data(0).set_points(TV, Eigen::RowVector3d(1, 1, 0));
        viewer.data(0).point_size = 15;
        int lastedx = TE.rows();
        int lastvdx = TV.rows();
        auto curve = GetSplineCurve();
        TV.conservativeResize(TV.rows() + curve.rows(), 3);
        TV.bottomRows(curve.rows()) = curve;
        TE.conservativeResize(lastedx + curve.rows() - 1, 2);
        TC.conservativeResize(lastedx + curve.rows() - 1, 3);
        for (int i = 0; i < curve.rows() - 1; i++) {
            TE.row(lastedx + i) << i + lastvdx, i + lastvdx + 1;
            TC.row(lastedx + i) << 0,1,1;
        }
        viewer.data(0).set_edges(TV, TE,TC);
        
        viewer.data(0).line_width = 2;
        };
    update_curve();

    auto update_surface = [&]() {
        if (surface) {
            delete surface;
            surface = nullptr;
        }
        surface = new NURBS::Surface(surface_pos_arr,3,5,5,NURBS::Standrad, NURBS::Standrad);
        Eigen::MatrixXd TV;
        int divisionx = 20, divisiony = 20;
        TV.resize((divisionx+1)*(divisiony+1), 3);
        std::vector<int>idcs;
        int lx = divisionx + 1;
        int ly = divisiony + 1;
        float dx = 1.0f /divisionx;
        float dy = 1.0f /divisiony;
        for (int iy = 0; iy < ly; iy++)
            for (int ix = 0; ix < lx; ix++) {
                int i = ix + iy * lx;
                float x = std::min(ix * dx, 1.0f - 1e-5f);
                float y = std::min(iy * dy, 1.0f - 1e-5f);
                MathN::vec3 v(0, 0, 0);
                bool f = surface->GetCurve(x, y,  v);
                if (!f) std::cout << "surface range is somehow wrong" << std::endl;;
                TV.row(i) << v[0],v[1],v[2];
                if(iy < divisiony && ix<divisionx) {
                    idcs.push_back(i);
                    idcs.push_back(i + 1);
                    idcs.push_back(i + lx);
                    idcs.push_back(i + lx);
                    idcs.push_back(i + 1);
                    idcs.push_back(i + lx + 1);
                }
            }
        Eigen::MatrixXi TF;
        TF.resize(idcs.size()/3,3);
        for (int i = 0; i < idcs.size() / 3; i++) {
            TF.row(i) << idcs[3 * i], idcs[3 * i + 1], idcs[3 * i + 2];
        }

        viewer.data(1).set_mesh(TV, TF);
        viewer.core().align_camera_center(TV, TF);

    };
    
    // Load a mesh in OFF format
    //igl::readOFF("D:\\Project\\pmp\\pmp-data\\off\\elephant.off", V, F);
    viewer.append_mesh();
    update_surface();
    //viewer.data(1).set_mesh(V, F);
    //viewer.core().align_camera_center(V, F);
    viewer.init();
    viewer.window = window;
    viewer.post_resize(SCR_WIDTH, SCR_HEIGHT);
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        bool flag = false;
        flag |= ImGui::SliderInt("Segments", &drawoption.segments, 5, 150);
        flag |= ImGui::SliderInt("K", &drawoption.k, 1, 10);
        flag |= Guizmo();
        for (int i = 0; i < pos_arr.size();i++) {
            std::string temp_name = "weight " + std::to_string(i)+":";
            flag|=ImGui::SliderFloat(temp_name.c_str(),&pos_arr[i].w,1.0f,50.0f);
        }
        if (ImGui::IsKeyReleased(ImGuiKey_::ImGuiKey_Space)) {
            drawoption.view_control = !drawoption.view_control;
        }
        if(flag)
            update_curve();
        viewer.draw();
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

