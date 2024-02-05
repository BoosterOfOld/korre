#pragma once

#include <deque>
#include <portaudio.h>

#include "sink.h"

#define M_PI_M2 ( M_PI + M_PI )

#define DEFAULT_CHANNELS        2
#define DEFAULT_VOLUME          0.7

volatile extern bool is_on;

struct pa_data
{
    double accumulator;
    std::shared_ptr<sampler> sampler;
    size_t t = 0;
};

extern uint32_t PA_SAMPLE_LENGTH;
extern uint32_t PA_SAMPLE_RATE;
extern uint32_t PA_BIT_DEPTH;
extern uint32_t PA_NUM_CHANNELS;

extern int PA_SELECTED_DEVICE;
extern bool PA_USE_DEFAULT_DEVICE;

extern int PA_T;
extern std::function<void(void)> PA_SIG_TRACK_END;
extern std::deque<std::function<void(void)>> comms_q;

class portaudio_sink : public sink
{
private:
    std::unique_ptr<std::thread> worker;

    static void *worker_run(std::shared_ptr<sampler> arg);
    static void terminate();

public:
    portaudio_sink();
    virtual ~portaudio_sink();

    virtual void pulse() override;
    virtual void open() override;
    virtual void plug() override;
    virtual bool is_open() override;
};
