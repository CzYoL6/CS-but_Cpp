//
// Created by zaiyichen on 2023/11/26.
//

#include <app/ImageSequencePlayer.h>
ImageSequencePlayer::ImageSequencePlayer(int framerate, std::shared_ptr<GGgui::Image> framebuffer)
:   _framerate(framerate), _current_frame_index(1),_playing(false),
    _accumulator(0), _framebuffer(framebuffer), _image_sequence(nullptr)
{

}

ImageSequencePlayer::~ImageSequencePlayer() {

}

void ImageSequencePlayer::ResetImageSequence(std::shared_ptr<std::vector<std::shared_ptr<EffectImage>>> images, bool pause) {
    _image_sequence = images;
    _current_frame_index = 1;
    _accumulator = 0;

    _playing = !pause;
    if(images->empty()){
        Stop();
    }
    else{
        // set to buffer immediately
        SetToBuffer();
    }

}

void ImageSequencePlayer::Update(float dt) {
    if(!_playing) return;
    _accumulator += dt;
    while(_accumulator >= 1.0f / _framerate){
        _accumulator -= 1.0f / _framerate;
        _current_frame_index ++;
        if(_current_frame_index > _image_sequence->size()){
            Stop();
            break;
        }
        SetToBuffer();
    }
}

void ImageSequencePlayer::SetToBuffer() {
    const auto& image =  (*_image_sequence)[_current_frame_index - 1];
    _framebuffer->LoadDataFromMemory(image->image_data(), image->width(), image->height());
}



void ImageSequencePlayer::Stop() {
    _playing = false;
}

void ImageSequencePlayer::Play() {
    _playing = true;
}
