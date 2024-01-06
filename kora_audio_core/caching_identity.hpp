#pragma once

#include <math.h>
#include <tuple>
#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <mutex>

#include "sampler.hpp"

class caching_identity : public sampler
{
private:
    std::mutex mx;
    std::unique_ptr<std::thread> switcher;

    std::unique_ptr<std::vector<float>> output_buffer_l;
    std::unique_ptr<std::vector<float>> output_buffer_r;

    float progressv = 0;

    float maxl = 0;
    float maxr = 0;

public:
    caching_identity(double sample_rate, int16_t max_quant) : sampler(sample_rate, max_quant)
    {
        ;
    }

    static void* identity_proxy(caching_identity* ci)
    {
        ci->perform_identity();
        return nullptr;
    }

    void perform_identity()
    {
        update();
        propagate();
    }

    ~caching_identity() override
    {
        if (switcher != nullptr && switcher->joinable())
        {
            switcher->join();
        }
    }

    void exchange_buffers(std::vector<float> &&newl, std::vector<float> &&newr)
    {
        mx.lock();
        output_buffer_l = std::make_unique<std::vector<float>>(newl);
        output_buffer_r = std::make_unique<std::vector<float>>(newr);
        mx.unlock();
    }

    virtual void post_connect() override
    {
        exchange_buffers(std::vector<float>(s_in1()->sample_size()), std::vector<float>(s_in1()->sample_size()));

        update();
    }

    void update()
    {
        is_propagating = true;

        exchange_buffers(std::vector<float>(s_in1()->sample_size()), std::vector<float>(s_in1()->sample_size()));

        std::tuple<double, double> s;

        float l = 0;
        float r = 0;

        for (auto i(0); i < s_in1()->sample_size(); ++i)
        {
            progressv = (float)i / (float)(s_in1()->sample_size());

            s = s_in1()->sample(i);
            mx.lock();
            l = std::get<0>(s);
            r = std::get<1>(s);
            (*output_buffer_l)[i] = std::get<0>(s);
            (*output_buffer_r)[i] = std::get<1>(s);

            if (l > maxl) maxl = l;
            if (r > maxr) maxr = r;

            mx.unlock();
        }

        progressv = 0.f;
        is_propagating = false;
    }

    virtual size_t sample_size() override
    {
        return this->s_in1()->sample_size();
    }

    virtual std::tuple<double, double> sample(size_t t) override
    {
        std::tuple<double, double> ret;

        mx.lock();
        double l_sample = ((*output_buffer_l)[t] / maxl) * (float)this->max_quant;
        double r_sample = ((*output_buffer_r)[t] / maxr) * (float)this->max_quant;
        mx.unlock();

        ret = {l_sample, r_sample};

        return ret;
    }

    virtual float progress() override
    {
        return progressv;
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

        switcher = std::make_unique<std::thread>(identity_proxy, this);
    }
};
