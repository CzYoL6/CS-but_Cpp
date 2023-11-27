//
// Created by zaiyichen on 2023/11/26.
//
#include <app/SettingWindow.h>
#include <app/KillEffectWindow.h>
#include<Input/Input.h>


void SettingWindow::OnUIRender()
{
    // make sure the window is on the top

    auto& app = GGgui::Application::Get();
//    static bool show = true;
//    std::cout << GGgui::Input::IsKeyDown(GGgui::KeyCode::A) << '\n';
//    if(ImGui::IsKeyDown(GLFW_KEY_F6)) show = !show;

    Settings& settings = Settings::Get();

    if(show) {
        ImGui::SetNextWindowSize({600, 0});
        ImGui::Begin("Hello", nullptr,
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBackground);
//        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, 5);

        auto viewport = ImGui::GetWindowViewport();
        viewport->Flags |= ImGuiViewportFlags_TopMost;

        if(KillEffectWindow::GetInstance().hidden()){
            if(ImGui::Button("ON")){
                KillEffectWindow::GetInstance().Show();
                glfwShowWindow(app.window_handle());
            }
        }
        else {
            if(ImGui::Button("OFF")){
                KillEffectWindow::GetInstance().Hide();
                glfwHideWindow(app.window_handle());
            }
        }
        ImGui::SameLine();
        if(KillEffectWindow::GetInstance().hidden())
            ImGui::BeginDisabled();
        if (ImGui::Button("Test")) {
            KillEffectWindow::GetInstance().AddKillCount_ThreadSafe(1);
            KillEffectWindow::GetInstance().ShowKillEffect_ThreadSafe();
        }
        if(KillEffectWindow::GetInstance().hidden())
            ImGui::EndDisabled();
        ImGui::SameLine();
        if (ImGui::Button("Close")) {
            app.Close();
        }

        ImGui::Separator();
        ////////////////////////////////////////////////////////////////////////////////////////
        ImGui::Spacing();
        ImGui::Text("Effect") ;
            ImGui::DragInt("Offset X", &settings.offset_x);
            ImGui::DragInt("Offset Y", &settings.offset_y);

//            ImGui::Text("Asset Quality");
//            ImGui::RadioButton("Medium", &settings.asset_quality, 0);
//            ImGui::SameLine();
//            ImGui::RadioButton("High", &settings.asset_quality, 1);
//
//            const char *framerates[] = {"60fps", "120fps"};
//            ImGui::Combo("FramerateCombo", &settings.high_framerate, framerates, IM_ARRAYSIZE(framerates));


        //////////////////////////////////////////////////////////////////////////////////////////
        ImGui::Spacing();
        ImGui::Text("Other");
            ImGui::RadioButton("Continuous Kill", &settings.kill_accumulate_method, 0);
            ImGui::SameLine();
            ImGui::RadioButton("Round Total Kill", &settings.kill_accumulate_method, 1);
            if(settings.kill_accumulate_method == 1)
                ImGui::BeginDisabled();
            ImGui::InputFloat("MaxContinuousKillTimeSecondInputFloat", &settings.max_continuous_kill_time_sec, 0.2f,
                              1.0f,
                              "%.1f");
            if(settings.kill_accumulate_method == 1)
                ImGui::EndDisabled();
            ImGui::Checkbox("ShowEffectWhenSpectatingCheckbox", &settings.show_effect_when_spectating);
            ////////////////////////////////////////////////////////////////////////////////////////////


//        ImGui::PopStyleVar();
        ImGui::End();
    }

//    ImGui::ShowDemoWindow();
}

void SettingWindow::OnAttach() {
    Layer::OnAttach();
}

#include "app/SettingWindow.h"
