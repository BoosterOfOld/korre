#pragma once

#include <string>
#include <cmath>
#include <utility>

#include "imtui/imtui.h"
#include "imtui/imtui-impl-ncurses.h"

#include "wave_source.hpp"
#include "sink.hpp"
#include "portaudio_sink.hpp"
#include "wave_source.hpp"
#include "meter.hpp"
#include "internal_signal.hpp"

class player
{
private:
    wave wav;

    std::shared_ptr<meter> m = nullptr;
    std::shared_ptr<portaudio_sink> pa_sink = nullptr;
    bool running;
public:
    std::shared_ptr<wave_source> ws = nullptr;

    player()
    {
        pa_sink = std::make_shared<portaudio_sink>();
    }

    ~player()
    {
        stop();
    }

    void load(const char *path, bool normalize)
    {
        if (path == nullptr)
        {
            //wav.load_wav("/Users/northkillpd/temp/lg96.wav");
            //wav.load_wav("/Users/northkillpd/temp/rihanna.wav");
            //wav.load_wav("/Users/northkillpd/temp/sweep.wav");
        }

        wav.load_wav(path, normalize);
        //run_analytics();

        PA_SAMPLE_RATE = wav.wav_file.sampleRate;
        PA_BIT_DEPTH = wav.wav_file.bitsPerSample;

        uint32_t num_samples = ((float)wav.wav_file.subchunk2Size / (float)wav.wav_file.numChannels) / ((float)wav.wav_file.bitsPerSample / 8.f);
        PA_SAMPLE_LENGTH = num_samples;
        PA_NUM_CHANNELS = wav.wav_file.numChannels;

        ws = std::make_shared<wave_source>(&wav, wav.wav_file.sampleRate);
        m = std::make_shared<meter>(wav.wav_file.sampleRate);
        ws->connect_to(m, 1, 1);
        m->connect_to(pa_sink, 1, 1);
    }

    void play()
    {
        pa_sink->open();
    }

    void stop()
    {
        pa_sink->plug();
    }

    int width = 80;
    int height = 30;
    int spacer = 2;

    void set_is(std::shared_ptr<internal_signal> isx) const
    {
        ws->is = std::move(isx);
    }

    void on_frame()
    {
        ImGui::SetNextWindowPos(ImVec2((float)3, (float)3), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2((float)(0 + width + 0 + 1), (float)height + 2), ImGuiCond_Once);

        ImGui::Begin("Audio Player");

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar;

        ImGui::Text("");

        ImGui::BeginChild("Col1", ImVec2(28.f, (float)8), false, window_flags);

        ImGui::Text(R"(.------------------------.)");
        ImGui::Text(R"(|\\////////       90 min |)");
        ImGui::Text(R"(| \/  __  ______  __     |)");
        ImGui::Text(R"(|    /  \|\.....|/  \    |)");
        ImGui::Text(R"(|    \__/|/_____|\__/    |)");
        ImGui::Text(R"(| A   ______________     |)");
        ImGui::Text(R"(|    /              \    |)");
        ImGui::Text(R"(|___/_._o________o_._\___|)");

        ImGui::EndChild();
        ImGui::SameLine();
        ImGui::BeginChild("Col2", ImVec2(((float)width/2.f)-10, (float)8), false, window_flags);

        ImGui::Text("");
        //ImGui::Text(("File Format: " + std::string((const char *)wav.wav_file.format, 4)).c_str());

        ImGui::Text("Audio Format:"); ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(4 / 7.0f, 0.6f, 0.8f));
        ImGui::Text((std::string((const char *)wav.wav_file.format, 4) + "/" + std::string((wav.wav_file.audioFormat == 1 ? "PCM" : "Non-PCM"))).c_str());
        ImGui::PopStyleColor();

