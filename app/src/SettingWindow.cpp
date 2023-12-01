//
// Created by zaiyichen on 2023/11/26.
//
#include <app/SettingWindow.h>
#include <app/KillEffectWindow.h>
#include<Input/Input.h>
#include <spdlog/spdlog.h>
#include <windows.h>
#include <psapi.h>

SettingWindow* SettingWindow::_instance = nullptr;

void SettingWindow::OnUIRender()
{


    auto& app = GGgui::Application::Get();
//    static bool show = true;
//    std::cout << GGgui::Input::IsKeyDown(GGgui::KeyCode::A) << '\n';
//    if(ImGui::IsKeyDown(GLFW_KEY_F6)) show = !show;

    Settings& settings = SettingWindow::GetInstance().settings();
    if(show) {
//        if(true) {
        ImGui::SetNextWindowSize({600, 0});
        ImGui::Begin("CS but Valorant v1.1", nullptr,
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking);
//        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, 5);
        ImGui::GetWindowViewport()->Flags |= ImGuiViewportFlags_TopMost;

        if (load_complete) {
            static bool once = false;
            if (!once) {
                KillEffectWindow::GetInstance().Show();
                glfwShowWindow(app.GetWindowHandle());
                once = true;
            }

            ImGui::SameLine();
            if (ImGui::Button("Test")) {
                KillEffectWindow::GetInstance().ShowRoundKillEffect(1);
            }

            ImGui::SameLine();

            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(346, 0.96f, 0.82f));
            if (ImGui::Button("Close")) {
                app.Close();
            }
            ImGui::PopStyleColor(1);

            ImGui::SameLine();
            ImGui::Text("Use Ctrl+F12 to show and hide this window.");
            ImGui::Separator();

            ImGui::Text("Memory consumption");
            auto mem = get_memory_consumption();
            ImGui::Text("Virtual Memory: %d MB", mem.first);
            ImGui::Text("Physical Memory: %d MB", mem.second);

            ImGui::Separator();
            ////////////////////////////////////////////////////////////////////////////////////////
            ImGui::Spacing();
            ImGui::Text("Offset");
            ImGui::DragInt("Offset X", &settings.offset_x);
            ImGui::DragInt("Offset Y", &settings.offset_y);

            ImGui::Separator();

            ImGui::Text("Asset Quality");
            ImGui::RadioButton("Medium", &settings.asset_quality, 0);
            ImGui::SameLine();
            ImGui::RadioButton("High", &settings.asset_quality, 1);

            const char *framerates[] = {"60fps", "120fps"};
            ImGui::Combo("Framerate", &settings.framerate, framerates, IM_ARRAYSIZE(framerates));
            if(ImGui::Button("Reload Assets")){
                spdlog::warn("Reloading Assets");
                KillEffectWindow::GetInstance().LoadAssets();
            }


            //////////////////////////////////////////////////////////////////////////////////////////
            ImGui::Separator();
            ImGui::Text("Other");
            ImGui::Checkbox("Only show effect when I'm playing(disable when spectating)", &settings.only_show_effect_when_im_playing);
            if(!settings.only_show_effect_when_im_playing)
                ImGui::BeginDisabled();
            char steamid[1024];
            memcpy(steamid, settings.steamid.c_str(),settings.steamid.size());
            steamid[settings.steamid.size()] = '\0';
            ImGui::InputText("steamid", steamid, sizeof(steamid));
            settings.steamid = steamid;
            if(!settings.only_show_effect_when_im_playing)
                ImGui::EndDisabled();

            //TODO: add volume control

            //TODO: add size setting
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

void SettingWindow::OnUpdate(float ts) {
    Layer::OnUpdate(ts);

}

void SettingWindow::OnAttach() {
    Layer::OnAttach();
    _hotkey_capture_thread = std::thread([this]{
        RegisterHotKey(NULL, 1, MOD_CONTROL | 0X4000, VK_F12) ; // Ctrl F12
        MSG msg;
        auto& app = GGgui::Application::Get();
        while(app.IsRunning()) {
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0) {
                if (msg.wParam == 1) {
//                    std::cout << "hot key" << std::endl;
                    show = !show;
                }
            }
        }
    });(void)_hotkey_capture_thread;
}

void SettingWindow::OnDetach() {
    Layer::OnDetach();
    if(_hotkey_capture_thread.joinable()) _hotkey_capture_thread.join();
}

std::pair<int, int> SettingWindow::get_memory_consumption(){
    static HANDLE process = GetCurrentProcess();
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(process, reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc), sizeof(pmc))) {
        return {pmc.PrivateUsage / (1024 * 1024), pmc.WorkingSetSize / (1024 * 1024)  };
    } else {
        spdlog::error("Error getting process memory information. Error code: {}",GetLastError() );
        exit(-1);
    }
}