//
// Created by zaiyi on 2023/8/14.
//

#include <gui/Image.h>
#include <GL/glu.h>
#include <spdlog/spdlog.h>

namespace GGgui {
    void Image::checkGLError() {
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            spdlog::error( "OpenGL Error: {}" ,(char*)gluErrorString(error) );
            exit(-1);
        }
    }
    Image::Image(const std::string_view &file_path) {
        // TODO: add file path verification
        LoadDataFromFile(file_path);
    }

    Image::Image(unsigned char *image_data, int width, int height) {
        LoadDataFromMemory(image_data, width, height);
    }

    void Image::LoadDataFromMemory(unsigned char *data, int width, int height) {
        if(width == image_width_ && height == image_height_ && gl_texture_id_ != 0){
            glBindTexture(GL_TEXTURE_2D, gl_texture_id_);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image_width_, image_height_,
                            GL_RGBA, GL_UNSIGNED_BYTE, data);
        }else {
            if (gl_texture_id_ != 0) glDeleteTextures(1, &gl_texture_id_);

            image_width_ = width;
            image_height_ = height;

            GLuint textureID;

            glGenTextures(1, &textureID);

            glBindTexture(GL_TEXTURE_2D, textureID);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width_, image_height_,
                         0,GL_RGBA, GL_UNSIGNED_BYTE, data);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            gl_texture_id_ = textureID;

        }
        checkGLError();
    }

    void Image::LoadDataFromFile(const std::string_view &file_path) {
        std::string image_file_path(file_path);
        int width, height, channel;
        unsigned char *data = stbi_load(image_file_path.c_str(), &width, &height, &channel,
                                        STBI_rgb_alpha);
        LoadDataFromMemory(data, width, height);

        stbi_image_free(data);
    }

    void Image::Release() {
        glDeleteTextures(1, &gl_texture_id_);
        image_width_ = image_height_ = 0;
    }

    Image::~Image() {
        Release();
    }

}