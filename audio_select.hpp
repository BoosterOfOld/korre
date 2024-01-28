#pragma once

#include <string>
#include <utility>
#include <vector>
#include <iostream>
#include <filesystem>

#include "imtui/imtui.h"
#include "imtui/imtui-impl-ncurses.h"
#include "windowth.hpp"

static bool LOADER_NORMALIZE = false;

class audio_select
{
private:
    std::vector<std::filesystem::path> itemies;
    int item_current_idx = 0;
    std::string dir_path = "/Users/northkillpd/temp";
    std::string selected_path;

    std::function<void(std::string)> callback;
    std::function<void(std::string)> ir_callback;

    void update_file_list()
    {
        itemies.clear();
        for (const auto & entry : std::filesystem::directory_iterator(dir_path))
        {
            if (entry.path().extension() == ".wav")
            {
                itemies.emplace_back(entry.path());
            }
        }
    }

public:
    explicit audio_select(std::function<void(std::string)> callback, std::function<void(std::string)> ir_callback)
    {
        this->callback = std::move(callback);
        this->ir_callback = std::move(ir_callback);
        update_file_list();
    }

    virtual ~audio_select()
    {
        ;
    }

    int w = 41;
    int h = 29;

    bool is_loading = false;

    void on_frame()
    {
        ImGui::SetNextWindowPos(ImVec2((float)86, (float)3), ImGuiCond_Once);

        windowth(w, h, "Audio_Select", [this]()->void {render_content();});
    }

    void render_content()
    {

        ImGui::Text(" ");
        ImGui::Text("Workspace:");

        ImGui::Text(" ");

        ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor(30, 30, 30));
        ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(255,255,104));
        static char wksp[182] = "/Users/northkillpd/temp/"; ImGui::InputText("", wksp, 128);
        ImGui::PopStyleColor(2);
        ImGui::SameLine();
        if (ImGui::Button(" RELOAD "))
        {
            dir_path = std::string(wksp);
            update_file_list();
        }

        ImGui::Text(" ");

        if (itemies.empty())
        {
            ImGui::Text("No WAV files at destination ");
        }
        else
        {
            ImGui::Text("Selected: ");
            ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(255,255,255));
            ImGui::SameLine(); ImGui::Text("%s", itemies[item_current_idx].filename().c_str());
            ImGui::PopStyleColor();
            selected_path = itemies[item_current_idx];
        }
        ImGui::Text(" ");

        ImGui::Text(" ");
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor(30, 30, 30));
        ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(255,255,104));
        if (ImGui::BeginListBox("Audio Select", ImVec2(w - 2.f, h - 15.f)))
        {
            for (int n = 0; n < itemies.size(); ++n)
            {
                const bool is_selected = (item_current_idx == n);
                if (ImGui::Selectable(itemies[n].filename().c_str(), is_selected))
                    item_current_idx = n;

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndListBox();
        }
        ImGui::PopStyleColor(2);

        ImGui::Text("");
        ImGui::Text("             "); ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor(60, 60, 60));
        ImGui::Checkbox(" Normalize track to 0dB", &LOADER_NORMALIZE);
        ImGui::PopStyleColor();
        ImGui::Text("");

        ImGui::Text(""); ImGui::SameLine();
        //ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.5f / 7.f, 0.6f, 0.6f));
        //ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.5f / 7.f, 0.7f, 0.7f));
        //ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.5f / 7.f, 0.8f, 0.8f));

        ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(255,255,255));
        ImGui::Text(" ");ImGui::SameLine();
        if (ImGui::Button(is_loading ? " ---- " : " LOAD AS IR "))
        {
            ir_callback(selected_path);
        }
        //ImGui::PopStyleColor(3);

        ImGui::SameLine(); ImGui::Text("   "); ImGui::SameLine();
        ImGui::SameLine();

        if (ImGui::Button(is_loading ? " ---- " : " LOAD SELECTED AUDIO "))
        {
            callback(selected_path);
        }
        ImGui::PopStyleColor();
    }
};