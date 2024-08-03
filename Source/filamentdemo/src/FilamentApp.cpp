#include <memory>

/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <filamentapp/FilamentApp.h>

#if defined(WIN32)
#    include <SDL_syswm.h>
#    include <utils/unwindows.h>
#endif

#include <iostream>

#include <imgui.h>

#include <utils/EntityManager.h>
#include <utils/Panic.h>
#include <utils/Path.h>

#include <filament/Camera.h>
#include <filament/Material.h>
#include <filament/MaterialInstance.h>
#include <filament/Renderer.h>
#include <filament/RenderableManager.h>
#include <filament/Scene.h>
#include <filament/Skybox.h>
#include <filament/SwapChain.h>
#include <filament/View.h>

#ifndef NDEBUG
#include <filament/DebugRegistry.h>
#endif

#if defined(FILAMENT_DRIVER_SUPPORTS_VULKAN)
#include <backend/platforms/VulkanPlatform.h>
#endif



#include <filamentapp/NativeWindowHelper.h>


#include "generated/resources/resources.h"

using namespace filament;

using namespace filament::math;
using namespace utils;

namespace {

    using namespace filament::backend;



} // anonymous namespace

FilamentApp& FilamentApp::get() {
    static FilamentApp filamentApp;
    return filamentApp;
}

FilamentApp::FilamentApp() {
    initSDL();
}

FilamentApp::~FilamentApp() {
    SDL_Quit();
}

View* FilamentApp::getGuiView() const noexcept {
    return nullptr;
    //return mImGuiHelper->getView();
}

