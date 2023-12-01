//
// Created by zaiyichen on 2023/11/26.
//

#ifndef IMGUIOPENGL_KILLEFFECTWINDOW_H
#define IMGUIOPENGL_KILLEFFECTWINDOW_H

#include <memory>
#include <gui/Application.h>
#include <app/ImageSequencePlayer.h>
#include <filesystem>
#include <httplib.h>
#include <json/json.h>


class KillEffectWindow : public GGgui::Layer{
public:
    KillEffectWindow();
private:
    static KillEffectWindow* _instance;
public:
    static KillEffectWindow& GetInstance() {
        assert(_instance != nullptr);
        return *_instance;
    }
public:
    virtual void OnUIRender() override;
    virtual void OnUpdate(float ts) override;
    virtual void OnAttach() override;
    virtual void OnDetach() override;
    void Hide() ;
    void Show();
    void ShowRoundKillEffect(int round_kill);
    void LoadAssets();
private:
    void load_images_from_disk(float *progress, bool *load_complete, int framerate, int quality);
    void handle_data(const Json::Value& data);
public:
    bool hidden() const { return _hidden; }
    void set_max_ckc(int c) { _max_continuous_kill_count = c; }
private:
    std::shared_ptr<ImageSequencePlayer> _image_sequence_player{nullptr};
    std::shared_ptr<GGgui::Image> _frame_buffer{nullptr};
    std::vector<std::shared_ptr<std::vector<std::shared_ptr<EffectImage>>>> _image_buffer;
    bool _hidden{true};
    int _max_continuous_kill_count{5};

    std::filesystem::path _image_folder_path{"./Assets/banner/"};        // where kill banners are stored
                                                                  // the folder structure must be
                                                                  // - folder
                                                                  //    - 1kill
                                                                  //        - frame_1.png
                                                                  //        - frame_2.png
                                                                  //        - ...
                                                                  //    - 2kill
                                                                  //    - ...
                                                                  // default is at "./Assets/banner/"

    httplib::Server _http_server;
    std::thread _http_server_thread;
    std::thread _load_assets_thread;
};


#endif //IMGUIOPENGL_KILLEFFECTWINDOW_H
