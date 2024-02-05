#pragma once

#include "wave_source.h"

wave_source::wave_source(wave *w, uint32_t sample_rate) : sampler(sample_rate, 0)
{
    name = "Wave Source";
    this->w = w;
}

wave_source::~wave_source()
{
    delete w;
}

void wave_source::add_signal(std::shared_ptr<internal_signal> sig)
{
    iss.emplace_back(sig);
}

void wave_source::select_signal(int sn)
{
    selected_signal = sn;
}

std::tuple<double, double> wave_source::sample(size_t t)
{
    if (selected_signal >= 0)
    {
        return iss[selected_signal]->sample(t);
    }

    if (w->wav_file.numChannels == 2)
    {
        return { (*w->l)[t], (*w->r)[t] };
    }
    else
    {
        return { (*w->l)[t], (*w->l)[t] };
    }
}

size_t wave_source::sample_size()
{
    int num_samples = ((float)w->wav_file.subchunk2Size / (float)w->wav_file.numChannels) / ((float)w->wav_file.bitsPerSample / 8.f);
    return num_samples;
}

void wave_source::pulse()
{
    this->propagate();
}