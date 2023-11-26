//
// Created by zaiyichen on 2023/11/26.
//

#ifndef IMGUIOPENGL_KILLEFFECTWINDOW_H
#define IMGUIOPENGL_KILLEFFECTWINDOW_H

#include <memory>
#include <gui/Application.h>
#include <app/ImageSequencePlayer.h>
#include <filesystem>


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
    void Hide() ;
    void Show();
    void AddKillCount(int c) ;
    void ShowKillEffect();
private:
    void load_images_from_disk();
public:
    bool hidden() const { return _hidden; }
    int continuous_kill_count() const { return _continuous_kill_count;}
    void set_continuous_kill_time(float t) { _max_continuous_kill_time = t;}
    void set_max_ckc(int c) { _max_continuous_kill_count = c; }
private:
    std::shared_ptr<ImageSequencePlayer> _image_sequence_player{nullptr};
    std::shared_ptr<GGgui::Image> _frame_buffer{nullptr};
    std::vector<std::shared_ptr<std::vector<std::shared_ptr<EffectImage>>>> _image_buffer;
    bool _hidden{false};
    int _continuous_kill_count{0};
    int _max_continuous_kill_count{5};
    float _max_continuous_kill_time{5};
    float _countinuous_kill_timer{0};

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


};


#endif //IMGUIOPENGL_KILLEFFECTWINDOW_H
