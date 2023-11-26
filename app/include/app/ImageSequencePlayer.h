//
// Created by zaiyichen on 2023/11/26.
//

#ifndef IMGUIOPENGL_IMAGESEQUENCEPLAYER_H
#define IMGUIOPENGL_IMAGESEQUENCEPLAYER_H

#include <vector>
#include <memory>
#include <gui/Image.h>
#include <app/EffectImage.h>

class ImageSequencePlayer {
public:
    ImageSequencePlayer(int framerate,std::shared_ptr< GGgui::Image> framebuffer) ;
    ~ImageSequencePlayer();

public:
    void ResetImageSequence(std::shared_ptr<std::vector<std::shared_ptr<EffectImage>>> images, bool pause = true);
    void Update(float dt);
    void Stop();
    void Play();
private:
    void SetToBuffer();

public:
    int framerate() const {return _framerate;}
    int current_frame_index() const {return _current_frame_index;}
    bool playing() const { return _playing;}

private:
    int _framerate;

    std::shared_ptr<std::vector<std::shared_ptr<EffectImage>>> _image_sequence;

    int _current_frame_index;                                       // starting from 1

    bool _playing;

    float _accumulator;

    std::shared_ptr<GGgui::Image> _framebuffer;
};


#endif //IMGUIOPENGL_IMAGESEQUENCEPLAYER_H