void FilamentApp::run(const Config& config, SetupCallback setupCallback,
    CleanupCallback cleanupCallback, ImGuiCallback imguiCallback,
    PreRenderCallback preRender, PostRenderCallback postRender,
    size_t width, size_t height) {
    mWindowTitle = config.title;
    std::unique_ptr<FilamentApp::Window> window(
        new FilamentApp::Window(this, config, config.title, width, height));

    mDepthMaterial = Material::Builder()
        .package(RESOURCES_DEPTHVISUALIZER_DATA, RESOURCES_DEPTHVISUALIZER_SIZE)
        .build(*mEngine);

    mDepthMI = mDepthMaterial->createInstance();

    mDefaultMaterial = Material::Builder()
        .package(RESOURCES_AIDEFAULTMAT_DATA, RESOURCES_AIDEFAULTMAT_SIZE)
        .build(*mEngine);

    mTransparentMaterial = Material::Builder()
        .package(RESOURCES_TRANSPARENTCOLOR_DATA, RESOURCES_TRANSPARENTCOLOR_SIZE)
        .build(*mEngine);

    //std::unique_ptr<Cube> cameraCube(new Cube(*mEngine, mTransparentMaterial, { 1,0,0 }));
    // we can't cull the light-frustum because it's not applied a rigid transform
    // and currently, filament assumes that for culling
   // std::unique_ptr<Cube> lightmapCube(new Cube(*mEngine, mTransparentMaterial, { 0,1,0 }, false));
    mScene = mEngine->createScene();

    window->mMainView->getView()->setVisibleLayers(0x4, 0x4);

    if (config.splitView) {
        auto& rcm = mEngine->getRenderableManager();

        //rcm.setLayerMask(rcm.getInstance(cameraCube->getSolidRenderable()), 0x3, 0x2);
        //rcm.setLayerMask(rcm.getInstance(cameraCube->getWireFrameRenderable()), 0x3, 0x2);

        //rcm.setLayerMask(rcm.getInstance(lightmapCube->getSolidRenderable()), 0x3, 0x2);
        //rcm.setLayerMask(rcm.getInstance(lightmapCube->getWireFrameRenderable()), 0x3, 0x2);

        // Create the camera mesh
        //mScene->addEntity(cameraCube->getWireFrameRenderable());
       // mScene->addEntity(cameraCube->getSolidRenderable());

        //mScene->addEntity(lightmapCube->getWireFrameRenderable());
       // mScene->addEntity(lightmapCube->getSolidRenderable());

        window->mDepthView->getView()->setVisibleLayers(0x4, 0x4);
        window->mGodView->getView()->setVisibleLayers(0x6, 0x6);
        window->mOrthoView->getView()->setVisibleLayers(0x6, 0x6);

        // only preserve the color buffer for additional views; depth and stencil can be discarded.
        window->mDepthView->getView()->setShadowingEnabled(false);
        window->mGodView->getView()->setShadowingEnabled(false);
        window->mOrthoView->getView()->setShadowingEnabled(false);
    }

    //loadDirt(config);
    //loadIBL(config);

    for (auto& view : window->mViews) {
        if (view.get() != window->mUiView) {
            view->getView()->setScene(mScene);
        }
    }

    setupCallback(mEngine, window->mMainView->getView(), mScene);

    if (imguiCallback) {
 
    }

    bool mousePressed[3] = { false };

    int sidebarWidth = mSidebarWidth;
    float cameraFocalLength = mCameraFocalLength;
    float cameraNear = mCameraNear;
    float cameraFar = mCameraFar;

    SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
    SDL_Window* sdlWindow = window->getSDLWindow();

    while (!mClosed) {
        if (mWindowTitle != SDL_GetWindowTitle(sdlWindow)) {
            SDL_SetWindowTitle(sdlWindow, mWindowTitle.c_str());
        }

        if (mSidebarWidth != sidebarWidth ||
            mCameraFocalLength != cameraFocalLength ||
            mCameraNear != cameraNear ||
            mCameraFar != cameraFar) {
            window->configureCamerasForWindow();
            sidebarWidth = mSidebarWidth;
            cameraFocalLength = mCameraFocalLength;
            cameraNear = mCameraNear;
            cameraFar = mCameraFar;
        }

        if (!UTILS_HAS_THREADING) {
            mEngine->execute();
        }

        // Allow the app to animate the scene if desired.
        if (mAnimation) {
            double now = (double)SDL_GetPerformanceCounter() / SDL_GetPerformanceFrequency();
            mAnimation(mEngine, window->mMainView->getView(), now);
        }

        // Loop over fresh events twice: first stash them and let ImGui process them, then allow
        // the app to process the stashed events. This is done because ImGui might wish to block
        // certain events from the app (e.g., when dragging the mouse over an obscuring window).
        constexpr int kMaxEvents = 16;
        SDL_Event events[kMaxEvents];
        int nevents = 0;
        while (nevents < kMaxEvents && SDL_PollEvent(&events[nevents]) != 0) {

            nevents++;
        }

        // Now, loop over the events a second time for app-side processing.
        for (int i = 0; i < nevents; i++) {
            const SDL_Event& event = events[i];
          
            switch (event.type) {
            case SDL_QUIT:
                mClosed = true;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                    mClosed = true;
                }
#ifndef NDEBUG
                if (event.key.keysym.scancode == SDL_SCANCODE_PRINTSCREEN) {
                    DebugRegistry& debug = mEngine->getDebugRegistry();
                    bool* captureFrame =
                        debug.getPropertyAddress<bool>("d.renderer.doFrameCapture");
                    *captureFrame = true;
                }
#endif
                window->keyDown(event.key.keysym.scancode);
                break;
            case SDL_KEYUP:
                window->keyUp(event.key.keysym.scancode);
                break;
            case SDL_MOUSEWHEEL:
                
                    window->mouseWheel(event.wheel.y);
                break;
            case SDL_MOUSEBUTTONDOWN:
                
                    window->mouseDown(event.button.button, event.button.x, event.button.y);
                break;
            case SDL_MOUSEBUTTONUP:
                
                    window->mouseUp(event.button.x, event.button.y);
                break;
            case SDL_MOUSEMOTION:
               
                    window->mouseMoved(event.motion.x, event.motion.y);
                break;
            case SDL_DROPFILE:
                if (mDropHandler) {
                    mDropHandler(event.drop.file);
                }
                SDL_free(event.drop.file);
                break;
            case SDL_WINDOWEVENT:
                switch (event.window.event) {
                case SDL_WINDOWEVENT_RESIZED:
                    window->resize();
                    break;
                default:
                    break;
                }
                break;
            default:
                break;
            }
        }

        // Calculate the time step.
        static Uint64 frequency = SDL_GetPerformanceFrequency();
        Uint64 now = SDL_GetPerformanceCounter();
        const float timeStep = mTime > 0 ? (float)((double)(now - mTime) / frequency) :
            (float)(1.0f / 60.0f);
        mTime = now;

        // Populate the UI scene, regardless of whether Filament wants to a skip frame. We should
        // always let ImGui generate a command list; if it skips a frame it'll destroy its widgets.


        // Update the camera manipulators for each view.
        for (auto const& view : window->mViews) {

        }

        // Update the position and orientation of the two cameras.
        filament::math::float3 eye, center, up;
        //window->mMainCameraMan->getLookAt(&eye, &center, &up);
        window->mMainCamera->lookAt(eye, center, up);
        //window->mDebugCameraMan->getLookAt(&eye, &center, &up);
        window->mDebugCamera->lookAt(eye, center, up);

        // Update the cube distortion matrix used for frustum visualization.
        const Camera* lightmapCamera = window->mMainView->getView()->getDirectionalShadowCamera();
        if (lightmapCamera) {
            //lightmapCube->mapFrustum(*mEngine, lightmapCamera);
        }
        //cameraCube->mapFrustum(*mEngine, window->mMainCamera);

        // Delay rendering for roughly one monitor refresh interval
        // TODO: Use SDL_GL_SetSwapInterval for proper vsync
        SDL_DisplayMode Mode;
        int refreshIntervalMS = (SDL_GetDesktopDisplayMode(
            SDL_GetWindowDisplayIndex(window->mWindow), &Mode) == 0 &&
            Mode.refresh_rate != 0) ? round(1000.0 / Mode.refresh_rate) : 16;
        SDL_Delay(refreshIntervalMS);

        Renderer* renderer = window->getRenderer();

        if (preRender) {
            preRender(mEngine, window->mViews[0]->getView(), mScene, renderer);
        }

        if (mReconfigureCameras) {
            window->configureCamerasForWindow();
            mReconfigureCameras = false;
        }

        if (config.splitView) {
            if (!window->mOrthoView->getView()->hasCamera()) {
                Camera const* debugDirectionalShadowCamera =
                    window->mMainView->getView()->getDirectionalShadowCamera();
                if (debugDirectionalShadowCamera) {
                    window->mOrthoView->setCamera(
                        const_cast<Camera*>(debugDirectionalShadowCamera));
                }
            }
        }

        if (renderer->beginFrame(window->getSwapChain())) {
            for (filament::View* offscreenView : mOffscreenViews) {
                renderer->render(offscreenView);
            }
            for (auto const& view : window->mViews) {
                renderer->render(view->getView());
            }
            if (postRender) {
                postRender(mEngine, window->mViews[0]->getView(), mScene, renderer);
            }
            renderer->endFrame();
        }
        else {
            ++mSkippedFrames;
        }
    }



    cleanupCallback(mEngine, window->mMainView->getView(), mScene);

    //cameraCube.reset();
    //lightmapCube.reset();
    window.reset();

    //mIBL.reset();
    mEngine->destroy(mDepthMI);
    mEngine->destroy(mDepthMaterial);
    mEngine->destroy(mDefaultMaterial);
    mEngine->destroy(mTransparentMaterial);
    mEngine->destroy(mScene);
    Engine::destroy(&mEngine);
    mEngine = nullptr;

