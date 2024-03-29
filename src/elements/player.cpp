#include "elements/player.h"

#include <string>
#include <utility>

#include "imtui/imtui.h"
#include "windowth.h"

player::player()
{
    pa_sink = std::make_shared<portaudio_sink>();
}

player::~player()
{
    stop();
}

void player::load(const char *path, bool normalize)
{
    auto dot = strrchr(path, '.');
    if (dot && !strcmp(dot, ".flac"))
    {
        wav.load_flac(path, normalize);
    }
    else if (dot && !strcmp(dot, ".mp3"))
    {
        wav.load_mp3(path, normalize);
    }
    else // wav
    {
        wav.load_wav(path, normalize);
    }

    PA_SAMPLE_RATE = wav.wav_file.sampleRate;
    PA_BIT_DEPTH = wav.wav_file.bitsPerSample;

    uint32_t num_samples = wav.l->size(); //((float)wav.wav_file.subchunk2Size / (float)wav.wav_file.numChannels) / ((float)wav.wav_file.bitsPerSample / 8.f);
    PA_SAMPLE_LENGTH = num_samples;
    PA_NUM_CHANNELS = wav.wav_file.numChannels;

    ws = std::make_shared<wave_source>(&wav, wav.wav_file.sampleRate);
    m = std::make_shared<meter>(wav.wav_file.sampleRate);
    ws->connect_to(m, 1, 1);
    m->connect_to(pa_sink, 1, 1);
}

void player::play()
{
    pa_sink->open();
}

void player::stop()
{
    pa_sink->plug();
}

void player::on_frame()
{
    ImGui::SetNextWindowPos(ImVec2((float)2, (float)2), ImGuiCond_Once);

    windowth(width, height, "Audio_Player", [this]()->void {render_content();});
}

void player::do_play()
{
    pa_sink->open();
    running = true;
}

