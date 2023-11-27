//
// Created by zaiyichen on 2023/11/26.
//
#include <app/SettingWindow.h>
#include <app/KillEffectWindow.h>
#include<Input/Input.h>

SettingWindow* SettingWindow::_instance = nullptr;

void SettingWindow::OnUIRender()
{


    auto& app = GGgui::Application::Get();
//    static bool show = true;
//    std::cout << GGgui::Input::IsKeyDown(GGgui::KeyCode::A) << '\n';
//    if(ImGui::IsKeyDown(GLFW_KEY_F6)) show = !show;

    Settings& settings = SettingWindow::GetInstance().settings();
    if(show) {
        ImGui::SetNextWindowSize({600, 0});
        ImGui::Begin("CS but Valorant", nullptr,
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking);
//        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, 5);
        ImGui::GetWindowViewport()->Flags |= ImGuiViewportFlags_TopMost;

        if (load_complete) {
            static bool once = false;
            if (!once) {
                KillEffectWindow::GetInstance().Show();
                glfwShowWindow(app.window_handle());
                once = true;
            }

            ImGui::SameLine();
            if (ImGui::Button("Test")) {
                KillEffectWindow::GetInstance().AddContinuousKillCount_ThreadSafe(1);
                KillEffectWindow::GetInstance().ShowContinuousKillEffect_ThreadSafe();
            }

            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)  ImColor::HSV(54, 0.94f, 1.0f));
            if (ImGui::Button("Hide")) {
                show = false;
            }
            ImGui::PopStyleColor(1);

            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(346, 0.96f, 0.82f));
            if (ImGui::Button("Close")) {
                app.Close();
            }
            ImGui::PopStyleColor(1);

            ImGui::Separator();
            ////////////////////////////////////////////////////////////////////////////////////////
            ImGui::Spacing();
            ImGui::Text("Effect");
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
            if (settings.kill_accumulate_method == 1)
                ImGui::BeginDisabled();
            ImGui::InputFloat("MaxContinuousKillTimeSecondInputFloat", &settings.max_continuous_kill_time_sec, 0.2f,
                              1.0f,
                              "%.1f");
            if (settings.kill_accumulate_method == 1)
                ImGui::EndDisabled();
            ImGui::Checkbox("ShowEffectWhenSpectatingCheckbox", &settings.show_effect_when_spectating);
            ////////////////////////////////////////////////////////////////////////////////////////////


//        ImGui::PopStyleVar();
        } else {
            ImGui::Text("Loading Assets...");
            ImGui::ProgressBar(assets_load_progress);
        }
        ImGui::End();
    }
//    ImGui::ShowDemoWindow();
}

void SettingWindow::OnAttach() {
    Layer::OnAttach();
    const auto& app = GGgui::Application::Get();
    glfwSetWindowIconifyCallback(app.window_handle(), [](GLFWwindow* window, int iconified){
        if(!SettingWindow::GetInstance().show) {
            SettingWindow::GetInstance().show = true;

//            std::cout << "focused" << '\n';
        }

    });

}

#include "app/SettingWindow.h"