#if defined(FILAMENT_DRIVER_SUPPORTS_VULKAN)
    if (mVulkanPlatform) {
        delete mVulkanPlatform;
    }
#endif
}

// RELATIVE_ASSET_PATH is set inside samples/CMakeLists.txt and used to support multi-configuration
// generators, like Visual Studio or Xcode.
#ifndef RELATIVE_ASSET_PATH
#define RELATIVE_ASSET_PATH "."
#endif

const utils::Path& FilamentApp::getRootAssetsPath() {
    static const utils::Path root = utils::Path::getCurrentExecutable().getParent() + RELATIVE_ASSET_PATH;
    return root;
}





void FilamentApp::initSDL() {
    FILAMENT_CHECK_POSTCONDITION(SDL_Init(SDL_INIT_EVENTS) == 0) << "SDL_Init Failure";
}

// ------------------------------------------------------------------------------------------------

FilamentApp::Window::Window(FilamentApp* filamentApp,
    const Config& config, std::string title, size_t w, size_t h)
    : mFilamentApp(filamentApp), mConfig(config), mIsHeadless(config.headless) {
    const int x = SDL_WINDOWPOS_CENTERED;
    const int y = SDL_WINDOWPOS_CENTERED;
    uint32_t windowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI;
    if (config.resizeable) {
        windowFlags |= SDL_WINDOW_RESIZABLE;
    }

    if (config.headless) {
        windowFlags |= SDL_WINDOW_HIDDEN;
    }

    // Even if we're in headless mode, we still need to create a window, otherwise SDL will not poll
    // events.
    mWindow = SDL_CreateWindow(title.c_str(), x, y, (int)w, (int)h, windowFlags);

    auto const createEngine = [&config, this]() {
        auto backend = config.backend;

        // This mirrors the logic for choosing a backend given compile-time flags and client having
        // provided DEFAULT as the backend (see PlatformFactory.cpp)
#if !defined(__EMSCRIPTEN__) && !defined(__ANDROID__) && !defined(IOS) && \
            !defined(__APPLE__) && defined(FILAMENT_DRIVER_SUPPORTS_VULKAN)
        if (backend == Engine::Backend::DEFAULT) {
            backend = Engine::Backend::VULKAN;
        }
#endif

        Engine::Config engineConfig = {};
        engineConfig.stereoscopicEyeCount = config.stereoscopicEyeCount;
#if defined(FILAMENT_SAMPLES_STEREO_TYPE_INSTANCED)
        engineConfig.stereoscopicType = Engine::StereoscopicType::INSTANCED;
#elif defined (FILAMENT_SAMPLES_STEREO_TYPE_MULTIVIEW)
        engineConfig.stereoscopicType = Engine::StereoscopicType::MULTIVIEW;
#else
        engineConfig.stereoscopicType = Engine::StereoscopicType::NONE;
#endif

        if (backend == Engine::Backend::VULKAN) {
#if defined(FILAMENT_DRIVER_SUPPORTS_VULKAN)
            mFilamentApp->mVulkanPlatform =
                new FilamentAppVulkanPlatform(config.vulkanGPUHint.c_str());
            return Engine::Builder()
                .backend(backend)
                .platform(mFilamentApp->mVulkanPlatform)
                .featureLevel(config.featureLevel)
                .config(&engineConfig)
                .build();
#endif
        }
        return Engine::Builder()
            .backend(backend)
            .featureLevel(config.featureLevel)
            .config(&engineConfig)
            .build();
        };

    if (config.headless) {
        mFilamentApp->mEngine = createEngine();
        mSwapChain = mFilamentApp->mEngine->createSwapChain((uint32_t)w, (uint32_t)h);
        mWidth = w;
        mHeight = h;
    }
    else {

        void* nativeWindow = ::getNativeWindow(mWindow);

        // Create the Engine after the window in case this happens to be a single-threaded platform.
        // For single-threaded platforms, we need to ensure that Filament's OpenGL context is
        // current, rather than the one created by SDL.
        mFilamentApp->mEngine = createEngine();

        // get the resolved backend
        mBackend = config.backend = mFilamentApp->mEngine->getBackend();

        void* nativeSwapChain = nativeWindow;

#if defined(__APPLE__)
        ::prepareNativeWindow(mWindow);

        void* metalLayer = nullptr;
        if (config.backend == filament::Engine::Backend::METAL) {
            metalLayer = setUpMetalLayer(nativeWindow);
            // The swap chain on Metal is a CAMetalLayer.
            nativeSwapChain = metalLayer;
        }

#if defined(FILAMENT_DRIVER_SUPPORTS_VULKAN)
        if (config.backend == filament::Engine::Backend::VULKAN) {
            // We request a Metal layer for rendering via MoltenVK.
            setUpMetalLayer(nativeWindow);
        }
#endif

#endif

        // Write back the active feature level.
        config.featureLevel = mFilamentApp->mEngine->getActiveFeatureLevel();

        mSwapChain = mFilamentApp->mEngine->createSwapChain(
            nativeSwapChain, filament::SwapChain::CONFIG_HAS_STENCIL_BUFFER);
    }

    mRenderer = mFilamentApp->mEngine->createRenderer();

    // create cameras
    utils::EntityManager& em = utils::EntityManager::get();
    em.create(3, mCameraEntities);
    mCameras[0] = mMainCamera = mFilamentApp->mEngine->createCamera(mCameraEntities[0]);
    mCameras[1] = mDebugCamera = mFilamentApp->mEngine->createCamera(mCameraEntities[1]);
    mCameras[2] = mOrthoCamera = mFilamentApp->mEngine->createCamera(mCameraEntities[2]);

    // set exposure
    for (auto camera : mCameras) {
        camera->setExposure(16.0f, 1 / 125.0f, 100.0f);
    }

    // create views
    mViews.emplace_back(mMainView = new CView(*mRenderer, "Main View"));
    if (config.splitView) {
        mViews.emplace_back(mDepthView = new CView(*mRenderer, "Depth View"));
        mViews.emplace_back(mGodView = new GodView(*mRenderer, "God View"));
        mViews.emplace_back(mOrthoView = new CView(*mRenderer, "Shadow View"));
    }
    mViews.emplace_back(mUiView = new CView(*mRenderer, "UI View"));



    mMainView->setCamera(mMainCamera);
   // mMainView->setCameraManipulator(mMainCameraMan);
    if (config.splitView) {
        // Depth view always uses the main camera
        mDepthView->setCamera(mMainCamera);
        //mDepthView->setCameraManipulator(mMainCameraMan);

        // The god view uses the main camera for culling, but the debug camera for viewing
        mGodView->setCamera(mMainCamera);
        mGodView->setGodCamera(mDebugCamera);
        //mGodView->setCameraManipulator(mDebugCameraMan);

        // Ortho view obviously uses an ortho camera
        Camera const* debugDirectionalShadowCamera = mMainView->getView()->getDirectionalShadowCamera();
        if (debugDirectionalShadowCamera) {
            mOrthoView->setCamera(const_cast<Camera*>(debugDirectionalShadowCamera));
        }
    }

    // configure the cameras
    configureCamerasForWindow();

    mMainCamera->lookAt({ 4, 0, -4 }, { 0, 0, -4 }, { 0, 1, 0 });
}

