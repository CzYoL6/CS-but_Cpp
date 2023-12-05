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
#include "Asset.h"


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
    void ShowHeadshotEffect();
    void LoadAssets();
private:
    void load_images_from_disk(float *progress, bool *load_complete, const AssetConfig &current_asset, int framerate,
                               int quality);
    void handle_data(const Json::Value& data);
public:
    bool hidden() const { return _hidden; }
private:
    std::shared_ptr<ImageSequencePlayer> _image_sequence_player{nullptr};
    std::shared_ptr<GGgui::Image> _frame_buffer{nullptr};
    std::shared_ptr<std::vector<std::shared_ptr<std::vector<std::shared_ptr<EffectImage>>>>> _image_buffer_round_kill;
    std::shared_ptr<std::vector<std::shared_ptr<EffectImage>>> _image_buffer_headshot;
    bool _hidden{true};

    bool _show_round_kill_effect_sign = false;
    int _show_round_kill_effect_count;
    std::mutex _show_round_kill_effect_mutex;

    bool _show_headshot_effect_sign = false;
    std::mutex _show_headshot_effect_mutex;

    httplib::Server _http_server;
    std::thread _http_server_thread;
    std::thread _load_assets_thread;

    void queue_show_round_kill_effect_on_opengl_thread(int count);
    void queue_show_headshot_effect_on_opengl_thread();
};


#endif //IMGUIOPENGL_KILLEFFECTWINDOW_H
