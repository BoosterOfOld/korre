#pragma once

#include <string>
#include <utility>
#include <vector>
#include <thread>

#include "meter.hpp"
#include "wave_source.hpp"
#include "windowth.hpp"

#include "imtui/imtui.h"
#include "imtui/imtui-impl-ncurses.h"

#include "armadillo"

class dsp_convolver
{
private:
    wave wav;

    std::shared_ptr<meter> m;
    std::unique_ptr<std::thread> visualizer;
    std::function<std::shared_ptr<wave_source>(void)> obtain_input;
    std::function<void(std::shared_ptr<internal_signal>)> yield_signal;
    std::function<void(void)> close;
    std::unique_ptr<std::thread> worker;

    bool stop = false;


    static void *worker_run(dsp_convolver *dc)
    {
        dc->in_progress = true;
        dc->right_channel = false;

        auto sigl = arma::conv_to<arma::rowvec>::from(*(dc->wsi->w->l));
        auto irl = arma::conv_to<arma::rowvec>::from(*(dc->ws->w->l));

        auto sigr = arma::conv_to<arma::rowvec>::from(*(dc->wsi->w->r));
        auto irr = arma::conv_to<arma::rowvec>::from(*(dc->ws->w->r));

        auto xl = arma::conv(sigl, irl, "same");
        auto xr = arma::conv(sigr, irr, "same");

        auto res_l = arma::conv_to<std::vector<double>>::from(xl);
        dc->right_channel = true;
        auto res_r = arma::conv_to<std::vector<double>>::from(xr);

        dc->convolved_l = std::make_shared<std::vector<double>>(res_l);
        dc->convolved_r = std::make_shared<std::vector<double>>(res_r);
        dc->in_progress = false;

        dc->yield();
    }

    /*
    static void *worker_run(dsp_convolver *dc)
    {
        dc->in_progress = true;
        dc->right_channel = false;

        auto sigl = dc->wsi->w->l;
        auto irl = dc->ws->w->l;

        auto sigr = dc->wsi->w->r;
        auto irr = dc->ws->w->r;

        auto xl = dc->convo_sum(sigl, irl);
        dc->right_channel = true;
        auto xr = dc->convo_sum(sigr, irr);

        dc->convolved_l = xl;
        dc->convolved_r = xr;
        dc->in_progress = false;

        dc->yield();
    }*/

    std::shared_ptr<std::vector<double>> convo_sum(
            std::vector<double> *signal,
            std::vector<double> *kernel)
    {
        auto signal_size = signal->size();
        auto kernel_size = kernel->size();
        auto cnt_operations = signal_size * kernel_size;

        int jmn = 0;
        int jmx = 0;

        double p = 0;

        auto ret = std::make_shared<std::vector<double>>(signal_size);

        size_t ops = 0;
        for(auto i = 0; i < signal_size; ++i)
        {
            jmn = (i >= signal_size - 1) ? i - (signal_size - 1) : 0;
            jmx = (i < kernel_size - 1) ? i : kernel_size - 1;
            for (auto j(jmn); j <= jmx; ++j)
            {
                ++ops;
                p += (kernel->at(j) * signal->at(i - j));
            }
            ret->emplace_back(p);
            p = 0;
        }

        return ret;
    }

public:
    std::shared_ptr<wave_source> ws = nullptr;
    std::shared_ptr<wave_source> wsi = nullptr;
    std::shared_ptr<std::vector<double>> convolved_l = nullptr;
    std::shared_ptr<std::vector<double>> convolved_r = nullptr;
    bool in_progress = false;
    bool right_channel = false;

    explicit dsp_convolver(
            std::function<std::shared_ptr<wave_source>(void)> obtain_input,
            std::function<void(std::shared_ptr<internal_signal>)> yield_signal,
            std::function<void(void)> close
    )
    {
        this->obtain_input = std::move(obtain_input);
        this->yield_signal = std::move(yield_signal);
        this->close = std::move(close);
    }

    virtual ~dsp_convolver()
    {
        stop = true;
        if (visualizer != nullptr && visualizer->joinable())
        {
            visualizer->join();
        }
        if (worker != nullptr && worker->joinable())
        {
            worker->join();
        }
    }

    static void* thread_proxy(dsp_convolver* ci)
    {
        while(!ci->stop)
        {
            ci->thread_proc();
        }
        return nullptr;
    }

    void thread_proc()
    {
        m->sample(t);
        ++t %= wav.l->size();
        std::this_thread::sleep_for(std::chrono::nanoseconds(  (int)(1000000.f/(float)wav.wav_file.sampleRate)  ));
    }

