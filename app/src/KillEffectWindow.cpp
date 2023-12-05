//
// Created by zaiyichen on 2023/11/26.
//

#include <format>
#include <filesystem>
#include <spdlog/spdlog.h>
#include <app/KillEffectWindow.h>
#include <gui/Application.h>
#include <gui/Application.h>
#include <app/SettingWindow.h>
#include <map>
#include <windows.h>
#include <psapi.h>

KillEffectWindow* KillEffectWindow::_instance = nullptr;

void KillEffectWindow::OnUIRender() {
    Layer::OnUIRender();
    const auto& app = GGgui::Application::Get();
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoDecoration;
//    if(true) {
    if(!_hidden && SettingWindow::GetInstance().load_complete) {
        const auto viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos({0,0});
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGui::Begin("Kill Effect Window", nullptr, window_flags);
        ImGui::GetWindowViewport()->Flags |= ImGuiViewportFlags_TopMost;
        if(_frame_buffer->gl_texture_id() != 0 && _image_sequence_player->playing()){
            Settings& settings = SettingWindow::GetInstance().settings();
            float factor =  ImGui::GetWindowWidth() / 
                ((SettingWindow::GetInstance().settings().asset_quality==0)?1920.0f:3840.0f);
            ImGui::SetCursorPos({ImGui::GetWindowWidth()*0.5f +  - factor*_frame_buffer->image_width()*0.5f + settings.offset_x, ImGui::GetWindowHeight()*0.5f - factor*_frame_buffer->image_height() + settings.offset_y});
//            std::cout <<ImGui::GetWindowWidth()*0.5f +  - factor*_frame_buffer->image_width()*0.5f + offset_x<<","<<ImGui::GetWindowHeight()*0.5f - factor*_frame_buffer->image_height()*0.5f + offset_y<<'\n';
            ImGui::Image(ImTextureID(_frame_buffer->gl_texture_id()),
                         {factor * _frame_buffer->image_width(), factor * _frame_buffer->image_height()});
        }

        ImGui::End();
    }
}

void KillEffectWindow::OnAttach() {
    Layer::OnAttach();

    _http_server.Post("/", [this](const httplib::Request &req, httplib::Response &res) {
//        std::cout << req.body << std::endl;
//                PlaySound("1kill.wav");
        static Json::Reader reader;
        Json::Value json_data;
        reader.parse(req.body, json_data);
        handle_data(json_data);

        res.set_content("", "text/html");
        res.set_header("Content-Type", "text/html" );
        res.status=200;
    });
    _http_server_thread = std::thread([&]{
        spdlog::warn("http server launched on port 3003.");
        _http_server.listen("127.0.0.1", 3003);
    }); (void )_http_server_thread;


    LoadAssets();


//    AddKillCount(1);
//    ShowKillEffect();
}

void KillEffectWindow::Hide() {
    _hidden = true;
}

void KillEffectWindow::Show() {
    _hidden = false;
}

void KillEffectWindow::OnUpdate(float ts) {
    Layer::OnUpdate(ts);
//    spdlog::info("timestep: {}s\n", ts);
    if(_show_round_kill_effect_sign){
        std::lock_guard<std::mutex> l(_show_round_kill_effect_mutex);
        ShowRoundKillEffect(_show_round_kill_effect_count);
        _show_round_kill_effect_sign = false;
    }

    if(_show_headshot_effect_sign){
        std::lock_guard<std::mutex> l(_show_headshot_effect_mutex);
        ShowHeadshotEffect();
        _show_headshot_effect_sign = false;

    }
    _image_sequence_player->Update(ts);
}



void KillEffectWindow::load_images_from_disk(float *progress, bool *load_complete, const AssetConfig &current_asset, int framerate,
                                             int quality) {

    for(int ckc = 1; ckc <= current_asset.max_kill_banner_count; ckc++ ) {
        std::vector<std::shared_ptr<EffectImage>> images;

        //load kill banner images from disk
        std::filesystem::path image_folder_of_kill_count = std::format("{}\\{}\\{}kill\\", current_asset.kill_banner_asset_folder, quality == 0 ? "1080p120hz" : "2k120hz", ckc);
        if (std::filesystem::exists(image_folder_of_kill_count) &&
            std::filesystem::is_directory(image_folder_of_kill_count)) {
            std::vector<std::filesystem::path> files;
            std::string png_ext = ".png";
            for (auto &f: std::filesystem::directory_iterator(image_folder_of_kill_count)) {
                if (f.path().extension() == png_ext) {
                    files.push_back(f.path());
                }
            }
            std::sort(files.begin(), files.end());
            for (int k = 0; k < files.size(); k += (framerate==0 ? 2 : 1)) {
                auto &f = files[k];
                images.push_back(std::make_shared<EffectImage>(f.string()));
            }
            _image_buffer_round_kill->push_back(std::make_shared<std::vector<std::shared_ptr<EffectImage>>>(std::move(images)));
        }

        *progress += 1.0f / (current_asset.max_kill_banner_count + (int)current_asset.enable_headshot);
    }

    if(current_asset.enable_headshot) {
        //load headshot from disk
        std::filesystem::path image_folder_of_headshot = std::format("{}\\{}\\", current_asset.headshot_banner_folder, quality == 0 ? "1080p120hz" : "2k120hz");
        if (std::filesystem::exists(image_folder_of_headshot) &&
            std::filesystem::is_directory(image_folder_of_headshot)) {
            std::vector<std::filesystem::path> files;
            std::string png_ext = ".png";
            for (auto &f: std::filesystem::directory_iterator(image_folder_of_headshot)) {
                if (f.path().extension() == png_ext) {
                    files.push_back(f.path());
                }
            }
            std::sort(files.begin(), files.end());
            for (int k = 0; k < files.size(); k += (framerate == 0 ? 2 : 1)) {
                auto &f = files[k];
                _image_buffer_headshot->push_back(std::make_shared<EffectImage>(f.string()));
            }
        }
        *progress += 1.0f / (current_asset.max_kill_banner_count + (int) current_asset.enable_headshot);
    }
    *load_complete = true;
    spdlog::warn("loading images to memory succeeded.");
}


