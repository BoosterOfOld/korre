#pragma once

#include <string>
#include <cstring>

#include "imtui/imtui.h"
#include "imtui/imtui-impl-ncurses.h"

#include "player.h"
#include "elements/audio_select.h"
#include "elements/dsp_convolver.h"
#include "queue.h"

class korre
{
private:
    bool exit = false;
    ImTui::TScreen* screen;
    std::string title_text = u8"Korre "; //  ╣Korre

    std::unique_ptr<player> pl;
    std::unique_ptr<audio_select> as;
    std::unique_ptr<dsp_convolver> dc;
    std::unique_ptr<queue> qu;

    bool audio_selected = false;
    std::filesystem::path selected_audio;

    bool ir_selected = false;
    std::string selected_ir;

    int num_devices = 0;
    int selected_device = 0;
    std::vector<std::string> device_names;
    std::vector<std::string> device_srs;

    bool convolver_enabled = false;

    void refresh_devices();
    void init();
    void clean_up();
    void unload_audio();
    void load_audio(std::filesystem::path path);
    void load_ir(std::string path);
    void main_loop();

    void on_frame();
    void render_main_menu();

public:
    korre();
    virtual ~korre();

    void run();
    void stop();
};
