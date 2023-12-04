//
// Created by zaiyichen on 2023/12/5.
//

#ifndef CPP_BUT_ASSET_H
#define CPP_BUT_ASSET_H
#include <string>
#include <filesystem>
#include <json/json.h>
#include <spdlog/spdlog.h>
#include <fstream>


struct AssetConfig {
    AssetConfig(const Json::Value& config){
        asset_name = config["asset_name"].asString();
        is_custom = config["is_custom"].asBool();
        kill_banner_asset_folder = config["kill_banner_asset_folder"].asString();
        max_kill_banner_count = config["max_kill_banner_count"].asInt();
        kill_sound_asset_folder = config["kill_sound_asset_folder"].asString();
        max_kill_sound_count = config["max_kill_sound_count"].asInt();
        enable_headshot = config["enable_headshot"].asBool();
        headshot_banner_folder = config["headshot_banner_folder"].asString();
        headshot_sound_file = config["headshot_sound_file"].asString();

        spdlog::info("Asset named {} has been loaded.", asset_name);
    }

    void WriteToJson(Json::Value& config){
         config["asset_name"] = asset_name;
         config["is_custom"] = is_custom;
         config["kill_banner_asset_folder"] = kill_banner_asset_folder;
         config["max_kill_banner_count"] = max_kill_banner_count;
         config["kill_sound_asset_folder"] = kill_sound_asset_folder;
         config["max_kill_sound_count"] = max_kill_sound_count;
         config["enable_headshot"] = enable_headshot;
         config["headshot_banner_folder"] = headshot_banner_folder;
         config["headshot_sound_file"] = headshot_sound_file;

         spdlog::info("Asset named {} has been saved.", asset_name);
    }

    std::string asset_name;
    bool is_custom;
    std::string kill_banner_asset_folder;
    int max_kill_banner_count;
    std::string kill_sound_asset_folder;
    int max_kill_sound_count;
    bool enable_headshot;
    std::string headshot_banner_folder;
    std::string headshot_sound_file;
};

struct Assets{
    Assets(){
        Json::Value assets;
        std::filesystem::path assets_file = "assets/asset_config.json";
        if(std::filesystem::exists(assets_file)){
            std::ifstream file(assets_file);
            file >> assets;

            for(const Json::Value& asset_config : assets["assets"]){
                asset_configs.emplace_back(asset_config);
            }

            asset_names = new char*[assets["assets"].size()];
            for(int i = 0; i < asset_configs.size(); i++){
                asset_names[i] = new char[asset_configs[i].asset_name.size()];
                memcpy(asset_names[i], asset_configs[i].asset_name.c_str(), asset_configs[i].asset_name.size());
            }
            file.close();
            spdlog::warn("{} assets loaded.", assets["assets"].size());
        }
        else{
            spdlog::error("Asset config file not found.");
            exit(-1);
        }
    }

    ~Assets(){
        for(int i = 0; i < sizeof(asset_names) / sizeof(asset_names[0]); i++){
            delete[] asset_names[i];
        }
        delete[] asset_names;
        Json::Value assets;
        std::filesystem::path assets_file = "assets/asset_config.json";
        if(std::filesystem::exists(assets_file)){
            std::fstream file(assets_file);
            file >> assets;

            for(int i = 0; i < assets["assets"].size(); i++){
                asset_configs[i].WriteToJson(assets["assets"][i]);
            }

            file << assets;

            file.close();
            spdlog::warn("{} assets saved.", assets["assets"].size());
        }
        else{
            spdlog::error("Asset config file not found.");
            exit(-1);
        }
    }

    std::vector<AssetConfig> asset_configs;
    char* *asset_names;      // for ImGui
};

#endif //CPP_BUT_ASSET_H
