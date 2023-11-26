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
        if(_frame_buffer->gl_texture_id() != 0)
            ImGui::Image(ImTextureID(_frame_buffer->gl_texture_id()), ImGui::GetWindowSize());

        ImGui::End();
    }
}

void KillEffectWindow::OnAttach() {
    Layer::OnAttach();
    load_images_from_disk();
    _frame_buffer = std::make_shared<GGgui::Image>();
    _image_sequence_player = std::make_shared<ImageSequencePlayer>(60, _frame_buffer);
    AddKillCount(1);
    ShowKillEffect();
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

void KillEffectWindow::ShowKillEffect() {
    assert(_continuous_kill_count <= _max_continuous_kill_count);
    std::cout << _continuous_kill_count << '\n';
    _image_sequence_player->ResetImageSequence(_image_buffer[_continuous_kill_count - 1]);
    auto &app = GGgui::Application::Get();
    app.PlaySound(std::format("./Assets/audio/{}kill.wav", _continuous_kill_count));
    _image_sequence_player->Play();
}

void KillEffectWindow::load_images_from_disk() {
    for(int ckc = 1; ckc <= _max_continuous_kill_count; ckc++ ) {
        std::vector<std::shared_ptr<EffectImage>> images;

        //load images from disk
        std::filesystem::path tmp = std::format("{}kill", ckc);
        std::filesystem::path image_folder_of_kill_count = _image_folder_path / tmp;
//        std::cout << image_folder_of_kill_count.string() << '\n';

        if (std::filesystem::exists(image_folder_of_kill_count) &&
            std::filesystem::is_directory(image_folder_of_kill_count)) {
            std::vector<std::filesystem::path> files;
            std::string png_ext = ".png";
            for (auto &f: std::filesystem::directory_iterator(image_folder_of_kill_count)) {
                if (f.path().extension() == png_ext) {
                    files.push_back(f.path());
                }
            }
//            std::sort(files.begin(), files.end());
            for (auto &f: files) {
//                std::cout << f.string() << '\n';
                images.push_back(std::make_shared<EffectImage>(f.string()));
            }
//            std::cout << images.size() << '\n';
            _image_buffer.push_back(std::make_shared<std::vector<std::shared_ptr<EffectImage>>>(std::move(images)));
        }
    }
    spdlog::info("loading images to memory succeeded.");
}

void KillEffectWindow::AddKillCount(int c) {
    _continuous_kill_count += c;
    if(_continuous_kill_count > _max_continuous_kill_count) {
        _continuous_kill_count = _max_continuous_kill_count;
    }
    _continuous_kill_timer = 0;


}

KillEffectWindow::KillEffectWindow() {
    assert(_instance == nullptr);
    _instance = this;
}
