//
// Created by zaiyichen on 2023/11/26.
//
#include <app/SettingWindow.h>
#include <app/KillEffectWindow.h>

void SettingWindow::OnUIRender()
{
    // make sure the window is on the top

    auto& app = GGgui::Application::Get();

    ImGui::Begin("Hello", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking);
    auto viewport = ImGui::GetWindowViewport();
    viewport->Flags |= ImGuiViewportFlags_TopMost;

    if(ImGui::Button("Close")){
        app.Close();
    }

    if(ImGui::Button("Kill")){
        KillEffectWindow::GetInstance().AddKillCount(1);
        KillEffectWindow::GetInstance().ShowKillEffect();
    }

    ImGui::End();

//		ImGui::ShowDemoWindow();
}
#include "app/SettingWindow.h"
