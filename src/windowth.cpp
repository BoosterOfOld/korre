#include "windowth.h"

#include "imtui/imtui.h"
#include "imtui/imtui-impl-ncurses.h"

void windowth(int width, int height, const char *name, std::function<void(void)> render_content)
{
    ImGui::SetNextWindowSize(ImVec2((float)(width), (float)height), ImGuiCond_Once);

    //ImGui::PushStyleColor(ImGuiCol_WindowBg, (ImVec4)ImColor(0,0,156)); // Blue
    //ImGui::PushStyleColor(ImGuiCol_WindowBg, (ImVec4)ImColor(0,0,122)); // Blue
    ImGui::PushStyleColor(ImGuiCol_WindowBg, (ImVec4)ImColor(0,0,0));
    // ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(120,220,238)); // Blue NC line
    ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(255,255,255));

    ImGuiWindowFlags window_flags =
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoScrollWithMouse |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoDecoration |
            //ImGuiWindowFlags_AlwaysUseWindowPadding |
            //ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoResize;

    /*
            case (uint8_t)*D_HORT: return u8"═";
            case (uint8_t)*D_VERT: return u8"║";
            case (uint8_t)*D_TOP_LEFT: return u8"╔";
            case (uint8_t)*D_TOP_RIGHT: return u8"╗";
            case (uint8_t)*D_BOTTOM_LEFT: return u8"╚";
            case (uint8_t)*D_BOTTOM_RIGHT: return u8"╝";
            case (uint8_t)*D_VERT_D_LEFT_SPLIT: return u8"╣";
            case (uint8_t)*D_VERT_D_RIGHT_SPLIT: return u8"╠";
            case (uint8_t)*D_HORT_D_BOTTOM_SPLIT: return u8"╦";
            case (uint8_t)*D_HORT_D_TOP_SPLIT: return u8"╩";
            case (uint8_t)*D_CROSS: return u8"╬";
     */

    static bool is_open = true;
    ImGui::Begin(name, &is_open, window_flags);

    // ###############
    // ##### TOP #####
    // ###############
    std::string appendage;
    appendage.append(u8"╔");
    for(int i=0; i<width-3; ++i)
    {
        appendage.append(u8"═");
    }
    appendage.append(u8"╗");
    ImGui::Text(appendage.data());

    // ################
    // ##### LEFT #####
    // ################

    std::string vert = u8"║";
    std::string pad(width - 3, u8' ');
    std::string line;
    line.append(vert);
    line.append(pad);
    line.append(vert);
    for(int i=0; i<height-2; ++i)
    {
        ImGui::Text(line.data());
    }

    // ##################
    // ##### BOTTOM #####
    // ##################
    appendage.clear();
    appendage.append(u8"╚");
    for(int i=0; i<width-3; ++i)
    {
        appendage.append(u8"═");
    }
    appendage.append(u8"╝");
    ImGui::Text(appendage.data());

    ImGui::SetCursorPos(ImVec2(1,0));
    ImGui::Text(u8"╡"); ImGui::SameLine();
    ImGui::Button(" "); ImGui::SameLine();
    ImGui::Text(u8"╞");

    ImGui::SetCursorPos(ImVec2(7,0));
    ImGui::Text(u8"╡"); ImGui::SameLine();
    ImGui::Text(name); ImGui::SameLine();
    ImGui::Text(u8"╞");

    ImGui::SetCursorPos(ImVec2(1,1));
    ImGui::BeginChild("wnd_content", ImVec2(width - 1, height), false, window_flags);

    //ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(255,255,104));
    ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(120,220,238));
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(80, 80, 80));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(120, 120, 120));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor(71, 167, 169));
    render_content();
    ImGui::PopStyleColor(4);

    ImGui::EndChild();

    ImGui::End();

    ImGui::PopStyleColor(2);
}