#pragma once

#include <string>
#include <cstring>
#include <iostream>

#include "imtui/imtui.h"
#include "imtui/imtui-impl-ncurses.h"

#include "player.hpp"
#include "audio_select.hpp"

class korre
{
private:
    bool exit = false;
    ImTui::TScreen* screen;
    std::string title_text = "Korre ";

    std::unique_ptr<player> pl;
    std::unique_ptr<audio_select> as;

    bool audio_selected = false;
    std::string selected_audio;

    int num_devices = 0;
    int selected_device = 0;
    std::vector<const PaDeviceInfo *> devices;
    std::vector<std::string> device_names;
    std::vector<std::string> device_srs;

    void refresh_devices()
    {
        devices.clear();
        device_names.clear();
        device_srs.clear();
        auto err = Pa_Initialize();
        if(err == paNoError)
        {
            num_devices = Pa_GetDeviceCount();
            for (auto i = 0; i < num_devices; ++i)
            {
                auto d = Pa_GetDeviceInfo(i);
                devices.emplace_back(d);
                device_names.emplace_back(d->name);
                device_srs.emplace_back(std::to_string((int32_t)d->defaultSampleRate));
            }
        }
        Pa_Terminate();
    }

    void init()
    {
        refresh_devices();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        screen = ImTui_ImplNcurses_Init(true);
        ImTui_ImplText_Init();
    }

    void clean_up()
    {
        ImTui_ImplText_Shutdown();
        ImTui_ImplNcurses_Shutdown();
    }

    void unload_audio()
    {
        audio_selected = false;
        pl->stop();
    }

    void load_audio(std::string path)
    {
        audio_selected = false;
        this->selected_audio = path;
        pl->stop();
        pl = std::make_unique<player>();
        pl->load(this->selected_audio.c_str());
        audio_selected = true;
    }

    void main_loop()
    {
        while(!exit)
        {
            ImTui_ImplNcurses_NewFrame();
            ImTui_ImplText_NewFrame();
            ImGui::NewFrame();

            on_frame();

            ImGui::Render();
            ImTui_ImplText_RenderDrawData(ImGui::GetDrawData(), screen);
            ImTui_ImplNcurses_DrawScreen();
        }
    }

    void on_frame()
    {
        render_main_menu();

        if (audio_selected)
        {
            pl->on_frame();
        }
        as->on_frame();
    }

    void render_main_menu()
    {
        if (ImGui::BeginMainMenuBar())
        {
            ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(1.f / 7.0f, 0.9f, 0.9f));
            ImGui::Text(title_text.c_str());
            ImGui::PopStyleColor();

            ImGui::SameLine();

            if (ImGui::BeginMenu("File"))
            {
                if(ImGui::MenuItem("Exit", nullptr, nullptr))
                {
                    exit = true;
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Output Device"))
            {
                if (ImGui::MenuItem("Refresh Device List", nullptr, nullptr))
                {
                    refresh_devices();
                }

                if (ImGui::BeginMenu("Devices           >"))
                {
                    if (ImGui::MenuItem((std::string("Use Default Device")+( (0 == selected_device) ? std::string(" (selected)") : std::string(""))).c_str()))
                    {
                        unload_audio();
                        selected_device = 0;
                        PA_SELECTED_DEVICE = -1;
                        PA_USE_DEFAULT_DEVICE = true;
                    }

                    int cnt = 1;
                    for (auto d: device_names)
                    {
                        if (ImGui::MenuItem((
                                                    std::string("Device ") + std::to_string(cnt - 1) + ": " + d + " at " + device_srs[cnt-1] + "Hz" +
                                                            ( ((cnt) == selected_device) ? std::string(" (selected)") : std::string(""))
                                                    ).c_str()))
                        {
                            unload_audio();
                            selected_device = cnt;
                            PA_SELECTED_DEVICE = selected_device - 1;
                            PA_USE_DEFAULT_DEVICE = false;
                        }
                        ++cnt;
                    }
                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
    }
public:
    korre()
    {
        as = std::make_unique<audio_select>([this](auto && PH1)
                    {
                        load_audio(std::forward<decltype(PH1)>(PH1));
                    }
                );
        pl = std::make_unique<player>();
    }

    virtual ~korre()
    {
        ;
    }

    void run()
    {
        init();
        main_loop();
        clean_up();
    }

    void stop()
    {
        exit = true;
    }
};