FilamentApp::Window::~Window() {
    mViews.clear();
    utils::EntityManager& em = utils::EntityManager::get();
    for (auto e : mCameraEntities) {
        mFilamentApp->mEngine->destroyCameraComponent(e);
        em.destroy(e);
    }
    mFilamentApp->mEngine->destroy(mRenderer);
    mFilamentApp->mEngine->destroy(mSwapChain);
    SDL_DestroyWindow(mWindow);
    //delete mMainCameraMan;
    //delete mDebugCameraMan;
}

void FilamentApp::Window::mouseDown(int button, ssize_t x, ssize_t y) {
    fixupMouseCoordinatesForHdpi(x, y);
    y = mHeight - y;
    for (auto const& view : mViews) {
        if (view->intersects(x, y)) {
            mMouseEventTarget = view.get();
            view->mouseDown(button, x, y);
            break;
        }
    }
}

void FilamentApp::Window::mouseWheel(ssize_t x) {
    if (mMouseEventTarget) {
        mMouseEventTarget->mouseWheel(x);
    }
    else {
        for (auto const& view : mViews) {
            if (view->intersects(mLastX, mLastY)) {
                view->mouseWheel(x);
                break;
            }
        }
    }
}

void FilamentApp::Window::mouseUp(ssize_t x, ssize_t y) {
    fixupMouseCoordinatesForHdpi(x, y);
    if (mMouseEventTarget) {
        y = mHeight - y;
        mMouseEventTarget->mouseUp(x, y);
        mMouseEventTarget = nullptr;
    }
}

