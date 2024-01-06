#pragma once

#include <string>
#include <utility>
#include <vector>
#include <iostream>
#include <filesystem>

#include "imtui/imtui.h"
#include "imtui/imtui-impl-ncurses.h"

class audio_select
{
private:
    std::vector<std::filesystem::path> itemies;
    int item_current_idx = 0;
    std::string dir_path = "/Users/northkillpd/temp";
    std::string selected_path;

    std::function<void(std::string)> callback;

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
    explicit audio_select(std::function<void(std::string)> callback)
    {
        this->callback = std::move(callback);
        update_file_list();
    }

    virtual ~audio_select()
    {
        ;
    }

    int w = 40;
    int h = 20;

    bool is_loading = false;

    void on_frame()
    {
        ImGui::SetNextWindowPos(ImVec2((float)86, (float)3), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2((float)(w + 1), (float)h + 2), ImGuiCond_Once);

        ImGui::Begin("Audio Select");

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar;

        ImGui::Text(" ");
        ImGui::Text("Workspace:");

        ImGui::Text(" ");

        static char wksp[182] = "/Users/northkillpd/temp/"; ImGui::InputText("", wksp, 128);
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
            ImGui::Text("Selected audio: ");
            ImGui::SameLine(); ImGui::Text("%s", itemies[item_current_idx].filename().c_str());
            selected_path = itemies[item_current_idx];
        }
        ImGui::Text(" ");

        ImGui::Text(" ");
        ImGui::SameLine();

        if (ImGui::BeginListBox("Audio Select", ImVec2(w - 3.f, 10.f)))
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

        ImGui::Text(" ");

        ImGui::Text("                ");
        ImGui::SameLine();
        if (ImGui::Button(is_loading ? " ---- " : " LOAD SELECTED AUDIO "))
        {
            callback(selected_path);
        }

        ImGui::End();
    }
};