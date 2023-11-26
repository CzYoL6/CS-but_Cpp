//
// Created by zaiyi on 2023/8/14.
//
#pragma once

#include <iostream>
#include <string_view>
#include <cstdint>
#include <stb_image/stb_image.h>
#include <GL/gl.h>

namespace GGgui {

    class Image {
    public:
        Image() = default;

        Image(const std::string_view &file_path);

        Image(unsigned char *image_data, int width, int height);

        ~Image();

        void LoadDataFromMemory(unsigned char *image_data, int width, int height);

        void Release();

        void LoadDataFromFile(const std::string_view &file_path);

        // getter and setter

        int image_width() const { return image_width_; }

        int image_height() const { return image_height_; }

        GLuint gl_texture_id() const { return gl_texture_id_; }

    private:
        uint32_t image_width_{0};
        uint32_t image_height_{0};
        GLuint gl_texture_id_{0};
    };
}
