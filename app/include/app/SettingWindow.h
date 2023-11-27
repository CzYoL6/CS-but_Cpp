//
// Created by zaiyichen on 2023/11/26.
//

#ifndef IMGUIOPENGL_SETTINGWINDOW_H
#define IMGUIOPENGL_SETTINGWINDOW_H

#include <gui/Application.h>

struct Settings{
    static Settings& Get(){
        static Settings instance;
        return instance;
    }

    int offset_x{0};
    int offset_y{0};

    int asset_quality{1};   // 0 medium , 1 high
    int high_framerate{1};  // 0 60fps,   1 120fps

    int kill_accumulate_method{0};   // 0 continuous kill, 1 round kill
    float max_continuous_kill_time_sec{5.0f};

    bool show_effect_when_spectating{false};
};

class SettingWindow : public GGgui::Layer
{
public:
    virtual void OnUIRender() override;
    virtual void OnAttach() override;
public:
    bool show = true;
};

#endif //IMGUIOPENGL_SETTINGWINDOW_H
