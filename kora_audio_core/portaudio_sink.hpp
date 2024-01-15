#pragma once

#include <iostream>
#include <stdexcept>
#include <cstdint> // Necessary for uint32_t
#include <fstream>

#include <math.h>
#include <random>
#include <stdio.h>
#include <iostream>
#include <tuple>

#include <portaudio.h>

#include <thread>
#include <memory>

#include "sink.hpp"
#include "sampler.hpp"

#define M_PI_M2 ( M_PI + M_PI )

#define DEFAULT_CHANNELS        2
#define DEFAULT_VOLUME          0.7

volatile static bool is_on = false;

size_t graph_sample_index = 0;

struct pa_data
{
    double accumulator;
    std::shared_ptr<sampler> sampler;
    size_t t = 0;
};

static uint32_t PA_SAMPLE_LENGTH;
static uint32_t PA_SAMPLE_RATE;
static uint32_t PA_BIT_DEPTH;
static uint32_t PA_NUM_CHANNELS;

static int PA_SELECTED_DEVICE = 1;
static bool PA_USE_DEFAULT_DEVICE = true;

static int PA_T;

static int paCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData )
{
    /* Cast data passed through stream to our structure. */
    pa_data *datax = (pa_data*)userData;

    // To 24 bits !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    int16_t *out = (int16_t *)outputBuffer;
    unsigned char *out_byte = (unsigned char *)outputBuffer;
    unsigned int i;
    (void) inputBuffer; /* Prevent unused variable warning. */

    std::tuple<double, double> pair;

    for(i=0; i<framesPerBuffer; ++i)
    {
        //pair = datax->sampler->sample(datax->t);
        pair = datax->sampler->sample(PA_T);

        double left_sample = std::get<0>(pair);
        double right_sample = std::get<1>(pair);

        if (PA_BIT_DEPTH == 16)
        {
            *out++ = (int16_t)left_sample;  /* left */
            *out++ = (int16_t)right_sample;  /* right */
        }
        else if (PA_BIT_DEPTH == 24)
        {
            auto l32 = (int32_t)left_sample;
            auto r32 = (int32_t)right_sample;

            // The 24 bit data will never be signed in a 32 bit int
            unsigned char lb1 = (l32 >> 16);
            unsigned char lb2 = (l32 >> 8);
            unsigned char lb3 = l32;

            unsigned char rb1 = (r32 >> 16);
            unsigned char rb2 = (r32 >> 8);
            unsigned char rb3 = r32;

            *out_byte++ = lb3;
            *out_byte++ = lb2;
            *out_byte++ = lb1;

            *out_byte++ = rb3;
            *out_byte++ = rb2;
            *out_byte++ = rb1;
        }

        //++datax->t;
        //datax->t %= PA_SAMPLE_LENGTH;
        ++PA_T;
        PA_T %= PA_SAMPLE_LENGTH;
    }
    return 0;
}


class portaudio_sink : public sink
{
private:
    std::unique_ptr<std::thread> worker;

    static void *worker_run(std::shared_ptr<sampler> arg)
    {
        is_on = true;

        PaError err;
        static PaStream *stream;
        struct pa_data datax;

        datax.sampler = arg;
        datax.accumulator = 0;

        /* Initialize library before making any other calls. */
        err = Pa_Initialize();
        if( err != paNoError ) terminate();

        if (PA_USE_DEFAULT_DEVICE)
        {
            // Open an audio I/O stream.
            err = Pa_OpenDefaultStream(&stream,
                                       0,
                                       2,
                                       PA_BIT_DEPTH == 8 ? paInt8 : PA_BIT_DEPTH == 16 ? paInt16 : PA_BIT_DEPTH == 24 ? paInt24 : PA_BIT_DEPTH == 32 ? paInt32 : paInt16,
                                       PA_SAMPLE_RATE,
                                       256,        // frames per buffer
                                        paCallback,
                                       &datax );
            if( err != paNoError ) terminate();
        }
        else
        {
            PaStreamParameters outputParameters;
            bzero(&outputParameters, sizeof(outputParameters)); //not necessary if you are filling in all the fields
            outputParameters.channelCount = 2;
            outputParameters.device = PA_SELECTED_DEVICE;
            outputParameters.hostApiSpecificStreamInfo = NULL;
            outputParameters.sampleFormat =
                    PA_BIT_DEPTH == 8 ? paInt8 : PA_BIT_DEPTH == 16 ? paInt16 : PA_BIT_DEPTH == 24 ? paInt24 :
                                                                                PA_BIT_DEPTH == 32 ? paInt32 : paInt16;
            outputParameters.suggestedLatency = Pa_GetDeviceInfo(PA_SELECTED_DEVICE)->defaultLowOutputLatency;
            outputParameters.hostApiSpecificStreamInfo = NULL; //See you specific host's API docs for info on using this field

            err = Pa_OpenStream(
                    &stream,
                    NULL,
                    &outputParameters,
                    PA_SAMPLE_RATE,
                    256,
                    paNoFlag, //flags that can be used to define dither, clip settings and more
                    paCallback, //your callback function
                    &datax);
            if (err != paNoError) terminate();
        }

        err = Pa_StartStream( stream );
        if( err != paNoError ) terminate();

        while(is_on)
        {
            Pa_Sleep(100);
        }

        return nullptr;
    }

    static void terminate()
    {
        is_on = false;
        Pa_Terminate();
    }

public:
    portaudio_sink()
    {
        name = "PA Sink";
        PA_T = 0;
    }

    virtual void pulse() override
    {
        ;
    }

    virtual ~portaudio_sink()
    {
        if (worker != nullptr)
        {
            terminate();
            worker->join();
        }
    }

    virtual void open() override
    {
        if (worker != nullptr)
        {
            return;
        }

        worker = std::make_unique<std::thread>(worker_run, s_in1());
    }

    virtual void plug() override
    {
        terminate();
        if (worker!=nullptr)
        {
            worker->join();
            worker = nullptr;
        }
    }

    virtual bool is_open() override
    {
        return is_on;
    }
};