    void load(const char *path)
    {
        wav.load_wav(path, false);
        ws = std::make_shared<wave_source>(&wav, wav.wav_file.sampleRate);
        m = std::make_unique<meter>(wav.wav_file.sampleRate);
        m->roll = 6;
        ws->connect_to(m, 1, 1);

        visualizer = std::make_unique<std::thread>(thread_proxy, this);
    }

    void run(std::shared_ptr<wave_source> wsinput)
    {
        if (this->worker != nullptr && this->worker->joinable())
        {
            this->worker->join();
        }
        this->wsi = std::move(wsinput);
        worker = std::make_unique<std::thread>(worker_run, this);
    }

    void yield()
    {
        auto x = std::make_shared<internal_signal>(
                wsi->w->wav_file.sampleRate,
                wsi->w->wav_file.bitsPerSample,
                "Convolver",
                convolved_l,
                convolved_r
        );

        x->normalize();

        yield_signal(x);
    }

    int w = 36;
    int h = 19;

    bool focused = false;

    size_t t = 0;
    void on_frame()
    {
        ImGui::SetNextWindowPos(ImVec2((float)65, (float)15), ImGuiCond_Once);

        windowth(w, h, "DSP:_Convolver", [this]()->void {render_content();});
    }

    void render_content()
    {
        ImGuiWindowFlags window_flags =
                ImGuiWindowFlags_NoScrollbar |
                ImGuiWindowFlags_NoScrollWithMouse |
                ImGuiWindowFlags_NoSavedSettings |
                ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoResize;

        ImGui::Text("");
        ImGui::TextWrapped("Convolve the audio signal with an impulse response");
        ImGui::Text("");

        ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(255,255,255));
        ImGui::Text(R"( ______________________________)");
        ImGui::Text(R"(|,--------------------------.  |)");

        // Child 1
        ImGui::Text(R"(||)");
        ImGui::Text(R"(||)");
        ImGui::Text(R"(||)");
        ImGui::Text(R"(||)");
        ImGui::Text(R"(||)");
        ImGui::Text(R"(||)");
        ImGui::Text(R"(||)");

        ImGui::SetCursorPos(ImVec2(1,6));
/*
        // Child 2

*/
        ImGui::SetCursorPos(ImVec2(27,6));
        // Child 3
        ImGui::Text(R"( |=|)");
        ImGui::SetCursorPos(ImVec2(27,6));
        ImGui::Text(R"( | |)");
        ImGui::SetCursorPos(ImVec2(27,7));
        ImGui::Text(R"(|| |)");
        ImGui::SetCursorPos(ImVec2(27,8));
        ImGui::Text(R"(|| |)");
        ImGui::SetCursorPos(ImVec2(27,9));
        ImGui::Text(R"(|| |)");
        ImGui::SetCursorPos(ImVec2(27,10));
        ImGui::Text(R"(|| |)");
        ImGui::SetCursorPos(ImVec2(27,11));
        ImGui::Text(R"(o| |)");

        ImGui::SetCursorPos(ImVec2(-1,12));
        ImGui::Text(R"(|`---------------------------' |)");
        ImGui::Text(R"( ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~)");

        ImGui::SetCursorPos(ImVec2(3,6));
        ImGui::BeginChild("dd", ImVec2(25,6), false, window_flags);
        //ImGui::PushStyleColor(ImGuiCol_PlotLines, (ImVec4)ImColor::HSV(120.f/360.f, 1.0f, 0.7f));
        ImGui::PushStyleColor(ImGuiCol_PlotLines, (ImVec4)ImColor(71, 167, 169));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(0, 0, 0.f/360.f));
        ImGui::PlotLines("", m->wind_avg, m->bin_range, 0, "IR SPECTRUM", 0.f, m->ceiling, ImVec2(25, 7));
        ImGui::PopStyleColor(2);
        ImGui::EndChild();

        ImGui::SetCursorPos(ImVec2(-1,14));

        ImGui::PopStyleColor();

        if (in_progress)
        {
            if (right_channel)
            {
                ImGui::Text("Convolving right channel...");
            }
            else
            {
                ImGui::Text("Convolving left channel...");
            }
        }
        else
        {
            ImGui::Text("");
        }

        //ImGui::Text("");

        static bool bypass;
        ImGui::Text("                      "); ImGui::SameLine();
        //ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.6f, 0.6f));
        //ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.7f));
        //ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f, 0.8f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(255,255,255));
        if(ImGui::Button(" CONVOLVE "))
        {
            run(obtain_input());
        }
        ImGui::PopStyleColor();
        //ImGui::PopStyleColor(3);
    }
};