        ImGui::Text("Sampling Rate:"); ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(4 / 7.0f, 0.6f, 0.8f));
        ImGui::Text("%s", std::to_string(wav.wav_file.sampleRate).c_str());
        ImGui::PopStyleColor();

        ImGui::Text("Bit Depth:"); ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(4 / 7.0f, 0.6f, 0.8f));
        ImGui::Text("%s", std::to_string(wav.wav_file.bitsPerSample).c_str());
        ImGui::PopStyleColor();

        ImGui::Text("Channels:"); ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(4 / 7.0f, 0.6f, 0.8f));
        ImGui::Text("%s", std::to_string(wav.wav_file.numChannels).c_str());
        ImGui::PopStyleColor();

        ImGui::Text("Data Size:"); ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(4 / 7.0f, 0.6f, 0.8f));
        ImGui::Text("%s", (std::to_string((float)wav.wav_file.subchunk2Size / 1048576.f) + " MB").c_str());
        ImGui::PopStyleColor();

        int num_samples = ((float)wav.wav_file.subchunk2Size / (float)wav.wav_file.numChannels) / ((float)wav.wav_file.bitsPerSample / 8.f);

        ImGui::Text("Number of Samples:"); ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(4 / 7.0f, 0.6f, 0.8f));
        ImGui::Text("%s", std::to_string( num_samples ).c_str());
        ImGui::PopStyleColor();

        ImGui::Text("Duration:"); ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(4 / 7.0f, 0.6f, 0.8f));
        ImGui::Text("%s", (std::to_string( num_samples / (float)wav.wav_file.sampleRate ) + " s").c_str());
        ImGui::PopStyleColor();

        /*
        ImGui::Text(("Lowest Sample Value: " + std::to_string(min_value)).c_str());
        ImGui::Text(("Minimal Sample Value: " + std::to_string(lower_limit)).c_str());
        ImGui::Text(("Highest Sample Value: " + std::to_string(max_value)).c_str());
        ImGui::Text(("Maximal Sample Value: " + std::to_string(upper_limit)).c_str());*/

        if (ws->is != nullptr)
        {
            ImGui::EndChild();
            ImGui::SameLine();
            ImGui::BeginChild("Col3", ImVec2((float) 20, (float) 8), false, window_flags);

            ImGui::Text("");
            ImGui::Text("Internal Signal:");

            ImGui::Text("Name:"); ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(4 / 7.0f, 0.6f, 0.8f));
            ImGui::Text("%s", ws->is->name.c_str());
            ImGui::PopStyleColor();

            ImGui::Checkbox("Use this signal", &ws->use_is);
        }

        ImGui::EndChild();

        ImGui::Text("");

        double i = pa_sink->is_open() ? 55.f / 360.f : 2.f / 7.f;
        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i, 0.6f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(i, 0.7f, 0.7f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(i, 0.8f, 0.8f));
        if (ImGui::Button(pa_sink->is_open() ? " PAUSE " : " >PLAY "))
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
        ImGui::PopStyleColor(3);

        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.6f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.7f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f, 0.8f, 0.8f));
        if (ImGui::Button(" STOP "))
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
        ImGui::PopStyleColor(3);

        ImGui::SameLine();

        char buf[32];
        sprintf(buf, "%.0f s", (float)m->pos/(float)PA_SAMPLE_RATE);
        ImGui::SetNextItemWidth(width);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(247.f / 360.f, 1.0f, 0.1));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::HSV(247.f / 360.f, 1.0f, 0.4));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, (ImVec4)ImColor::HSV(247.f / 360.f, 1.0f, 0.4));
        ImGui::PushStyleColor(ImGuiCol_SliderGrab, (ImVec4)ImColor::HSV(54.f / 360.f, 1.0f, 0.9f));
        ImGui::SetNextItemWidth(width - 15);
        ImGui::SliderInt("", &PA_T, 0.0f, PA_SAMPLE_LENGTH, buf);
        ImGui::PopStyleColor(4);

        ImGui::Text("");

        /*
        char buf[32];
        sprintf(buf, "%.0f s", (float)m->pos/(float)PA_SAMPLE_RATE);
        //ImGui::Text(" "); ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, (ImVec4)ImColor::HSV(4 / 7.0f, 0.6f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(4 / 7.0f, 0.6f, 0.6f));
        ImGui::ProgressBar((float)m->pos/(float)m->pos_max, ImVec2(width, 1), buf);
        ImGui::PopStyleColor(3);
         */

        //ImGui::Text("");


        i = 3;
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, (ImVec4)ImColor::HSV(i / 7.0f, 0.6f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(0, 0, 0));
        ImGui::PlotLines("", m->compacted, m->compact_columns, 0, "FREQUENCY SPECTRUM", 0.f, 1.0,ImVec2(width, 15));
        ImGui::PopStyleColor(2);

        ImGui::Text(" ");

        ImGui::Text("10Hz     50     100     200     400        1k      2k      4k      8k    13 18");

        ImGui::Text("");

        //ImGui::Spacing();

        ImGui::End();
    }


};