void FilamentApp::Window::mouseMoved(ssize_t x, ssize_t y) {
    fixupMouseCoordinatesForHdpi(x, y);
    y = mHeight - y;
    if (mMouseEventTarget) {
        mMouseEventTarget->mouseMoved(x, y);
    }
    mLastX = x;
    mLastY = y;
}

void FilamentApp::Window::keyDown(SDL_Scancode key) {
    auto& eventTarget = mKeyEventTarget[key];

    // keyDown events can be sent multiple times per key (for key repeat)
    // If this key is already down, do nothing.
    if (eventTarget) {
        return;
    }

    // Decide which view will get this key's corresponding keyUp event.
    // If we're currently in a mouse grap session, it should be the mouse grab's target view.
    // Otherwise, it should be whichever view we're currently hovering over.
    CView* targetView = nullptr;
    if (mMouseEventTarget) {
        targetView = mMouseEventTarget;
    }
    else {
        for (auto const& view : mViews) {
            if (view->intersects(mLastX, mLastY)) {
                targetView = view.get();
                break;
            }
        }
    }

    if (targetView) {
        targetView->keyDown(key);
        eventTarget = targetView;
    }
}

void FilamentApp::Window::keyUp(SDL_Scancode key) {
    auto& eventTarget = mKeyEventTarget[key];
    if (!eventTarget) {
        return;
    }
    eventTarget->keyUp(key);
    eventTarget = nullptr;
}

