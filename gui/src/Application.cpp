// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include <iostream>
#include <stdio.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <gui/Application.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <soloud_wav.h>
#include <conio.h>
#include <windows.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <locale>
#include <codecvt>

#ifdef __MINGW32__

#ifdef DEFINE_PROPERTYKEY
#undef DEFINE_PROPERTYKEY
#endif

/* clang-format off */

#define DEFINE_PROPERTYKEY(id, a, b, c, d, e, f, g, h, i, j, k, l) \
	const PROPERTYKEY id = { { a,b,c, { d,e,f,g,h,i,j,k, } }, l };
DEFINE_PROPERTYKEY(PKEY_Device_FriendlyName, \
	0xa45c254e, 0xdf1c, 0x4efd, 0x80, \
	0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 14);

/* clang-format on */

#else

#include <functiondiscoverykeys_devpkey.h>

#endif

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static GGgui::Application* s_Instance = nullptr;

extern bool g_ApplicationRunning;


static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void get_resolution(int &window_width, int &window_height) {
    const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    window_width = mode->width;
    window_height = mode->height;
}
//static void window_focus_callback(GLFWwindow* window, int focused)
//{
//    printf("%0.2f Window %s\n",
//           glfwGetTime(),
//           focused ? "focused" : "defocused");
//}
namespace GGgui {
    const char* const logo[] =
            {

                    "................",
                    "................",
                    "...0000..0000...",
                    "...0.....0......",
                    "...0.....0000...",
                    "...0........0...",
                    "...0000..0000...",
                    "................",
                    "................",
                    "..............0.",
                    "...0........0...",
                    "....0.....0.....",
                    ".....0..0.......",
                    "......0.........",
                    "................",
                    "................"
            };
    const unsigned char icon_colors[5][4] =
            {
                    {   0,   0,   0, 255 }, // black
                    { 255,   0,   0, 255 }, // red
                    {   0, 255,   0, 255 }, // green
                    {   0,   0, 255, 255 }, // blue
                    { 255, 255, 255, 255 }  // white
            };
    static void set_icon(GLFWwindow* window, int icon_color)
    {
        int x, y;
        unsigned char pixels[16 * 16 * 4];
        unsigned char* target = pixels;
        GLFWimage img = { 16, 16, pixels };

        for (y = 0;  y < img.width;  y++)
        {
            for (x = 0;  x < img.height;  x++)
            {
                if (logo[y][x] == '0')
                    memcpy(target, icon_colors[icon_color], 4);
                else
                    memset(target, 0, 4);

                target += 4;
            }
        }

        glfwSetWindowIcon(window, 1, &img);
    }
    Application::Application(const ApplicationSpecification &specification)
            : m_Specification(specification) {
        s_Instance = this;

        Init();
    }

    Application::~Application() {
        Shutdown();

        s_Instance = nullptr;
    }

    Application &Application::Get() {
        return *s_Instance;
    }

    void Application::Init() {
        // Set locale to support UTF-8
		//std::locale::global(std::locale("en_US.UTF-8"));

        // init logger
        auto logger = spdlog::basic_logger_mt("file_logger", "logs.txt");
        spdlog::set_default_logger(logger);

        // init soloud
        _soloud_core.init();
        for(int i = 0; i < 5; i++){
            _wav_ass.emplace(std::make_shared<SoLoud::Wav>());
        }

        // Setup window
        glfwSetErrorCallback(glfw_error_callback);
        if (!glfwInit()) {
//            std::cerr << "Could not initalize GLFW!\n";
            spdlog::error("Could not initalize GLFW!");
            return;
        }

        // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
        // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
        // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
        // GL 3.0 + GLSL 130
        const char *glsl_version = "#version 130";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif
        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, 1);
        glfwWindowHint(GLFW_FLOATING, 1);
//        glfwWindowHint(GLFW_MAXIMIZED, 1);
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
//        glfwWindowHint(GLFW_MOUSE_PASSTHROUGH, GLFW_TRUE);


        // Create window with graphics context
        m_WindowHandle = glfwCreateWindow(m_Specification.Width, m_Specification.Height, m_Specification.Name.c_str(),
                                          NULL, NULL);
        if (m_WindowHandle == NULL) {
            std::cerr << "Could not create GLFW window!\n";
            return;
        }
        glfwMakeContextCurrent(m_WindowHandle);
        glfwSwapInterval(1);

//        glfwHideWindow(m_WindowHandle);
//        glfwSetWindowFocusCallback(m_WindowHandle, window_focus_callback);

        set_icon(m_WindowHandle, 3);


        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
		io.IniFilename = nullptr; // Prevents ImGui from saving settings to a file
        (void) io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

