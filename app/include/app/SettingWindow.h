//
// Created by zaiyichen on 2023/11/26.
//

#ifndef IMGUIOPENGL_SETTINGWINDOW_H
#define IMGUIOPENGL_SETTINGWINDOW_H

#include <gui/Application.h>
#include <json/json.h>
#include <fstream>

struct Settings{
    Settings(){
        Json::Value setting;
        std::filesystem::path setting_file = "settings.json";
        if(std::filesystem::exists(setting_file)) {
            std::ifstream file(setting_file);
            file >> setting;

            offset_x = setting["offset_x"].asInt();
            offset_y = setting["offset_y"].asInt();

            kill_accumulate_method = setting["kill_accumulate_method"].asInt();
            max_continuous_kill_time_sec = setting["max_continuous_kill_time_sec"].asFloat();

            show_effect_when_spectating = setting["show_effect_when_spectating"].asBool();
            file.close();
        }
        else{
            setting =
                R"({
                    "offset_x" : 0,
                    "offset_y" : 400,
                    "kill_accumulate_method" : 0,
                    "max_continuous_kill_time_sec" : 5.0,
                    "show_effect_when_spectating" : 0

                    })";
            std::ofstream file(setting_file );
            file << setting;
            file.close();
        }

    }

    ~Settings(){
        Json::Value setting;
        setting["offset_x"] = offset_x;
        setting["offset_y"] = offset_y;

        setting["kill_accumulate_method"] = kill_accumulate_method;
        setting["max_continuous_kill_time_sec"] = max_continuous_kill_time_sec;

        setting["show_effect_when_spectating"] = show_effect_when_spectating;
        std::filesystem::path setting_file = "./settings.json";
        std::fstream file(setting_file, std::ios_base::out);
        file << setting;
        file.close();
    }

    int offset_x{0};
    int offset_y{400};


    int kill_accumulate_method{0};   // 0 continuous kill, 1 round kill
    float max_continuous_kill_time_sec{5.0f};

    bool show_effect_when_spectating{false};
};

class SettingWindow : public GGgui::Layer
{
public:
    SettingWindow(){
        assert(_instance == nullptr);
        _instance = this;
    }
private:
    static SettingWindow* _instance;
    Settings _settings;
public:
    static SettingWindow& GetInstance() {
        assert(_instance != nullptr);
        return *_instance;
    }
public:
    virtual void OnUIRender() override;
    virtual void OnAttach() override;
    Settings& settings() {return _settings;}
public:
    float assets_load_progress = 0.0f;
    bool load_complete = false;
};

#endif //IMGUIOPENGL_SETTINGWINDOW_H