KillEffectWindow::KillEffectWindow() {
    assert(_instance == nullptr);
    _instance = this;

    _frame_buffer = std::make_shared<GGgui::Image>();
    _image_buffer_round_kill = std::make_shared<std::vector<std::shared_ptr<std::vector<std::shared_ptr<EffectImage>>>>>();
    _image_buffer_headshot = std::make_shared<std::vector<std::shared_ptr<EffectImage>>>();
    _image_sequence_player = std::make_shared<ImageSequencePlayer>(SettingWindow::GetInstance().settings().framerate==0?60:120, _frame_buffer);

}

void KillEffectWindow::handle_data(const Json::Value &data) {
    static std::string pre_steamid = "empty";
    static int round_kills = 0;
    static int round_kills_hs = 0;
    if (data["player"]["steamid"].empty()) return;
    std::string steamid = data["player"]["steamid"].asString();
    if(SettingWindow::GetInstance().settings().only_show_effect_when_im_playing) {
        if (steamid != SettingWindow::GetInstance().settings().steamid) {
            return;
        }
    }

    if(data["player"]["state"]["round_killhs"].empty()) return;
    int cur_player_round_kill_hs = data["player"]["state"]["round_killhs"].asInt();

    if (data["player"]["state"]["round_kills"].empty()) return;
    int cur_player_round_kill = data["player"]["state"]["round_kills"].asInt();

    if (steamid != pre_steamid) {
        pre_steamid = steamid;
        round_kills = cur_player_round_kill;
        round_kills_hs = cur_player_round_kill_hs;
    }

    int delta_round_killhs = cur_player_round_kill_hs - round_kills_hs;
    int delta_round_kill = cur_player_round_kill - round_kills;

    round_kills_hs = cur_player_round_kill_hs;
    round_kills = cur_player_round_kill;


    if (delta_round_kill > 0) {
        if(cur_player_round_kill == 1 && SettingWindow::GetInstance().current_asset().enable_headshot){
            if(delta_round_killhs > 0){
                queue_show_headshot_effect_on_opengl_thread();
                return;
            }
        }
        queue_show_round_kill_effect_on_opengl_thread(cur_player_round_kill);
    }
}

void KillEffectWindow::ShowRoundKillEffect(int round_kill) {
    const auto& current_asset = SettingWindow::GetInstance().current_asset();
    int clamped_banner_index = std::min(round_kill, current_asset.max_kill_banner_count);
    int clamped_audio_index = std::min(round_kill, current_asset.max_kill_sound_count);

    if(!SettingWindow::GetInstance().load_complete) return;
    _image_sequence_player->ResetImageSequence((*_image_buffer_round_kill)[clamped_banner_index - 1]);
    auto &app = GGgui::Application::Get();
    app.PlayAudio(std::format("{}\\{}kill.wav", current_asset.kill_sound_asset_folder, clamped_audio_index));

    _image_sequence_player->Play();
}

void KillEffectWindow::ShowHeadshotEffect() {
    if(!SettingWindow::GetInstance().load_complete) return;
    _image_sequence_player->ResetImageSequence(_image_buffer_headshot);
    auto &app = GGgui::Application::Get();
    app.PlayAudio(SettingWindow::GetInstance().current_asset().headshot_sound_file);

    _image_sequence_player->Play();
}

void KillEffectWindow::OnDetach() {
    Layer::OnDetach();
    _http_server.stop();
    if(_http_server_thread.joinable()) _http_server_thread.join();
    if(_load_assets_thread.joinable()) _load_assets_thread.join();
}

void KillEffectWindow::LoadAssets() {
    _image_sequence_player->Stop();
    _image_sequence_player->set_framerate(SettingWindow::GetInstance().settings().framerate==0?60:120);
    _image_buffer_round_kill->clear();
    _image_buffer_headshot->clear();
    SettingWindow::GetInstance().assets_load_progress = 0.0f;
    SettingWindow::GetInstance().load_complete = false;
    if(_load_assets_thread.joinable()) _load_assets_thread.join();
    _load_assets_thread = std::thread([this](){
        load_images_from_disk(&SettingWindow::GetInstance().assets_load_progress,
                              &SettingWindow::GetInstance().load_complete,
                              SettingWindow::GetInstance().current_asset(),
                              SettingWindow::GetInstance().settings().framerate,
                              SettingWindow::GetInstance().settings().asset_quality);
    });(void)_load_assets_thread;
}

void KillEffectWindow::queue_show_round_kill_effect_on_opengl_thread(int count) {
    std::lock_guard<std::mutex> l(_show_round_kill_effect_mutex);
    _show_round_kill_effect_count = count;
    _show_round_kill_effect_sign = true;
}

void KillEffectWindow::queue_show_headshot_effect_on_opengl_thread() {
    std::lock_guard<std::mutex> l(_show_headshot_effect_mutex);
    _show_headshot_effect_sign = true;
}



