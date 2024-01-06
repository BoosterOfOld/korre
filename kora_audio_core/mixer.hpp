#pragma once

#include <math.h>
#include <tuple>

#include "sampler.hpp"

class mixer : public sampler
{
private:

public:
    mixer(double sample_rate, int16_t max_quant) : sampler(sample_rate, max_quant)
    {
        ;
    }

    virtual std::tuple<double, double> sample(size_t t) override
    {
        std::tuple<double, double> a;

        if (s_in1() == s_in2())
        {
            a = s_in1()->sample(t);
        }
        else
        {
            a = s_in1()->sample(t);
        }

        auto b = s_in2()->sample(t);

        auto newa = (std::get<0>(a) + std::get<0>(b)) / 2;
        auto newb = (std::get<1>(a) + std::get<1>(b)) / 2;

        return
        {
            newa,
            newb,
        };
    }

    virtual size_t sample_size() override
    {
        return s_in1()->sample_size();
    }

    virtual void pulse() override
    {
        this->propagate();
    }
};
