#pragma once

#include "sinks/portaudio_sink.h"
#include "sources/wave_source.h"
#include "elements/meter.h"

class player
{
private:
    wave wav;

    std::shared_ptr<meter> m = nullptr;
    std::shared_ptr<portaudio_sink> pa_sink = nullptr;
    bool running;

    int width = 81;
    int height = 35;
    int spacer = 2;

public:
    std::shared_ptr<wave_source> ws = nullptr;

    player();
    virtual ~player();

    void load(const char *path, bool normalize);
    void play();
    void stop();
    void do_play();

    void on_frame();
    void render_content();
};