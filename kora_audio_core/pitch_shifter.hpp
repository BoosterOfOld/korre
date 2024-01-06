#pragma once

#include <math.h>
#include <tuple>
#include <vector>
#include <iostream>
#include <mutex>
#include <thread>

#include "sampler.hpp"
#include "StftPitchShift/StftPitchShift.h"

class pitch_shifter : public sampler
{
private:
    std::unique_ptr<stftpitchshift::StftPitchShift> shift;

    std::mutex mx;
    std::unique_ptr<std::thread> switcher;

    std::unique_ptr<std::vector<float>> input_buffer_l;
    std::unique_ptr<std::vector<float>> input_buffer_r;

    std::unique_ptr<std::vector<float>> output_buffer_l;
    std::unique_ptr<std::vector<float>> output_buffer_r;

    float progressl;
    float progressr;

public:
    std::atomic<bool> bypassed;
    float switch_param = 1;

    pitch_shifter(double sample_rate, int16_t max_quant) : sampler(sample_rate, max_quant)
    {
        shift = std::make_unique<stftpitchshift::StftPitchShift>(1024, 256, sample_rate);
    }

    ~pitch_shifter() override
    {
        if (switcher != nullptr && switcher->joinable())
        {
            switcher->join();
        }
    }

    static void* switcher_proxy(pitch_shifter* ps)
    {
        ps->switch_thr(ps->switch_param);
        return nullptr;
    }

    void switch_thr(float shift)
    {
        update();
        is_propagating = false;
        bypassed = false;
        propagate();
    }

    void exchange_input_buffers(std::vector<float> &&newl, std::vector<float> &&newr)
    {
        input_buffer_l = std::make_unique<std::vector<float>>(newl);
        input_buffer_r = std::make_unique<std::vector<float>>(newr);
    }

    void exchange_output_buffers(std::vector<float> &&newl, std::vector<float> &&newr)
    {
        output_buffer_l = std::make_unique<std::vector<float>>(newl);
        output_buffer_r = std::make_unique<std::vector<float>>(newr);
    }

    virtual void post_connect() override
    {
        mx.lock();
        exchange_input_buffers(std::vector<float>(s_in1()->sample_size()), std::vector<float>(s_in1()->sample_size()));
        exchange_output_buffers(std::vector<float>(s_in1()->sample_size()), std::vector<float>(s_in1()->sample_size()));
        mx.unlock();

        update();
    }

    void update()
    {
        is_propagating = true;

        mx.lock();
        exchange_input_buffers(std::vector<float>(s_in1()->sample_size()), std::vector<float>(s_in1()->sample_size()));
        mx.unlock();

        std::tuple<double, double> s;

        for (auto i(0); i < s_in1()->sample_size(); ++i)
        {
            s = s_in1()->sample(i);
            (*input_buffer_l)[i] = std::get<0>(s);
            (*input_buffer_r)[i] = std::get<1>(s);
        }

        std::vector<float> shifted_l(s_in1()->sample_size());
        std::vector<float> shifted_r(s_in1()->sample_size());


        shift->shiftpitch((*input_buffer_l), shifted_l, switch_param, 0, 1, &progressl);
        shift->shiftpitch((*input_buffer_r), shifted_r, switch_param, 0, 1, &progressr);

        mx.lock();
        exchange_output_buffers(std::move(shifted_l), std::move(shifted_r));
        mx.unlock();

        progressl = 0.f;
        progressr = 0.f;
        is_propagating = false;
    }

    virtual size_t sample_size() override
    {
        return this->s_in1()->sample_size();
    }

    void bypass()
    {
        if (switcher != nullptr && is_propagating)
        {
            switcher->join();
        }

        std::vector<float> bypassed_l(s_in1()->sample_size());
        std::vector<float> bypassed_r(s_in1()->sample_size());

        bypassed_l = *input_buffer_l;
        bypassed_r = *input_buffer_r;

        mx.lock();
        exchange_output_buffers(std::move(bypassed_l), std::move(bypassed_r));
        mx.unlock();

        bypassed = true;
        propagate();
    }

    void enable()
    {
        bypassed = false;
        propagate();
    }

    void request_pitch_shift(float shift)
    {
        switch_param = shift;
        if (is_propagating)
        {
            return;
        }
        is_propagating = true;

        if (switcher != nullptr && is_propagating)
        {
            switcher->join();
        }

        switcher = std::make_unique<std::thread>(switcher_proxy, this);
    }

    virtual std::tuple<double, double> sample(size_t t) override
    {
        size_t c = t;

        std::tuple<double, double> ret;

        double l_sample;
        double r_sample;

        mx.lock();
        l_sample = (*output_buffer_l)[c];
        r_sample = (*output_buffer_r)[c];
        mx.unlock();

        ret = {l_sample, r_sample};

        return ret;
    }

    virtual float progress() override
    {
        return (progressl + progressr) / 2.f;
    }

    virtual void pulse() override
    {
        if (is_propagating)
        {
            return;
        }

        if (switcher != nullptr && switcher->joinable())
        {
            switcher->join();
        }

        is_propagating = true;

        switcher = std::make_unique<std::thread>(switcher_proxy, this);
    }
};
