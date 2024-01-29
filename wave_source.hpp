#pragma once

#include <vector>

#include "sampler.hpp"
#include "wave.hpp"
#include "internal_signal.hpp"

class wave_source : public sampler
{
public:
    wave *w;

    std::vector<std::shared_ptr<internal_signal>> iss;
    int selected_signal = -1;

    explicit wave_source(wave *w, uint32_t sample_rate) : sampler(sample_rate, 0)
    {
        name = "Wave Source";
        this->w = w;
    }

    ~wave_source() override
    {
        delete w;
    }

    void add_signal(std::shared_ptr<internal_signal> sig)
    {
        iss.emplace_back(sig);
    }

    void select_signal(int sn)
    {
        selected_signal = sn;
    }

    virtual std::tuple<double, double> sample(size_t t) override
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