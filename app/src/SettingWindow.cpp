//
// Created by zaiyichen on 2023/11/26.
//
#include <app/SettingWindow.h>
#include <app/KillEffectWindow.h>
#include<Input/Input.h>
#include <spdlog/spdlog.h>
#include <imgui/imgui_stdlib.h>
#include <windows.h>
#include <psapi.h>
#include <app/WindowsFileDialog.h>

SettingWindow* SettingWindow::_instance = nullptr;

void SettingWindow::OnUIRender()
{


    auto& app = GGgui::Application::Get();

    if(bShow) {
        ImGui::SetNextWindowSize({700, 500});
		ImGui::Begin("CS but v2.0", &bShow,
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize );

        static bool pre_load_complete = false;
        static bool reload = false;

        if (load_complete) {
            if(!pre_load_complete) {
                pre_load_complete = true;
                KillEffectWindow::GetInstance().Show();
            }

			ImGui::Text("System");
			if (ImGui::BeginTabBar("##Tabs1", ImGuiTabBarFlags_None)) {
				if (ImGui::BeginTabItem("Monitor"))
                {
					ImGui::Spacing();
					ImGui::BeginChild("monitor", ImVec2(0, 100), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);

   						ImGui::Text("Memory Consumption");
						ImGui::Spacing();
						auto mem = get_memory_consumption();
						ImGui::Text("Virtual Memory: %d MB", mem.first);
						ImGui::Text("Physical Memory: %d MB", mem.second);

					ImGui::EndChild();
					ImGui::EndTabItem();
                }
				if (ImGui::BeginTabItem("Test")) {
					ImGui::Spacing();
					ImGui::BeginChild("test", ImVec2(0, 100), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);

						static int test_kill_count = 1;
						const int max_kill_count = std::min(SettingWindow::current_asset().max_kill_sound_count, SettingWindow::current_asset().max_kill_banner_count);
						ImGui::DragInt("Test Kill Count", &test_kill_count, 1, 1, max_kill_count);
						if (ImGui::Button("Kill")) {
							KillEffectWindow::GetInstance().ShowRoundKillEffect(test_kill_count);
							if((++test_kill_count) == max_kill_count + 1){
								test_kill_count = 1;
							}
						}
						ImGui::SameLine();
						if(current_asset().enable_headshot) {
							if (ImGui::Button("Headshot")) {
								KillEffectWindow::GetInstance().ShowHeadshotEffect();
							}
						}

					ImGui::EndChild();
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Other")) {
					ImGui::Spacing();
					ImGui::BeginChild("presets", ImVec2(0, 100), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);

						ImGui::Checkbox("Only show effect when I'm playing(disable when spectating)", &_settings.only_show_effect_when_im_playing);
						if(_settings.only_show_effect_when_im_playing) {
							ImGui::InputText("steamid", &_settings.steamid);
						}
					
					ImGui::EndChild();
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}


            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            
			ImGui::Text("Effect");
            if (ImGui::BeginTabBar("##Tabs2", ImGuiTabBarFlags_None)) {
				if (ImGui::BeginTabItem("Presets"))
                {
					ImGui::Spacing();
					ImGui::BeginChild("presets", ImVec2(0, -ImGui::GetFrameHeightWithSpacing() + 10), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);

						if(ImGui::Button("New Copy")){
							// copy the current one
							_assets.asset_configs.push_back( _assets.asset_configs[_settings.asset_preset]);
							_assets.asset_configs[_assets.asset_configs.size() - 1].asset_name =
									std::format("Copy of {}", current_asset().asset_name);
							_assets.asset_configs[_assets.asset_configs.size() - 1].is_custom = true;

							// show the new one
							_settings.asset_preset = _assets.asset_configs.size() - 1;

							_assets.ReassignAssetNames();
						}
						ImGui::SameLine();
						bool tmp = !current_asset().is_custom;
						if(tmp) ImGui::BeginDisabled();
						if(ImGui::Button("Delete")){
							spdlog::info("Delete Asset Preset \"{}\".", current_asset().asset_name);
							_assets.asset_configs.erase(_assets.asset_configs.begin() + _settings.asset_preset);
							_settings.asset_preset = 0;
							_assets.ReassignAssetNames();
						}
						if(tmp) ImGui::EndDisabled();

						ImGui::SameLine();
						ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(255, 255, 0, 255));
						ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 0, 255));
						ImGui::PushID(1);
						if(ImGui::Button("Save & Reload")){
							reload = true;
						}
						ImGui::PopID();
						ImGui::PopStyleColor(2);
						ImGui::SameLine();
						ImGui::Text("Reload assets after any changes are made.");

						ImGui::Combo("Asset Preset", &_settings.asset_preset,_assets.asset_names, _assets.asset_configs.size());
						tmp = !current_asset().is_custom;
						if(tmp){
							ImGui::BeginDisabled();
						}

						ImGui::InputText("Asset Name", &current_asset().asset_name);
						ImGui::SameLine();
						if(ImGui::Button("Save")){
							_assets.ReassignAssetNames();
						}

						ImGui::BeginDisabled();
						ImGui::InputText("Kill Banner Folder", &current_asset().kill_banner_asset_folder);
						ImGui::EndDisabled();
						ImGui::SameLine();
						ImGui::PushID(1);
						if(ImGui::Button("Select")){
							FileDialog::openFolderDialog(current_asset().kill_banner_asset_folder);
						}
						ImGui::PopID();
						ImGui::DragInt("Max Kill Count Of Banner", &current_asset().max_kill_banner_count, 1, 1, 100);
						ImGui::BeginDisabled();
						ImGui::InputText("Kill Sound Folder", &current_asset().kill_sound_asset_folder);
						ImGui::EndDisabled();
						ImGui::SameLine();
						ImGui::PushID(2);
						if(ImGui::Button("Select")){
							FileDialog::openFolderDialog(current_asset().kill_sound_asset_folder);
						}
						ImGui::PopID();
						ImGui::DragInt("Max Kill Count Of Sound", &current_asset().max_kill_sound_count, 1, 1, 100);


						ImGui::Checkbox("Enable Headshot", &current_asset().enable_headshot);
						if(current_asset().enable_headshot) {
							ImGui::BeginDisabled();
							ImGui::InputText("Headshot Banner Folder", &current_asset().headshot_banner_folder);
							ImGui::EndDisabled();

							ImGui::SameLine();
							ImGui::PushID(3);
							if (ImGui::Button("Select")) {
								FileDialog::openFolderDialog(current_asset().headshot_banner_folder);
							}
							ImGui::PopID();

							ImGui::BeginDisabled();
							ImGui::InputText("Headshot Sound File", &current_asset().headshot_sound_file);
							ImGui::EndDisabled();

							ImGui::SameLine();
							ImGui::PushID(4);
							if (ImGui::Button("Select")) {
								FileDialog::openFileDialog(current_asset().headshot_sound_file, "Wave Files\0*.wav\0All Files\0*.*\0");
							}
							ImGui::PopID();
						}
						if(tmp){
							ImGui::EndDisabled();
						}

						ImGui::Spacing();
						ImGui::Separator();
						ImGui::Spacing();

						ImGui::Text("Banner Asset Quality");
						ImGui::RadioButton("Medium", &_settings.asset_quality, 0);
						ImGui::SameLine();
						ImGui::RadioButton("High", &_settings.asset_quality, 1);

						const char *framerates[] = {"60fps", "120fps"};
						ImGui::Combo("Framerate", &_settings.framerate, framerates, IM_ARRAYSIZE(framerates));

						ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(255, 255, 0, 255));
						ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 0, 255));
						ImGui::PushID(2);
						if(ImGui::Button("Save & Reload")){
							reload = true;
						}
						ImGui::PopID();
						ImGui::PopStyleColor(2);

						ImGui::SameLine();
						ImGui::Text("Reload assets after any changes are made.");

					ImGui::EndChild();
					ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Kill Banner"))
                {
					ImGui::Spacing();
					ImGui::BeginChild("kill banner", ImVec2(0, -ImGui::GetFrameHeightWithSpacing() + 10), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
						
						static bool was_kill_banner_enabled = true;
						ImGui::Checkbox(_settings.kill_banner_enabled ? "Enabled" : "Disabled", &_settings.kill_banner_enabled);
						if (_settings.kill_banner_enabled != was_kill_banner_enabled) {
							if (_settings.kill_banner_enabled) {
								KillEffectWindow::GetInstance().Show();
								spdlog::warn("Kill banner enabled.");
							}
							else {
								KillEffectWindow::GetInstance().Hide();
								spdlog::warn("Kill banner disabled.");
							}
							reload = true;

							was_kill_banner_enabled = _settings.kill_banner_enabled;
						}

						ImGui::Spacing();
						ImGui::Separator();
						ImGui::Spacing();

						ImGui::Text("Offset");
						ImGui::Spacing();
						ImGui::DragInt("Offset X", &_settings.offset_x);
						ImGui::DragInt("Offset Y", &_settings.offset_y);

						ImGui::Spacing();
						ImGui::Separator();
						ImGui::Spacing();

						ImGui::Text("Scale");
						ImGui::Spacing();
						ImGui::DragFloat("Scale Factor", &_settings.scale_factor, 0.01f, 0.01f, 9.99f);
						ImGui::Spacing();
						ImGui::Separator();
						ImGui::Spacing();
					ImGui::EndChild();
					ImGui::EndTabItem();
                }
				if (ImGui::BeginTabItem("Sound"))
                {
					ImGui::Spacing();
					ImGui::BeginChild("sound", ImVec2(0, -ImGui::GetFrameHeightWithSpacing() + 10), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
						if (ImGui::Button("Reinit Audio Device") ) {
							GGgui::Application& app = GGgui::Application::Get();
							app.ReinitAudioDevice();	
						}

						ImGui::Spacing();
						ImGui::Separator();
						ImGui::Spacing();

						ImGui::Text("Volume");
						ImGui::Spacing();
						ImGui::SliderFloat("Volume", &_settings.volume, 0.0f, 2.0f);

					ImGui::EndChild();
					ImGui::EndTabItem();
                }
				
				

                ImGui::EndTabBar();
            }
		
        } else {
            if(pre_load_complete && !load_complete){
                pre_load_complete = false;
            }
            KillEffectWindow::GetInstance().Hide();
            ImGui::Text("Loading Assets...");
            ImGui::ProgressBar(assets_load_progress);
        }
        ImGui::End();


        if(reload){
            reload = false;
            _settings.Save() ;
            _assets.Save() ;
            spdlog::warn("Reloading Assets");
            KillEffectWindow::GetInstance().LoadAssets();
        }
    }

	else {
		app.Close();
	}
//    ImGui::ShowDemoWindow();
}

void SettingWindow::OnUpdate(float ts) {
    Layer::OnUpdate(ts);

}

void SettingWindow::OnAttach() {
    Layer::OnAttach();
//    _hotkey_capture_thread = std::thread([this]{
//        RegisterHotKey(NULL, 1, MOD_CONTROL | 0X4000, VK_F12) ; // Ctrl F12
//        MSG msg;
//        auto& app = GGgui::Application::Get();
//        while(app.IsRunning()) {
//            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0) {
//                if (msg.wParam == 1) {
////                    std::cout << "hot key" << std::endl;
////                    show = !show;
//                }
//            }
//        }
//    });(void)_hotkey_capture_thread;
}

void SettingWindow::OnDetach() {
    Layer::OnDetach();
//    if(_hotkey_capture_thread.joinable()) _hotkey_capture_thread.join();
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
