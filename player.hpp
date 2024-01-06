#pragma once

#include <string>
#include <cmath>

#include "imtui/imtui.h"
#include "imtui/imtui-impl-ncurses.h"

#include "wave_source.hpp"
#include "sink.hpp"
#include "portaudio_sink.hpp"
#include "wave_source.hpp"
#include "meter.hpp"

class player
{
private:
    wave wav;
    std::shared_ptr<wave_source> ws = nullptr;
    std::shared_ptr<meter> m = nullptr;
    std::shared_ptr<portaudio_sink> pa_sink = nullptr;
    bool running;

    double min_value = 99999999999999;
    double max_value = -99999999999999;
    int upper_limit = 0;
    int lower_limit = 0;

    void run_analytics()
    {
        for (double val : *wav.lr)
        {
            if (val < min_value)
            {
                min_value = val;
            }
            if (val > max_value)
            {
                max_value = val;
            }
        }

        if (wav.wav_file.bitsPerSample == 8)
        {
            lower_limit = 0;
            upper_limit = sizeof(char);
        }
        else
        {
            lower_limit = -pow(2.f,(float)wav.wav_file.bitsPerSample)/2.f;
            upper_limit = pow(2,(float)wav.wav_file.bitsPerSample)/2.f;
        }
    }

public:
    player()
    {
        pa_sink = std::make_shared<portaudio_sink>();
    }

    ~player()
    {
        stop();
    }

    void load(const char *path)
    {
        if (path == nullptr)
        {
            //wav.load_wav("/Users/northkillpd/temp/lg96.wav");
            //wav.load_wav("/Users/northkillpd/temp/rihanna.wav");
            //wav.load_wav("/Users/northkillpd/temp/sweep.wav");
        }

        wav.load_wav(path);
        run_analytics();

        PA_SAMPLE_RATE = wav.wav_file.sampleRate;
        PA_BIT_DEPTH = wav.wav_file.bitsPerSample;

        uint32_t num_samples = ((float)wav.wav_file.subchunk2Size / (float)wav.wav_file.numChannels) / ((float)wav.wav_file.bitsPerSample / 8.f);
        PA_SAMPLE_LENGTH = num_samples;
        PA_NUM_CHANNELS = wav.wav_file.numChannels;

        ws = std::make_shared<wave_source>(&wav, wav.wav_file.sampleRate, upper_limit);
        m = std::make_shared<meter>(wav.wav_file.sampleRate, max_value);
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
    int height = 40;
    int spacer = 2;

    void on_frame()
    {
        ImGui::SetNextWindowPos(ImVec2((float)3, (float)3), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2((float)(0 + width + 0 + 1), (float)height + 2), ImGuiCond_Once);

        ImGui::Begin("Wave File");

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar;

        /*
        ImGui::BeginChild("Col1", ImVec2((float)left_panel_width, (float)height), false, window_flags);

        ImGui::PlotHistogram("", rms_left, 1, 0, nullptr, 0.1f, 1.f, ImVec2(3.f, height));
        ImGui::SameLine();
        ImGui::PlotHistogram("", rms_right, 1, 0, nullptr, 0.1f, 1.f, ImVec2(3.f, height));

        ImGui::EndChild();
        ImGui::SameLine(0, 1);
*/
        //ImGui::BeginChild("Col2", ImVec2((float)width, (float)height), false, window_flags);

        //ImGui::TextWrapped("This Kora application emulates the DOHM noise machine.");

        ImGui::Text("");

        ImGui::BeginChild("Col1", ImVec2((float)width/2.f, (float)8), false, window_flags);

        ImGui::Text(("File Format: " + std::string((const char *)wav.wav_file.format, 4)).c_str());
        ImGui::Text(("Audio Format: " + std::string((wav.wav_file.audioFormat == 1 ? "PCM" : "Non-PCM")) ).c_str());
        ImGui::Text(("Sampling Rate: " + std::to_string(wav.wav_file.sampleRate)).c_str());
        ImGui::Text(("Bit Depth: " + std::to_string(wav.wav_file.bitsPerSample)).c_str());
        ImGui::Text(("Channels: " + std::to_string(wav.wav_file.numChannels)).c_str());
        ImGui::Text(("Data Size: " + std::to_string((float)wav.wav_file.subchunk2Size / 1048576.f) + " MB").c_str());
        int num_samples = ((float)wav.wav_file.subchunk2Size / (float)wav.wav_file.numChannels) / ((float)wav.wav_file.bitsPerSample / 8.f);
        ImGui::Text(("Number of Samples: " + std::to_string( num_samples )).c_str());
        ImGui::Text(("Duration: " + std::to_string( num_samples / (float)wav.wav_file.sampleRate ) + " s").c_str());

        ImGui::EndChild();
        ImGui::SameLine();
        ImGui::BeginChild("Col2", ImVec2((float)width/2.f, (float)8), false, window_flags);

        ImGui::Text(("Lowest Sample Value: " + std::to_string(min_value)).c_str());
        ImGui::Text(("Minimal Sample Value: " + std::to_string(lower_limit)).c_str());
        ImGui::Text(("Highest Sample Value: " + std::to_string(max_value)).c_str());
        ImGui::Text(("Maximal Sample Value: " + std::to_string(upper_limit)).c_str());

        ImGui::EndChild();

        ImGui::Text("");

        if (ImGui::Button(pa_sink->is_open() ? " STOP " : " PLAY "))
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

        ImGui::Text("");

        //ImGui::SliderFloat("slider float", &f1, 0.0f, 1.0f, "ratio = %.3f");

        char buf[32];
        sprintf(buf, "%.0f", (float)m->pos);
        ImGui::ProgressBar((float)m->pos/(float)m->pos_max, ImVec2(width, 1), buf);

        ImGui::Text("");


        // nn/4
        ImGui::PlotLines("", m->wind_avg, m->bin_range, 0, "FREQUENCY SPECTRUM", 0.f, m->ceiling, ImVec2(width, 15));

        ImGui::Text(" ");
        //ImGui::Text(" ");

        if (m->last_bin_freq == 20000)
        {
            ImGui::Text("10                5|k                 10k                 15k               20k");
        }
        else if (m->last_bin_freq == 16000)
        {
            ImGui::Text("10                4|k                 8|k                 12k               16k");
        }
        else if (m->last_bin_freq == 12000)
        {
            ImGui::Text("10                3|k                 6|k                 9|k               12k");
        }
        else if (m->last_bin_freq == 8000)
        {
            ImGui::Text("10                2|k                 4|k                 6|k               8k|");
        }

        static int f_range_type = 0;
        ImGui::Text("Upper limit: "); ImGui::SameLine();
        ImGui::RadioButton("20 kHz", &f_range_type, 0); ImGui::SameLine();
        ImGui::RadioButton("16 kHz", &f_range_type, 1); ImGui::SameLine();
        ImGui::RadioButton("12 kHz", &f_range_type, 2); ImGui::SameLine();
        ImGui::RadioButton("8 kHz", &f_range_type, 3);

        switch (f_range_type)
        {
            case 0:
                if (m->last_bin_freq != 20000)
                {
                    m->update_range(20000);
                }
                break;
            case 1:
                if (m->last_bin_freq != 16000)
                {
                    m->update_range(16000);
                }
                break;
            case 2:
                if (m->last_bin_freq != 12000)
                {
                    m->update_range(12000);
                }
                break;
            case 3:
                if (m->last_bin_freq != 8000)
                {
                    m->update_range(8000);
                }
                break;
        }

        ImGui::Spacing();



        ImGui::End();
    }


};