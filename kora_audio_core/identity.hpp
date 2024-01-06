#pragma once

#include <math.h>
#include <tuple>
#include <vector>
#include <iostream>

#include "sampler.hpp"

class identity : public sampler
{
private:
    // Input
    size_t m_sample_size;

    std::tuple<std::vector<float>, std::vector<float>> *kernel;

    size_t output_size;

public:
    identity(double sample_rate, int16_t max_quant) : sampler(sample_rate, max_quant)
    {
        ;
    }

    virtual void post_connect()
    {
        ;
    }

    virtual size_t sample_size()
    {
        return s_in1()->sample_size();
    }

    virtual std::tuple<double, double> sample(double t)
    {
        return s_in1()->sample(t);
    }

    virtual void pulse()
    {
        this->propagate();
    }
};

