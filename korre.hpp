#pragma once

#include <string>

#include "imtui/imtui.h"
#include "imtui/imtui-impl-ncurses.h"

#include "player.hpp"
#include "audio_select.hpp"

class korre
{
private:
    bool exit = false;
    ImTui::TScreen* screen;
    std::string title_text = "Korre";

    std::unique_ptr<player> pl;
    std::unique_ptr<audio_select> as;

    bool audio_selected = false;
    std::string selected_audio;

    void init()
    {
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

            if (ImGui::BeginMenu("Menu"))
            {
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Examples"))
            {
                ImGui::MenuItem("Console", nullptr, nullptr);
                ImGui::MenuItem("Kora", nullptr, nullptr);

                ImGui::EndMenu();
            }
            //if (ImGui::MenuItem("MenuItem")) {} // You can also use MenuItem() inside a menu bar!
            if (ImGui::BeginMenu("Tools"))
            {
                //ImGui::MenuItem("Metrics/Debugger", NULL, &show_tool_metrics, has_debug_tools);
                //ImGui::MenuItem("Style Editor", NULL, &show_tool_style_editor);
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
