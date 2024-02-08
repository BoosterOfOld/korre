#pragma once

#include <thread>

#include "meter.h"
#include "sources/wave_source.h"

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

    static void *worker_run(dsp_convolver *dc);
    std::shared_ptr<std::vector<double>> convo_sum(
            std::vector<double> *signal,
            std::vector<double> *kernel);

    int w = 36;
    int h = 19;
    bool focused = false;
    size_t t = 0;

    void yield();
    void render_content();
    void run(std::shared_ptr<wave_source> wsinput);
    static void* thread_proxy(dsp_convolver* ci);
    void thread_proc();

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
    );
    virtual ~dsp_convolver();

    void load(const char *path);
    void on_frame();
};