void FilamentApp::Window::fixupMouseCoordinatesForHdpi(ssize_t& x, ssize_t& y) const {
    int dw, dh, ww, wh;
    SDL_GL_GetDrawableSize(mWindow, &dw, &dh);
    SDL_GetWindowSize(mWindow, &ww, &wh);
    x = x * dw / ww;
    y = y * dh / wh;
}

void FilamentApp::Window::resize() {
    void* nativeWindow = ::getNativeWindow(mWindow);

#if defined(__APPLE__)

    if (mBackend == filament::Engine::Backend::METAL) {
        resizeMetalLayer(nativeWindow);
    }

#if defined(FILAMENT_DRIVER_SUPPORTS_VULKAN)
    if (mBackend == filament::Engine::Backend::VULKAN) {
        resizeMetalLayer(nativeWindow);
    }
#endif

#endif

    configureCamerasForWindow();

    // Call the resize callback, if this FilamentApp has one. This must be done after
    // configureCamerasForWindow, so the viewports are correct.
    if (mFilamentApp->mResize) {
        mFilamentApp->mResize(mFilamentApp->mEngine, mMainView->getView());
    }
}

void FilamentApp::Window::configureCamerasForWindow() {
    float dpiScaleX = 1.0f;
    float dpiScaleY = 1.0f;

    // If the app is not headless, query the window for its physical & virtual sizes.
    if (!mIsHeadless) {
        uint32_t width, height;
        SDL_GL_GetDrawableSize(mWindow, (int*)&width, (int*)&height);
        mWidth = (size_t)width;
        mHeight = (size_t)height;

        int virtualWidth, virtualHeight;
        SDL_GetWindowSize(mWindow, &virtualWidth, &virtualHeight);
        dpiScaleX = (float)width / virtualWidth;
        dpiScaleY = (float)height / virtualHeight;
    }

    const uint32_t width = mWidth;
    const uint32_t height = mHeight;

    const float3 at(0, 0, -4);
    const double ratio = double(height) / double(width);
    const int sidebar = mFilamentApp->mSidebarWidth * dpiScaleX;

    const bool splitview = mViews.size() > 2;

    // To trigger a floating-point exception, users could shrink the window to be smaller than
    // the sidebar. To prevent this we simply clamp the width of the main viewport.
    const uint32_t mainWidth = splitview ? width : std::max(1, (int)width - sidebar);

    double near = mFilamentApp->mCameraNear;
    double far = mFilamentApp->mCameraFar;
    if (mMainView->getView()->getStereoscopicOptions().enabled) {
        mat4 projections[4];
        projections[0] = Camera::projection(mFilamentApp->mCameraFocalLength, 1.0, near, far);
        projections[1] = projections[0];
        // simulate foveated rendering
        projections[2] = Camera::projection(mFilamentApp->mCameraFocalLength * 2.0, 1.0, near, far);
        projections[3] = projections[2];
        mMainCamera->setCustomEyeProjection(projections, 4, projections[0], near, far);
    }
    else {
        mMainCamera->setLensProjection(mFilamentApp->mCameraFocalLength, 1.0, near, far);
    }
    mDebugCamera->setProjection(45.0, double(width) / height, 0.0625, 4096, Camera::Fov::VERTICAL);

    auto aspectRatio = double(mainWidth) / height;
    if (mMainView->getView()->getStereoscopicOptions().enabled) {
        const int ec = mConfig.stereoscopicEyeCount;
        aspectRatio = double(mainWidth) / ec / height;
    }
    mMainCamera->setScaling({ 1.0 / aspectRatio, 1.0 });

    // We're in split view when there are more views than just the Main and UI views.
    if (splitview) {
        uint32_t vpw = width / 2;
        uint32_t vph = height / 2;
        mMainView->setViewport({ 0,            0, vpw,         vph });
        mDepthView->setViewport({ int32_t(vpw),            0, width - vpw, vph });
        mGodView->setViewport({ int32_t(vpw), int32_t(vph), width - vpw, height - vph });
        mOrthoView->setViewport({ 0, int32_t(vph), vpw,         height - vph });
    }
    else {
        mMainView->setViewport({ sidebar, 0, mainWidth, height });
    }
    mUiView->setViewport({ 0, 0, width, height });
}

