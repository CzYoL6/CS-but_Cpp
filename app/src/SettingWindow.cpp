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

    if(show) {
//        if(true) {
        ImGui::SetNextWindowSize({600, 0});
        ImGui::Begin("CS but Valorant v1.1", nullptr,
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking);
//        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, 5);
        ImGui::GetWindowViewport()->Flags |= ImGuiViewportFlags_TopMost;

        static bool pre_load_complete = false;

        if (load_complete) {
            if(!pre_load_complete) {
                pre_load_complete = true;
                KillEffectWindow::GetInstance().Show();
//                glfwShowWindow(app.GetWindowHandle());
//                glfwMaximizeWindow(app.GetWindowHandle());
//                glfwFocusWindow(app.GetWindowHandle());
            }


            ImGui::SameLine();
            if (ImGui::Button("Test")) {
                KillEffectWindow::GetInstance().ShowRoundKillEffect(5);
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
            ImGui::DragInt("Offset X", &_settings.offset_x);
            ImGui::DragInt("Offset Y", &_settings.offset_y);

            ImGui::Separator();

            ImGui::Combo("Asset Preset", &_settings.asset_preset,_assets.asset_names, IM_ARRAYSIZE(_assets.asset_names));
            ImGui::Separator();

            ImGui::Text("Asset Quality");
            ImGui::RadioButton("Medium", &_settings.asset_quality, 0);
            ImGui::SameLine();
            ImGui::RadioButton("High", &_settings.asset_quality, 1);

            const char *framerates[] = {"60fps", "120fps"};
            ImGui::Combo("Framerate", &_settings.framerate, framerates, IM_ARRAYSIZE(framerates));
            if(ImGui::Button("Reload Assets")){
                spdlog::warn("Reloading Assets");
                KillEffectWindow::GetInstance().LoadAssets();
            }


            //////////////////////////////////////////////////////////////////////////////////////////
            ImGui::Separator();
            ImGui::Text("Other");
            ImGui::Checkbox("Only show effect when I'm playing(disable when spectating)", &_settings.only_show_effect_when_im_playing);
            if(!_settings.only_show_effect_when_im_playing)
                ImGui::BeginDisabled();
            char steamid[1024];
            memcpy(steamid, _settings.steamid.c_str(),_settings.steamid.size());
            steamid[_settings.steamid.size()] = '\0';
            ImGui::InputText("steamid", steamid, sizeof(steamid));
            _settings.steamid = steamid;
            if(!_settings.only_show_effect_when_im_playing)
                ImGui::EndDisabled();

            //TODO: add volume control

            //TODO: add size setting
            ////////////////////////////////////////////////////////////////////////////////////////////


//        ImGui::PopStyleVar();
        } else {
            if(pre_load_complete && !load_complete){
                pre_load_complete = false;
            }
            KillEffectWindow::GetInstance().Hide();
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