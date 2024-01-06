#pragma once

#include <math.h>
#include <tuple>
#include <vector>
#include <iostream>

#include "sampler.hpp"

class convolver : public sampler
{
private:
    // Input
    size_t m_sample_size;

    std::tuple<std::vector<float>, std::vector<float>> *kernel;

    float *input_buffer_l;
    float *input_buffer_r;

    float *output_buffer_l;
    float *output_buffer_r;

    size_t output_size;

public:
    convolver(double sample_rate, int16_t max_quant, std::tuple<std::vector<float>, std::vector<float>> *kernel) : sampler(sample_rate, max_quant)
    {
        this->kernel = kernel;
    }

    virtual void post_connect() override
    {
        this->m_sample_size = s_in1()->sample_size();
        this->output_size = m_sample_size + kernel_size() - 1;
        this->input_buffer_l = new float[output_size];
        this->input_buffer_r = new float[output_size];
        this->output_buffer_l = new float[m_sample_size];
        this->output_buffer_r = new float[m_sample_size];
    }

    float *kernel_l()
    {
        return std::get<0>(*kernel).data();
    }

    float *kernel_r()
    {
        return std::get<1>(*kernel).data();
    }

    size_t kernel_size()
    {
        return std::get<0>(*kernel).size();
    }

    virtual size_t sample_size() override
    {
        return m_sample_size;
    }

    virtual std::tuple<double, double> sample(size_t t) override
    {
        auto s = s_in1()->sample(t);

        float l_in = std::get<0>(s);
        float r_in = std::get<1>(s);

        input_buffer_l[t] = l_in;
        input_buffer_r[t] = r_in;

        //int const n = kernel_size() + m_sample_size - 1;

        float l = 0;
        float r = 0;

        auto i = t;

        int const jmn = (i >= m_sample_size - 1) ? i - (m_sample_size - 1) : 0;
        int const jmx = (i <  kernel_size() - 1)   ? i                       : kernel_size() - 1;
        for(auto j(jmn); j <= jmx; ++j)
        {
            l += (kernel_l()[j] * input_buffer_l[i - j]);
            r += (kernel_r()[j] * input_buffer_r[i - j]);
        }

        output_buffer_l[t] = l;
        output_buffer_r[t] = r;
/*
        ++counter;
        counter %= m_sample_size;*/

        std::tuple<double, double> ret =
        {
            l,
            r,
        };

        return ret;
    }

    virtual void pulse() override
    {
        this->propagate();
    }
};