// ------------------------------------------------------------------------------------------------

FilamentApp::CView::CView(Renderer& renderer, std::string name)
    : engine(*renderer.getEngine()), mName(name) {
    view = engine.createView();
    view->setName(name.c_str());
}

FilamentApp::CView::~CView() {
    engine.destroy(view);
}

void FilamentApp::CView::setViewport(filament::Viewport const& viewport) {
    mViewport = viewport;
    view->setViewport(viewport);

}

void FilamentApp::CView::mouseDown(int button, ssize_t x, ssize_t y) {

}

void FilamentApp::CView::mouseUp(ssize_t x, ssize_t y) {

}

void FilamentApp::CView::mouseMoved(ssize_t x, ssize_t y) {

}

void FilamentApp::CView::mouseWheel(ssize_t x) {

}


void FilamentApp::CView::keyUp(SDL_Scancode scancode) {

}

void FilamentApp::CView::keyDown(SDL_Scancode scancode) {

}

bool FilamentApp::CView::intersects(ssize_t x, ssize_t y) {
    if (x >= mViewport.left && x < mViewport.left + mViewport.width)
        if (y >= mViewport.bottom && y < mViewport.bottom + mViewport.height)
            return true;

    return false;
}



void FilamentApp::CView::setCamera(Camera* camera) {
    view->setCamera(camera);
}

void FilamentApp::GodView::setGodCamera(Camera* camera) {
    getView()->setDebugCamera(camera);
}
