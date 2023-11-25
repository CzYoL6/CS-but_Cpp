#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>

#include <imgui/imgui.h>
#include <GLFW/glfw3.h>
#include <gui/Image.h>
#include <gui/Layer.h>

struct GLFWwindow;

namespace GGgui {

    struct ApplicationSpecification
    {
        std::string Name = "GGgui App";
        uint32_t Width = 1600;
        uint32_t Height = 900;
    };

    class Application
    {
    public:
        Application(const ApplicationSpecification& applicationSpecification = ApplicationSpecification());
        ~Application();

        static Application& Get();

        void Run();
        void SetMenubarCallback(const std::function<void()>& menubarCallback) { m_MenubarCallback = menubarCallback; }

        template<typename T>
        void PushLayer()
        {
            static_assert(std::is_base_of<Layer, T>::value, "Pushed type is not subclass of Layer!");
            m_LayerStack.emplace_back(std::make_shared<T>())->OnAttach();
        }

        void PushLayer(const std::shared_ptr<Layer>& layer) { m_LayerStack.emplace_back(layer); layer->OnAttach(); }

        void Close();

        float GetTime();
        float GetSmoothedFps(){return m_SmoothedFps;}
        GLFWwindow* GetWindowHandle() const { return m_WindowHandle; }

    private:
        void Init();
        void Shutdown();
    private:
        ApplicationSpecification m_Specification;
        GLFWwindow* m_WindowHandle = nullptr;
        bool m_Running = false;

        float m_TimeStep = 0.0f;
        float m_FrameTime = 0.0f;
        float m_LastFrameTime = 0.0f;
        const float smooth_ratio = 0.8f;
        float m_SmoothedFrameTime = 0.0f;
        uint32_t m_SmoothedFps = 0;

        std::vector<std::shared_ptr<Layer>> m_LayerStack;
        std::function<void()> m_MenubarCallback;
    };

    // Implemented by CLIENT
    Application* CreateApplication(int argc, char** argv);
}