        // do not auto merge, so when the glfw enables mouse passthrough, the window inside the main viewport wont be unclickable
        io.ConfigViewportsNoAutoMerge = true;
//        io.ConfigWindowsResizeFromEdges = true;
//        io.ConfigViewportsNoTaskBarIcon = true;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle &style = ImGui::GetStyle();
//        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
//            style.WindowRounding = 0.0f;
//            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
//        }
        SetupStyle(false, 1.0f);


        // Setup Platform/Renderer src
        ImGui_ImplGlfw_InitForOpenGL(m_WindowHandle, true);
        ImGui_ImplOpenGL3_Init(glsl_version);

        // Load default font
        ImFontConfig fontConfig;
        fontConfig.FontDataOwnedByAtlas = false;
        fontConfig.OversampleH = 2;  // Lower oversampling to reduce size
        fontConfig.OversampleV = 2;
        fontConfig.PixelSnapH = true;  // Align to pixels for smaller texture

        io.Fonts->AddFontFromFileTTF("NotoSansCJKtc-Regular.otf", 20.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());


        // Load Fonts
        // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
        // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
        // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
        // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
        // - Read 'docs/FONTS.md' for more instructions and details.
        // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
        //io.Fonts->AddFontDefault();
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
        //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
        //IM_ASSERT(font != NULL);
    }

    void Application::Shutdown() {
        _soloud_core.deinit();

        for (auto &layer: m_LayerStack)
            layer->OnDetach();

        m_LayerStack.clear();

        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(m_WindowHandle);
        glfwTerminate();

        g_ApplicationRunning = false;
    }

    void Application::Run() {
        m_Running = true;

        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 0.0f);
        auto &io = ImGui::GetIO();

        spdlog::warn("Application successfully launched.");
        // Main loop
        while (!glfwWindowShouldClose(m_WindowHandle) && m_Running) {
            int monitor_width, monitor_height;
//            glfwMaximizeWindow(m_WindowHandle);
            get_resolution(monitor_width, monitor_height);
            if(monitor_height != _monitor_height || monitor_width != _monitor_width){
                glfwSetWindowSize(m_WindowHandle, monitor_width - 1, monitor_height - 1);
                glfwSetWindowPos(m_WindowHandle,0,0);
                _monitor_width = monitor_width;
                _monitor_height = monitor_height;
                _monitor_aspect_ratio = 1.0f * _monitor_width / _monitor_height;
            }


            // Poll and handle events (inputs, window resize, etc.)
            // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
            // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
            // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
            // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
            glfwPollEvents();




            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // To set mouse paththrough, we need to set viewport flags, or set glfw hint here, cuz ImGui_ImplGlfw_NewFrame()
            // override that hint.
            ImGuiViewport *viewport = ImGui::GetMainViewport();
            viewport->Flags |= ImGuiViewportFlags_NoInputs;

            // Render custom UI
            for (auto &layer: m_LayerStack)
                layer->OnUpdate(m_TimeStep);

            {
//                static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

                // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
                // because it would be confusing to have two docking targets within each others.
//                ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
//                if (m_MenubarCallback)
//                    window_flags |= ImGuiWindowFlags_MenuBar;

//                const ImGuiViewport* viewport = ImGui::GetMainViewport();
//                ImGui::SetNextWindowPos(viewport->WorkPos);
//                ImGui::SetNextWindowSize(viewport->WorkSize);
//                ImGui::SetNextWindowViewport(viewport->ID);
//                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
//                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
//                window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize ;
//                window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

                // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
                // and handle the pass-thru hole, so we ask Begin() to not render a background.
//                if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
//                    window_flags |= ImGuiWindowFlags_NoBackground;

                // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
                // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
                // all active windows docked into it will lose their parent and become undocked.
                // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
                // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
//                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
//                ImGui::Begin("DockSpace Demo", nullptr, window_flags);
//                ImGui::PopStyleVar();

//                ImGui::PopStyleVar(2);

                // Submit the DockSpace
//                ImGuiIO& io = ImGui::GetIO();
//                if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
//                {
//                    ImGuiID dockspace_id = ImGui::GetID("VulkanAppDockspace");
//                    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
//                }

                if (m_MenubarCallback) {
                    if (ImGui::BeginMenuBar()) {
                        m_MenubarCallback();
                        ImGui::EndMenuBar();
                    }
                }

                for (auto &layer: m_LayerStack)
                    layer->OnUIRender();

//                ImGui::End();
            }


            // Rendering
            ImGui::Render();
            int display_w, display_h;
            glfwGetFramebufferSize(m_WindowHandle, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
//            glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
            glClearColor(0, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            // Update and Render additional Platform Windows
            // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
            //  For this specific demo gui we could also call glfwMakeContextCurrent(window) directly)
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
                GLFWwindow *backup_current_context = glfwGetCurrentContext();
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                glfwMakeContextCurrent(backup_current_context);
            }

            glfwSwapBuffers(m_WindowHandle);


            // Update time
            float time = GetTime();
            m_FrameTime = time - m_LastFrameTime;
            m_TimeStep = glm::min<float>(m_FrameTime, 0.0333f);
            m_LastFrameTime = time;
            m_SmoothedFrameTime = smooth_ratio * m_SmoothedFrameTime + (1 - smooth_ratio) * m_FrameTime;
            m_SmoothedFps = (uint32_t) (1.0f / m_SmoothedFrameTime);

        }

    }

    void Application::Close() {
        m_Running = false;
        spdlog::warn("Application exit.");
    }

    float Application::GetTime() {
        return (float) glfwGetTime();
    }

	void Application::ReinitAudioDevice()
	{
        _soloud_core.deinit();
        _soloud_core.init();
	}

	void Application::ListAudioDevices(std::vector<std::string>& out_audio_devices)
    {
        out_audio_devices.clear();

        CoInitialize(NULL);
        IMMDeviceEnumerator* enumerator = NULL;
        CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&enumerator);

        IMMDeviceCollection* devices = NULL;
        enumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &devices);

        UINT count;
        devices->GetCount(&count);

        for (UINT i = 0; i < count; i++) {
            IMMDevice* device;
            devices->Item(i, &device);

            IPropertyStore* props;
            device->OpenPropertyStore(STGM_READ, &props);

            PROPVARIANT varName;
            PropVariantInit(&varName);
            props->GetValue(PKEY_Device_FriendlyName, &varName);

            std::string deviceName = WideToUTF8(varName.pwszVal);
            spdlog::info("Get device {}: {}", i, deviceName);
            out_audio_devices.push_back(deviceName);

            PropVariantClear(&varName);
            props->Release();
            device->Release();
        }

        devices->Release();
        enumerator->Release();
        CoUninitialize();
    }

	void Application::SetAudioDevices(const int& audio_device_index)
	{
        // TODO: soloud does not provide api to change audio device.
        _soloud_core.init();
	}

	void Application::PlayAudio(std::string_view audio_path, float volume) {
        assert(volume >= 0.0f && volume <= 2.0f);

        auto as = _wav_ass.front();
        _wav_ass.pop();
        _wav_ass.push(as);

        as->load(audio_path.data());
        _soloud_core.play(*as, volume);
    }

    void Application::SetupStyle(bool dark, float alpha) {
        ImGuiStyle& style = ImGui::GetStyle();

        // light style from Pacôme Danhiez (user itamago) https://github.com/ocornut/imgui/pull/511#issuecomment-175719267
        style.Alpha = 1.0f;
        style.FrameRounding = 3.0f;
        style.Colors[ImGuiCol_Text]                  = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
        style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.94f, 0.94f, 0.94f, 0.94f);
