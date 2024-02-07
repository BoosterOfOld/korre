#include "elements/audio_select.h"

bool LOADER_NORMALIZE = false;

void audio_select::update_file_list()
{
    itemies.clear();
    for (const auto & entry : std::filesystem::directory_iterator(dir_path))
    {
        if (entry.path().extension() == ".wav"
            || entry.path().extension() == ".flac"
               || entry.path().extension() == ".mp3"
            || entry.path().extension() == ".txt"
        )
        {
            itemies.emplace_back(entry.path());
        }
    }
}

audio_select::audio_select(
        std::function<void(std::filesystem::path)> callback,
        std::function<void(std::string)> ir_callback
)
{
    this->callback = std::move(callback);
    this->ir_callback = std::move(ir_callback);
    update_file_list();
}

audio_select::~audio_select()
{
    ;
}

void audio_select::on_frame()
{
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::SetNextWindowPos(ImVec2((float)83, (float)15), ImGuiCond_Once);

    windowth(w, h, "Audio_Select", [this]()->void {render_content();});
}

void audio_select::render_content()
{

    ImGui::Text(" ");
    ImGui::Text("Workspace:");

    ImGui::Text(" ");

    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor(30, 30, 30));
    ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(255,255,104));
    static char wksp[182] = "/Users/northkillpd/temp/";
    ImGui::InputText("", wksp, 128);
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
    if (ImGui::BeginListBox("Audio Select", ImVec2(w - 2.f, h - 14.f)))
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