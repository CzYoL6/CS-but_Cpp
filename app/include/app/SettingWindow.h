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
#include <app/Asset.h>
#include <app/WindowsFileDialog.h>
struct Settings{
    Settings(){
        Json::Value setting;
        std::filesystem::path setting_file = FileDialog::getCanonicalPath("settings.json");
        if(std::filesystem::exists(setting_file)) {
            std::ifstream file(setting_file);
            file >> setting;

            offset_x = setting["offset_x"].asInt();
            offset_y = setting["offset_y"].asInt();
            asset_quality = setting["asset_quality"].asInt();
            framerate = setting["framerate"].asInt();
            only_show_effect_when_im_playing = setting["only_show_effect_when_im_playing"].asBool();
            steamid = setting["steamid"].asString();
            asset_preset = setting["asset_preset"].asInt();
            scale_factor = setting["scale_factor"].asFloat();
            volume = setting["volume"].asFloat();

            file.close();
            spdlog::warn("Settings loaded.");
        }
        else{
            Json::Reader reader;
            reader.parse(R"({
                    "offset_x" : 0,
                    "offset_y" : 400,
                    "asset_quality" : 0,
                    "framerate" : 0,
                    "only_show_effect_when_im_playing" : false,
                    "steamid" : "",
                    "asset_preset" : 0,
                    "scale_factor" : 1.0,
                    "volume" : 1.0
                    })", setting);
            std::ofstream file(setting_file );
            file << setting;
            file.close();
            spdlog::warn("No settings file found, new one created.");
        }
    }

    ~Settings(){
        Save();
    }

    void Save(){
        Json::Value setting;
        setting["offset_x"] = offset_x;
        setting["offset_y"] = offset_y;
        setting["asset_quality"] = asset_quality;
        setting["framerate"] = framerate;
        setting["only_show_effect_when_im_playing"] = only_show_effect_when_im_playing;
        setting["steamid"] = steamid;
        setting["asset_preset"] = asset_preset;
        setting["scale_factor"] = scale_factor;
        setting["volume"] = volume;
        std::filesystem::path setting_file = FileDialog::getCanonicalPath("settings.json");
        std::ofstream file(setting_file);
        file << setting;
        file.close();
        spdlog::warn("Settings saved.");

    }

    int offset_x{0};
    int offset_y{400};
    int asset_quality{0};   //0 medium, 1 high
    int framerate{0};   //0 60fps, 1 120fps

    bool only_show_effect_when_im_playing{false};
    std::string steamid;
    int asset_preset{0};

    float scale_factor{1.0};
    float volume{1.0};
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
    Assets _assets;
//    std::thread _hotkey_capture_thread;
    static std::pair<int, int> get_memory_consumption(); // <virtual mem, physical mem> MB

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
    Assets& assets() {return _assets;}
    AssetConfig& current_asset() { return _assets.asset_configs[_settings.asset_preset];}

public:
    bool show = true;
    float assets_load_progress = 0.0f;
    bool load_complete = false;
};

#endif //IMGUIOPENGL_SETTINGWINDOW_H
