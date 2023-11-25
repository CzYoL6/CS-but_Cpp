//
// Created by zaiyichen on 2023/11/26.
//
#include <app/setting_window.h>

void SettingWindow::OnUIRender()
{
    // make sure the window is on the top
    auto viewport = ImGui::GetWindowViewport();
    viewport->Flags |= ImGuiViewportFlags_TopMost;

    auto& app = GGgui::Application::Get();

    ImGui::Begin("Hello", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking);

    if(ImGui::Button("Close")){
        app.Close();
    }

    ImGui::End();

//		ImGui::ShowDemoWindow();
}
#include "app/setting_window.h"