//        style.Colors[ImGuiCol_ChildWindowBg]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style.Colors[ImGuiCol_PopupBg]               = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
        style.Colors[ImGuiCol_Border]                = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
        style.Colors[ImGuiCol_BorderShadow]          = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
        style.Colors[ImGuiCol_FrameBg]               = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
        style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
        style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
        style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
        style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
        style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
        style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
        style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
//        style.Colors[ImGuiCol_ComboBg]               = ImVec4(0.86f, 0.86f, 0.86f, 0.99f);
        style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
        style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_Button]                = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
        style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_Header]                = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
        style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
        style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
//        style.Colors[ImGuiCol_Column]                = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
//        style.Colors[ImGuiCol_ColumnHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
//        style.Colors[ImGuiCol_ColumnActive]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
        style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
        style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
//        style.Colors[ImGuiCol_CloseButton]           = ImVec4(0.59f, 0.59f, 0.59f, 0.50f);
//        style.Colors[ImGuiCol_CloseButtonHovered]    = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
//        style.Colors[ImGuiCol_CloseButtonActive]     = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
        style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
        style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
//        style.Colors[ImGuiCol_ModalWindowDarkening]  = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

        if( dark )
        {
            for (int i = 0; i <= ImGuiCol_COUNT; i++)
            {
                ImVec4& col = style.Colors[i];
                float H, S, V;
                ImGui::ColorConvertRGBtoHSV( col.x, col.y, col.z, H, S, V );

                if( S < 0.1f )
                {
                    V = 1.0f - V;
                }
                ImGui::ColorConvertHSVtoRGB( H, S, V, col.x, col.y, col.z );
                if( col.w < 1.00f )
                {
                    col.w *= alpha;
                }
            }
        }
        else
        {
            for (int i = 0; i <= ImGuiCol_COUNT; i++)
            {
                ImVec4& col = style.Colors[i];
                if( col.w < 1.00f )
                {
                    col.x *= alpha;
                    col.y *= alpha;
                    col.z *= alpha;
                    col.w *= alpha;
                }
            }
        }
    }

	std::string Application::WideToUTF8(const std::wstring& wstr)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		return converter.to_bytes(wstr);
	}

}