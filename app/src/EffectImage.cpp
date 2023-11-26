//
// Created by zaiyichen on 2023/11/26.
//

#include <app/EffectImage.h>
#include <stb_image/stb_image.h>
#include <iostream>
#include <format>

EffectImage::EffectImage(std::string_view file_path) {
    int channel;
    _image_data = stbi_load(file_path.data(), &_width, &_height, &channel, STBI_rgb_alpha);
//    std::cout << std::format("width: {}, height: {}\n", width(), height());
}

EffectImage::~EffectImage() {
    stbi_image_free(_image_data);
}
