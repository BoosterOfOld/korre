#include "elements/queue.h"

#include "windowth.h"
#include "imtui/imtui.h"
#include "include/sinks/portaudio_sink.h"

queue::queue(
        std::function<void(std::filesystem::path)> loader,
        std::function<void(void)> unloader,
        std::function<void(void)> play
)
{
    this->loader = std::move(loader);
    this->unloader = std::move(unloader);
    this->play = std::move(play);

    PA_SIG_TRACK_END = [this]() -> void {
        on_track_end();
    };
}

void queue::on_track_end()
{
    if (q.empty())
    {
        unloader();
        return;
    }

    if (!repeat1)
    {
        hist.push_front(q.front());
        q.pop_front();
    }
    if (q.empty())
    {
        unloader();
    }
    else
    {
        loader(q.front());
        play();
    }
}

void queue::enqueue_track_or_playlist(std::filesystem::path path)
{
    auto pre_empty = q.empty();

    if (path.extension() == ".txt")
    { // Playlist
        std::ifstream file(path);
        std::string track_path;
        while (std::getline(file, track_path))
        {
            q.emplace_back(track_path);
        }
    }
    else
    { // Audio file
        q.push_back(path);
    }

    if (pre_empty && !q.empty())
    {
        loader(q.front());
    }
}

void queue::shuffle_queue(bool yes)
{
    if (yes)
    {
        q_backup = q;
        hist_backup = hist;

        std::default_random_engine rng(rd());

        if (!q.empty())
        {
            std::shuffle(std::begin(q), std::end(q), rng);
            loader(q.front());
        }
        if (!hist.empty())
        {
            std::shuffle(std::begin(hist), std::end(hist), rng);
        }

        return;
    }

    q = q_backup;
    hist = hist_backup;
}

void queue::on_frame()
{
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::SetNextWindowPos(ImVec2((float) 83, (float) 2), ImGuiCond_Once);

    windowth(w, h, "Audio_Queue", [this]() -> void
    { render_content(); });
}

void queue::render_content()
{
    ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(255,255,255));

    ImGui::Text("");
    ImGui::Text(" ");ImGui::SameLine();
    if(ImGui::Button("  ◀◀ \n PREV \n"))
    {
        if (!hist.empty())
        {
            q.push_front(hist.front());
            hist.pop_front();
            if (q.empty())
            {
                unloader();
            }
            else
            {
                loader(q.front());
                play();
            }
        }
    }
    ImGui::SameLine(); ImGui::Text(" "); ImGui::SameLine();
    if(ImGui::Button("  ▶▶ \n NEXT \n"))
    {
        if (!q.empty())
        {
            hist.push_front(q.front());
            q.pop_front();
            if (q.empty())
            {
                unloader();
            }
            else
            {
                loader(q.front());
                play();
            }
        }
    }
    ImGui::SameLine(); ImGui::Text("  "); ImGui::SameLine();


    ImGui::SetCursorPos(ImVec2(29,1));
    if(ImGui::Button("  CLR  "))
    {
        q.clear();
        unloader();
    }
    ImGui::SetCursorPos(ImVec2(29,3));
    if(ImGui::Button("  DEL  "))
    {
        if (!q.empty())
        {
            q.pop_back();

            if (q.empty())
            {
                unloader();
            }
        }
    }

    // Shuffle
    ImGui::SetCursorPos(ImVec2(29,5));
    ImGui::PushStyleColor(ImGuiCol_Text, shuffle ?
                                         (ImVec4)ImColor(0,255,0) :
                                         (ImVec4)ImColor(255,0,0));
    ImGui::Text("█");
    ImGui::PopStyleColor();
    ImGui::SameLine();
    if(ImGui::Button(" SHF  "))
    {
        shuffle = !shuffle;
        shuffle_queue(shuffle);
    }

    // Repeat 1
    ImGui::SetCursorPos(ImVec2(29,7));
    ImGui::PushStyleColor(ImGuiCol_Text, repeat1 ?
                                         (ImVec4)ImColor(0,255,0) :
                                         (ImVec4)ImColor(255,0,0));
    ImGui::Text("█");
    ImGui::PopStyleColor();
    ImGui::SameLine();
    if(ImGui::Button(" RPT1 "))
    {
        repeat1 = !repeat1;
    }


    ImGui::SetCursorPos(ImVec2(29,9 ));
    ImGui::Button("  OTH  ");

    ImGui::SetCursorPos(ImVec2(0, 4));

    ImGui::Text(" "); ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_ChildBg, (ImVec4)ImColor(30,30,30));
    ImGui::BeginChild("qu", ImVec2(29, 6));



    // Bkwd

    if (hist.size() >= 2)
    {
        std::string x = "┃  "; x.append(hist[1].filename());
        ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4) ImColor(179, 179, 179));
        ImGui::Text(x.c_str());
        ImGui::PopStyleColor();
    }
    else
    {
        ImGui::Text(" ");
    }

    if (hist.size() >= 1)
    {
        std::string x = "┃  "; x.append(hist[0].filename());
        ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4) ImColor(221, 221, 138));
        ImGui::Text(x.c_str());
        ImGui::PopStyleColor();
    }
    else
    {
        ImGui::Text(" ");
    }

    // Fwd

    if (q.size() >= 1)
    {
        std::string x = "┣━ "; x.append(q[0].filename()); // ▶⏸
        ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4) ImColor(255, 255, 104));
        ImGui::Text(x.c_str());
        ImGui::PopStyleColor();
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4) ImColor(255, 255, 104));
        ImGui::Text("┣━ N/A");
        ImGui::PopStyleColor();
    }

    if (q.size() >= 2)
    {
        std::string x = "┃  "; x.append(q[1].filename());
        ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4) ImColor(221, 221, 138));
        ImGui::Text(x.c_str());
        ImGui::PopStyleColor();
    }

    if (q.size() >= 3)
    {
        std::string x = "┃  "; x.append(q[2].filename());
        ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(179, 179, 179));
        ImGui::Text(x.c_str());
        ImGui::PopStyleColor();
    }

    if (q.size() >= 4)
    {
        std::string x = "┖  +";
        x.append(std::to_string(q.size() - 3));
        x.append(". ...");
        ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4) ImColor(140, 140, 140));
        ImGui::Text(x.c_str());
        ImGui::PopStyleColor();
    }

    ImGui::EndChild();
    ImGui::PopStyleColor();

    ImGui::PopStyleColor();
}