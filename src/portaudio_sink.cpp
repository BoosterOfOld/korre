#include "portaudio_sink.h"

#include <iostream>

#include <cmath>
#include <thread>
#include <memory>

uint32_t PA_SAMPLE_LENGTH;
uint32_t PA_SAMPLE_RATE;
uint32_t PA_BIT_DEPTH;
uint32_t PA_NUM_CHANNELS;

int PA_SELECTED_DEVICE = 1;
bool PA_USE_DEFAULT_DEVICE = true;

int PA_T;
std::function<void(void)> PA_SIG_TRACK_END = nullptr;
std::deque<std::function<void(void)>> comms_q;

volatile bool is_on = false;

static int paCallback( const void *inputBuffer, void *outputBuffer,
                       unsigned long framesPerBuffer,
                       const PaStreamCallbackTimeInfo* timeInfo,
                       PaStreamCallbackFlags statusFlags,
                       void *userData )
{
    /* Cast data passed through stream to our structure. */
    pa_data *datax = (pa_data*)userData;

    int16_t *out = (int16_t *)outputBuffer;
    unsigned char *out_byte = (unsigned char *)outputBuffer;
    unsigned int i;
    (void) inputBuffer; /* Prevent unused variable warning. */

    std::tuple<double, double> pair;

    for(i=0; i<framesPerBuffer; ++i)
    {
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

        if ((PA_SIG_TRACK_END != nullptr) && (PA_T != 0) && (PA_T % PA_SAMPLE_LENGTH == 0))
        {
            PA_T = 0;
            comms_q.push_front(PA_SIG_TRACK_END);
        }
        else
        {
            PA_T %= PA_SAMPLE_LENGTH;
        }
    }
    return 0;
}

void *portaudio_sink::worker_run(std::shared_ptr<sampler> arg)
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
        if( err != paNoError )
        {
            terminate();
        }
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

void portaudio_sink::terminate()
{
    is_on = false;
    Pa_Terminate();
}

portaudio_sink::portaudio_sink()
{
    name = "PA Sink";
    PA_T = 0;
}

void portaudio_sink::pulse()
{
    ;
}

portaudio_sink::~portaudio_sink()
{
    if (worker != nullptr)
    {
        terminate();
        worker->join();
    }
}

void portaudio_sink::open()
{
    if (worker != nullptr)
    {
        return;
    }

    worker = std::make_unique<std::thread>(worker_run, s_in1());
}

void portaudio_sink::plug()
{
    terminate();
    if (worker!=nullptr)
    {
        worker->join();
        worker = nullptr;
    }
}

bool portaudio_sink::is_open()
{
    return is_on;
}
