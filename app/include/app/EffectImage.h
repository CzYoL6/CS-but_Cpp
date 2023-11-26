//
// Created by zaiyichen on 2023/11/26.
//

#ifndef IMGUIOPENGL_EFFECTIMAGE_H
#define IMGUIOPENGL_EFFECTIMAGE_H

#include <string_view>

class EffectImage {
public:
    EffectImage(std::string_view file_path);
    ~EffectImage();
public:
    int width() const {return _width;}
    int height() const {return _height;}
    unsigned char* image_data() const {return _image_data;}
private:
    unsigned char* _image_data{nullptr};
    int _width{0};
    int _height{0};
};


#endif //IMGUIOPENGL_EFFECTIMAGE_H
