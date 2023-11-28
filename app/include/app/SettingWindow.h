//
// Created by zaiyichen on 2023/11/26.
//

#ifndef IMGUIOPENGL_SETTINGWINDOW_H
#define IMGUIOPENGL_SETTINGWINDOW_H

#include <gui/Application.h>
#include <json/json.h>
#include <fstream>
#include <thread>
#include <spdlog/spdlog.h>

struct Settings{
    Settings(){
        Json::Value setting;
        std::filesystem::path setting_file = "settings.json";
        if(std::filesystem::exists(setting_file)) {
            std::ifstream file(setting_file);
            file >> setting;

            offset_x = setting["offset_x"].asInt();
            offset_y = setting["offset_y"].asInt();

            only_show_effect_when_im_playing = setting["only_show_effect_when_im_playing"].asBool();
            steamid = setting["steamid"].asString();
            file.close();
            spdlog::warn("Settings loaded.");
        }
        else{
            Json::Reader reader;
            reader.parse(R"({
                    "offset_x" : 0,
                    "offset_y" : 400,
                    "only_show_effect_when_im_playing" : false,
                    "steamid" : ""
                    })", setting);
            std::ofstream file(setting_file );
            file << setting;
            file.close();
            spdlog::warn("No settings file found, new one created.");
        }
    }

    ~Settings(){
        Json::Value setting;
        setting["offset_x"] = offset_x;
        setting["offset_y"] = offset_y;

        setting["only_show_effect_when_im_playing"] = only_show_effect_when_im_playing;
        setting["steamid"] = steamid;
        std::filesystem::path setting_file = "settings.json";
        std::ofstream file(setting_file);
        file << setting;
        file.close();
        spdlog::warn("Settings saved.");
    }

    int offset_x{0};
    int offset_y{400};

    bool only_show_effect_when_im_playing{false};
    std::string steamid;
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
    std::thread _hotkey_capture_thread;
public:
    static SettingWindow& GetInstance() {
        assert(_instance != nullptr);
        return *_instance;
    }
public:
    virtual void OnUIRender() override;
    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(float ts) override;

    Settings& settings() {return _settings;}
public:
    bool show = true;
    float assets_load_progress = 0.0f;
    bool load_complete = false;
};

#endif //IMGUIOPENGL_SETTINGWINDOW_H