void player::render_content()
{
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar;

    ImGui::Text("  ");ImGui::SameLine();
    ImGui::BeginChild("Col1", ImVec2(28.f, (float)8), false, window_flags);

    ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(255,255,255));
    ImGui::Text(R"(.------------------------.)");
    ImGui::Text(R"(|\\////////       90 min |)");
    ImGui::Text(R"(| \/  __  ______  __     |)");
    ImGui::Text(R"(|    /  \|\.....|/  \    |)");
    ImGui::Text(R"(|    \__/|/_____|\__/    |)");
    ImGui::Text(R"(| A   ______________     |)");
    ImGui::Text(R"(|    /              \    |)");
    ImGui::Text(R"(|___/_._o________o_._\___|)");
    ImGui::PopStyleColor();

    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::SetCursorPos(ImVec2(28,0));
    ImGui::BeginChild("Col2", ImVec2(((float)width/2.f)-10, (float)9), false, window_flags);

    ImGui::Text("╒══╡Track_Details╞══════════╕");
    ImGui::Text("│                           │");
    ImGui::Text("│                           │");
    ImGui::Text("│                           │");
    ImGui::Text("│                           │");
    ImGui::Text("│                           │");
    ImGui::Text("│                           │");
    ImGui::Text("│                           │");
    ImGui::Text("╘═══════════════════════════╛");

    ImGui::SetCursorPos(ImVec2(0,1));

    ImGui::Text("Audio Format: "); ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(255,255,255));
    if (!wav.wav_file.extra.empty())
    {
        ImGui::Text("MP3 ");
        ImGui::SameLine(); ImGui::Text(wav.wav_file.extra.c_str());
        ImGui::SameLine(); ImGui::Text( "kbps");
    }
    else
    {
        ImGui::Text((std::string((const char *)wav.wav_file.format, 4) + "/" + std::string((wav.wav_file.audioFormat == 1 ? "PCM" : "Non-PCM"))).c_str());
    }
    ImGui::PopStyleColor();

    ImGui::Text(" Sampling Rate: "); ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(255,255,255));
    ImGui::Text("%s", std::to_string(wav.wav_file.sampleRate).c_str());
    ImGui::PopStyleColor();

    ImGui::Text(" Bit Depth: "); ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(255,255,255));
    ImGui::Text("%s", std::to_string(wav.wav_file.bitsPerSample).c_str());
    ImGui::PopStyleColor();

    ImGui::Text(" Channels: "); ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(255,255,255));
    ImGui::Text("%s", std::to_string(wav.wav_file.numChannels).c_str());
    ImGui::PopStyleColor();

    ImGui::Text(" Data Size: "); ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(255,255,255));
    ImGui::Text("%s", (std::to_string((float)wav.wav_file.subchunk2Size / 1048576.f) + " MB").c_str());
    ImGui::PopStyleColor();

    int num_samples = ((float)wav.wav_file.subchunk2Size / (float)wav.wav_file.numChannels) / ((float)wav.wav_file.bitsPerSample / 8.f);

    ImGui::Text(" Number of Samples: "); ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(255,255,255));
    ImGui::Text("%s", std::to_string( num_samples ).c_str());
    ImGui::PopStyleColor();

    ImGui::Text(" Duration: "); ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(255,255,255));
    ImGui::Text("%s", (std::to_string( num_samples / (float)wav.wav_file.sampleRate ) + " s").c_str());
    ImGui::PopStyleColor();

    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::SetCursorPos(ImVec2(57,0));
    ImGui::BeginChild("Col3", ImVec2((float) 21, (float) 9), false, window_flags);

    ImGui::Text("╒══╡DSP╞═══════════╕");
    ImGui::Text("│                  │");
    ImGui::Text("│                  │");
    ImGui::Text("│                  │");
    ImGui::Text("│                  │");
    ImGui::Text("│                  │");
    ImGui::Text("│                  │");
    ImGui::Text("│                  │");
    ImGui::Text("╘═══════╡"); ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4) ImColor(60, 60, 60));
    ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4) ImColor(255, 255, 104));
    if(ImGui::RadioButton("Bypass", ws->selected_signal == -1))
    {
        ws->selected_signal = -1;
    }; ImGui::SameLine();
    ImGui::PopStyleColor(2);
    ImGui::Text("╞══╛");

    if (!ws->iss.empty())
    {
        ImGui::SetCursorPos(ImVec2(-1,1));

        static int ss = 0;
        ss = 0;
        for(auto sig : ws->iss)
        {
            ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4) ImColor(60, 60, 60));
            ImGui::Text(" ");
            ImGui::SameLine();
            ImGui::PushID(ss+ 1000);
            if(ImGui::RadioButton(std::to_string(ss).c_str(), ws->selected_signal == ss))
            {
                ws->selected_signal = ss;
            }
            ImGui::PopID();
            ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4) ImColor(255, 255, 104));
            ImGui::SameLine();
            ImGui::Text("%s ", sig->name.c_str());
            ImGui::SameLine();
            ImGui::PopStyleColor();
            ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4) ImColor(255, 130, 130));
            ImGui::PushID(ss);
            if(ImGui::Button("✘"))
            {
                ws->selected_signal = -1;
                ws->iss.erase(std::next(ws->iss.begin(), ss), std::next(ws->iss.begin(), ss+1));
                ImGui::PopStyleColor(2);
                ImGui::PopID();
                break;
            }
            ImGui::PopID();
            ImGui::PopStyleColor(2);
            ++ss;
        }
    }

    ImGui::EndChild();

    ImGui::Text(" ");ImGui::SameLine();
    if (ImGui::Button(pa_sink->is_open() ? " ⏸ \n PAUSE \n" : " ▶ \n PLAY  \n"))
    {
        running = !running;

        if (pa_sink->is_open() && !running)
        {
            running = false;
            pa_sink->plug();
        }
        else if (!pa_sink->is_open() && running)
        {
            pa_sink->open();
            running = true;
        }
    }

    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(255,255,255));

    ImGui::Text(" ");ImGui::SameLine();
    if (ImGui::Button(" ⏹ \n STOP \n"))
    {
        running = !running;

        if (pa_sink->is_open() && !running)
        {
            running = false;
            pa_sink->plug();
        }
        PA_T = 0;
        m->pos = 0;
    }

    ImGui::SameLine(); ImGui::Text(" "); ImGui::SameLine();

    char buf[32];
    sprintf(buf, "\n%.0f s", (float)m->pos/(float)PA_SAMPLE_RATE);
    ImGui::SetNextItemWidth(width);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor(30, 30, 30));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor(60, 60, 60));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, (ImVec4)ImColor(60, 60, 60));
    ImGui::PushStyleColor(ImGuiCol_SliderGrab, (ImVec4)ImColor(255,255,104));

    ImGui::SetNextItemWidth(width - 19);
    ImGui::SliderInt("", &PA_T, 0.0f, PA_SAMPLE_LENGTH, buf);
    ImGui::PopStyleColor(4);

    ImGui::Text("");

    static auto num_col = (ImVec4)ImColor(120,220,238);

    ImGui::Text("╒════════════════════════════╡Frequency_Spectrum╞════════════════════════════╕");
    ImGui::Text("│                                                                            │");
    ImGui::Text("│                                                                            │");
    ImGui::Text("│                                                                            │");
    ImGui::Text("│                                                                            │");
    ImGui::Text("│                                                                            │");
    ImGui::Text("│                                                                            │");
    ImGui::Text("│                                                                            │");
    ImGui::Text("│                                                                            │");
    ImGui::Text("│                                                                            │");
    ImGui::Text("│                                                                            │");
    ImGui::Text("│                                                                            │");
    ImGui::Text("│                                                                            │");
    ImGui::Text("│                                                                            │");
    ImGui::Text("│                                                                            │");
    ImGui::Text("│                                                                            │");
    ImGui::Text("│                                                                            │");
    ImGui::Text("│                                                                            │");
    ImGui::Text("├╢"); ImGui::SameLine(); ImGui::PushStyleColor(ImGuiCol_Text, num_col); ImGui::SameLine();ImGui::Text("20Hz"); ImGui::SameLine(); ImGui::PopStyleColor();
    ImGui::Text("╟──╢"); ImGui::SameLine(); ImGui::PushStyleColor(ImGuiCol_Text, num_col); ImGui::SameLine();ImGui::Text("50"); ImGui::SameLine(); ImGui::PopStyleColor();
    ImGui::Text("╟───╢"); ImGui::SameLine(); ImGui::PushStyleColor(ImGuiCol_Text, num_col); ImGui::SameLine();ImGui::Text("100"); ImGui::SameLine(); ImGui::PopStyleColor();
    ImGui::Text("╟───╢"); ImGui::SameLine(); ImGui::PushStyleColor(ImGuiCol_Text, num_col); ImGui::SameLine();ImGui::Text("200"); ImGui::SameLine(); ImGui::PopStyleColor();
    ImGui::Text("╟──╢"); ImGui::SameLine(); ImGui::PushStyleColor(ImGuiCol_Text, num_col); ImGui::SameLine();ImGui::Text("400"); ImGui::SameLine(); ImGui::PopStyleColor();
    ImGui::Text("╟──────╢"); ImGui::SameLine(); ImGui::PushStyleColor(ImGuiCol_Text, num_col); ImGui::SameLine();ImGui::Text("1k"); ImGui::SameLine(); ImGui::PopStyleColor();
    ImGui::Text("╟───╢"); ImGui::SameLine(); ImGui::PushStyleColor(ImGuiCol_Text, num_col); ImGui::SameLine();ImGui::Text("2k"); ImGui::SameLine(); ImGui::PopStyleColor();
    ImGui::Text("╟────╢"); ImGui::SameLine(); ImGui::PushStyleColor(ImGuiCol_Text, num_col); ImGui::SameLine();ImGui::Text("4k"); ImGui::SameLine(); ImGui::PopStyleColor();
    ImGui::Text("╟────╢"); ImGui::SameLine(); ImGui::PushStyleColor(ImGuiCol_Text, num_col); ImGui::SameLine();ImGui::Text("8k"); ImGui::SameLine(); ImGui::PopStyleColor();
    ImGui::Text("╟──╢"); ImGui::SameLine(); ImGui::PushStyleColor(ImGuiCol_Text, num_col); ImGui::SameLine();ImGui::Text("13"); ImGui::SameLine(); ImGui::PopStyleColor();
    ImGui::Text("╟╢"); ImGui::SameLine(); ImGui::PushStyleColor(ImGuiCol_Text, num_col); ImGui::SameLine();ImGui::Text("2"); ImGui::SameLine(); ImGui::PopStyleColor();

    ImGui::Text("┤");

    ImGui::Text("╘╩════╩══╩══╩═══╩═══╩═══╩═══╩══╩═══╩══════╩══╩═══╩══╩════╩══╩════╩══╩══╩══╩╩═╛");

    ImGui::SetCursorPos(ImVec2(-1,14));

    ImGui::Text("  ");ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_PlotLines, (ImVec4)ImColor(251,244,224));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor(0, 0, 0));
    ImGui::PlotLines("", m->compacted, m->compact_columns, 0, "", 0.f, 1.0,ImVec2(width-6, 15));
    ImGui::PopStyleColor(2);

    ImGui::SetCursorPos(ImVec2(-1,14+15+5));

    ImGui::Text("blah");

    ImGui::PopStyleColor();
}
