#pragma once

#include "sampler.hpp"
#include "wave.hpp"

class wave_source : public sampler
{
public:
    wave *w;
    explicit wave_source(wave *w, uint32_t sample_rate, int16_t max_quant) : sampler(sample_rate, max_quant)
    {
        name = "Wave Source";
        this->w = w;
    }

    ~wave_source() override
    {
        delete w;
    }

    virtual std::tuple<double, double> sample(size_t t) override
    {
        if (w->wav_file.numChannels == 2)
        {
            return { (*w->l)[t], (*w->r)[t] };
        }
        else
        {
            return { (*w->l)[t], (*w->l)[t] };
        }
    }

    virtual size_t sample_size() override
    {
        int num_samples = ((float)w->wav_file.subchunk2Size / (float)w->wav_file.numChannels) / ((float)w->wav_file.bitsPerSample / 8.f);

        return num_samples;
    }

    virtual void pulse() override
    {
        this->propagate();
    }
};