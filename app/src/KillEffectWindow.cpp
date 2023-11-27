//
// Created by zaiyichen on 2023/11/26.
//

#include <format>
#include <filesystem>
#include <spdlog/spdlog.h>
#include <app/KillEffectWindow.h>
#include <gui/Application.h>

KillEffectWindow* KillEffectWindow::_instance = nullptr;

void KillEffectWindow::OnUIRender() {
    Layer::OnUIRender();
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDocking;
    if(!_hidden) {
        const auto viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos({0,0});
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGui::Begin("Kill Effect Window", nullptr, window_flags);
        if(_frame_buffer->gl_texture_id() != 0 && _image_sequence_player->playing()){
            float factor =  ImGui::GetWindowWidth() / 3840.0f;
            ImGui::SetCursorPos({1400 * factor, 1400 * factor});
            ImGui::Image(ImTextureID(_frame_buffer->gl_texture_id()),
                         {factor * _frame_buffer->image_width(), factor * _frame_buffer->image_height()});
        }

        ImGui::End();
    }
}

void KillEffectWindow::OnAttach() {
    Layer::OnAttach();
    load_images_from_disk();

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
        spdlog::warn("http server launched.");
        _http_server.listen("127.0.0.1", 3003);
    });


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
    if(_continuous_kill_count > 0){
        _continuous_kill_timer += ts;
        if(_continuous_kill_timer >= _max_continuous_kill_time){
            _continuous_kill_timer = 0;
            _continuous_kill_count = 0;
        }
    }

    _image_sequence_player->Update(ts);
}

void KillEffectWindow::ShowKillEffect_ThreadSafe() {
    _continuous_kill_mutex.lock();

    assert(_continuous_kill_count <= _max_continuous_kill_count);
    std::cout << _continuous_kill_count << '\n';
    _image_sequence_player->ResetImageSequence(_image_buffer[_continuous_kill_count - 1]);
    auto &app = GGgui::Application::Get();
    app.PlayAudio(std::format("E:\\Programming\\CS-but_Cpp\\cmake-build-release\\app\\Assets\\audio\\{}kill.wav", _continuous_kill_count));

    _continuous_kill_mutex.unlock();
    _image_sequence_player->Play();
}

void KillEffectWindow::load_images_from_disk() {
    for(int ckc = 1; ckc <= _max_continuous_kill_count; ckc++ ) {
        std::vector<std::shared_ptr<EffectImage>> images;

        //load images from disk
//        std::filesystem::path tmp = std::format("{}kill", ckc);
//        std::filesystem::path image_folder_of_kill_count = _image_folder_path / tmp;
//        std::filesystem::path image_folder_of_kill_count = std::format("E:\\Programming\\CS-but_Cpp\\cmake-build-release\\app\\Assets\\banner\\{}kill\\", ckc);
        std::filesystem::path image_folder_of_kill_count = std::format("E:\\AdobeWorks\\Valorant\\cropped\\{}kill\\", ckc);
        std::cout << image_folder_of_kill_count.string() << '\n';
        std::cout << std::filesystem::exists(image_folder_of_kill_count) << std::endl;
        std::cout <<std::filesystem::is_directory(image_folder_of_kill_count)  << std::endl;
        if (std::filesystem::exists(image_folder_of_kill_count) &&
            std::filesystem::is_directory(image_folder_of_kill_count)) {
//if(true){
            std::vector<std::filesystem::path> files;
            std::string png_ext = ".png";
            for (auto &f: std::filesystem::directory_iterator(image_folder_of_kill_count)) {
                if (f.path().extension() == png_ext) {
                    files.push_back(f.path());
                }
            }
//            std::sort(files.begin(), files.end());
            for (auto &f: files) {
                std::cout << f.string() << '\n';
                images.push_back(std::make_shared<EffectImage>(f.string()));
            }
            std::cout << images.size() << '\n';
            _image_buffer.push_back(std::make_shared<std::vector<std::shared_ptr<EffectImage>>>(std::move(images)));
        }
    }
    std::cout << _image_buffer.size() << std::endl;
    spdlog::info("loading images to memory succeeded.");
}

void KillEffectWindow::AddKillCount_ThreadSafe(int c) {

    _continuous_kill_mutex.lock();

    _continuous_kill_count += c;
    if(_continuous_kill_count > _max_continuous_kill_count) {
        _continuous_kill_count = _max_continuous_kill_count;
    }
    _continuous_kill_mutex.unlock();
    _continuous_kill_timer = 0;


}

KillEffectWindow::KillEffectWindow() {
    assert(_instance == nullptr);
    _instance = this;

    _frame_buffer = std::make_shared<GGgui::Image>();
    _image_sequence_player = std::make_shared<ImageSequencePlayer>(120, _frame_buffer);

}

void KillEffectWindow::handle_data(const Json::Value &data) {
    do {
        if(data.empty() || data["player"].empty() || data["player"]["steamid"].empty()) break;
        std::string steamid = data["player"]["steamid"].asString();
        if (steamid != "76561198863164733") {
            return;
        }

        if(data["player"]["state"].empty() || data["player"]["state"]["round_kills"].empty()) break;
        int cur_player_round_kill = data["player"]["state"]["round_kills"].asInt();

        if(data["previously"].empty() || data["previously"]["player"].empty() || data["previously"]["player"]["state"].empty() || data["previously"]["player"]["state"]["round_kills"].empty()) break;
        int previous_player_round_kill = data["previously"]["player"]["state"]["round_kills"].asInt();

        int delta_round_kill = cur_player_round_kill - previous_player_round_kill;

        if (delta_round_kill > 0) {

            AddKillCount_ThreadSafe(delta_round_kill);
            ShowKillEffect_ThreadSafe();
        }
    }while(false);

}
