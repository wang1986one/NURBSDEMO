#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "ImGuizmo.h"
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include  <math.h>
#include<vector>
#include <iostream>
#include <imgui.h>
#include <filament/Camera.h>
#include <filament/Engine.h>
#include <filament/IndexBuffer.h>
#include <filament/Material.h>
#include <filament/MaterialInstance.h>
#include <filament/RenderableManager.h>
#include <filament/Scene.h>
#include <filament/Skybox.h>
#include <filament/TransformManager.h>
#include <filament/VertexBuffer.h>
#include<filament/Renderer.h>
#include<filament/SwapChain.h>
#include<filament/Viewport.h>
#include <filament/View.h>
#include"generated/resources/resources.h"
#include <utils/EntityManager.h>


#define SCR_WIDTH 800
#define SCR_HEIGHT 600
#define M_PI       3.14159265358979323846
void g_mainWindow(GLFWwindow* window, int w, int h) {
   
}
void Scorllcallback(GLFWwindow* window, double dx, double dy) {

}
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

}
void mouseButton(GLFWwindow* window, int B, int S, int F) {
    
}
void mouseMove(GLFWwindow* window, double x, double y) {
}
struct App {
    
    filament::VertexBuffer* vb;
    filament::IndexBuffer* ib;
    filament::Material* mat;
    filament::Camera* cam;
    utils::Entity camera;
    filament::Skybox* skybox;
    utils::Entity renderable;
};
struct Vertex {
    filament::math::float2 position;
    uint32_t color;
};

static const Vertex TRIANGLE_VERTICES[3] = {
    {{1, 0}, 0xffff0000u},
    {{cos(M_PI * 2 / 3), sin(M_PI * 2 / 3)}, 0xff00ff00u},
    {{cos(M_PI * 4 / 3), sin(M_PI * 4 / 3)}, 0xff0000ffu},
};
static constexpr uint16_t TRIANGLE_INDICES[3] = { 0, 1, 2 };
int main()
{
    App app{};
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
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
    const char* glsl_version = "#version 460";

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    glEnable(GL_DEPTH_TEST);


    auto builder=filament::Material::Builder();

    filament::Engine::Config engineConfig = {};
    engineConfig.stereoscopicEyeCount = 2;
    engineConfig.stereoscopicType = filament::Engine::StereoscopicType::NONE;
    auto engine=filament::Engine::Builder()
        .backend(filament::backend::Backend::OPENGL)
        .featureLevel(filament::Engine::FeatureLevel::FEATURE_LEVEL_0)
        .config(&engineConfig)
        .build();
    

    auto scene = engine->createScene();
    auto renderer = engine->createRenderer();
    // create cameras
    utils::Entity mCameraEntities[3];
    filament::Camera* mCameras[3] = { nullptr };
    filament::Camera* mMainCamera;
    filament::Camera* mDebugCamera;
    filament::Camera* mOrthoCamera;
    utils::EntityManager& em = utils::EntityManager::get();
    em.create(3, mCameraEntities);
    mCameras[0] = mMainCamera = engine->createCamera(mCameraEntities[0]);
    mCameras[1] = mDebugCamera = engine->createCamera(mCameraEntities[1]);
    mCameras[2] = mOrthoCamera = engine->createCamera(mCameraEntities[2]);
    mMainCamera->lookAt({ 4, 0, -4 }, { 0, 0, -4 }, { 0, 1, 0 });
    mMainCamera->setScaling({800.0 / 600.0, 1.0 });
    mMainCamera->setLensProjection(28.0, 1.0, 0.1f, 100.0f);
    // set exposure
    for (auto camera : mCameras) {
        camera->setExposure(16.0f, 1 / 125.0f, 100.0f);
    }
    auto view = engine->createView();
    view->setName("Main View");
    view->setCamera(mMainCamera);
    view->setViewport(filament::Viewport{ 0, 0, SCR_WIDTH, SCR_HEIGHT });
    view->setScene(scene);

    auto setup = [&app](filament::Engine* engine, filament::View* view, filament::Scene* scene) {
        app.skybox = filament::Skybox::Builder().color({ 0.1, 0.125, 0.25, 1.0 }).build(*engine);
        scene->setSkybox(app.skybox);
        view->setPostProcessingEnabled(false);
        static_assert(sizeof(Vertex) == 12, "Strange vertex size.");
        app.vb = filament::VertexBuffer::Builder()
            .vertexCount(3)
            .bufferCount(1)
            .attribute(filament::VertexAttribute::POSITION, 0, filament::VertexBuffer::AttributeType::FLOAT2, 0, 12)
            .attribute(filament::VertexAttribute::COLOR, 0, filament::VertexBuffer::AttributeType::UBYTE4, 8, 12)
            .normalized(filament::VertexAttribute::COLOR)
            .build(*engine);
        app.vb->setBufferAt(*engine, 0,
            filament::VertexBuffer::BufferDescriptor(TRIANGLE_VERTICES, 36, nullptr));
        app.ib = filament::IndexBuffer::Builder()
            .indexCount(3)
            .bufferType(filament::IndexBuffer::IndexType::USHORT)
            .build(*engine);
        app.ib->setBuffer(*engine,
            filament::IndexBuffer::BufferDescriptor(TRIANGLE_INDICES, 6, nullptr));
        app.mat = filament::Material::Builder()
            .package(RESOURCES_BAKEDCOLOR_DATA, RESOURCES_BAKEDCOLOR_SIZE)
            .build(*engine);
        app.renderable = utils::EntityManager::get().create();
        filament::RenderableManager::Builder(1)
            .boundingBox({ { -1, -1, -1 }, { 1, 1, 1 } })
            .material(0, app.mat->getDefaultInstance())
            .geometry(0, filament::RenderableManager::PrimitiveType::TRIANGLES, app.vb, app.ib, 0, 3)
            .culling(false)
            .receiveShadows(false)
            .castShadows(false)
            .build(*engine, app.renderable);
        scene->addEntity(app.renderable);
        app.camera = utils::EntityManager::get().create();
        app.cam = engine->createCamera(app.camera);
        view->setCamera(app.cam);
        } ;
    auto cleanup = [&app](filament::Engine* engine, filament::View*, filament::Scene*) {
        engine->destroy(app.skybox);
        engine->destroy(app.renderable);
        engine->destroy(app.mat);
        engine->destroy(app.vb);
        engine->destroy(app.ib);
        engine->destroyCameraComponent(app.camera);
        utils::EntityManager::get().destroy(app.camera);
        };
    auto animate = [&app](filament::Engine* engine, filament::View* view, double now) {
        constexpr float ZOOM = 1.5f;
        const uint32_t w = SCR_WIDTH;
        const uint32_t h = SCR_HEIGHT;
        const float aspect = (float)w / h;
        app.cam->setProjection(filament::Camera::Projection::ORTHO,
            -aspect * ZOOM, aspect * ZOOM,
            -ZOOM, ZOOM, 0, 1);
        auto& tcm = engine->getTransformManager();
        tcm.setTransform(tcm.getInstance(app.renderable),
            filament::math::mat4f::rotation(now, filament::math::float3{ 0, 0, 1 }));
        };

    setup(engine, view, scene);
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Text("Hello World");

        if (renderer->beginFrame(nullptr)) {
            renderer->render(view);
            renderer->endFrame();
        }

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

