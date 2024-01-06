#pragma once

#include <math.h>
#include <vector>
#include <tuple>
#include <cstdint> // Necessary for uint32_t

#include "sampler.hpp"

class nmixer : public sampler
{
private:
    // Input
    std::vector<sampler *> samplers;

public:
    nmixer(double sample_rate, int16_t max_quant, std::vector<sampler *> samplers) : sampler(sample_rate, max_quant)
    {
        this->samplers = samplers;
    }

    virtual std::tuple<double, double> sample(double t)
    {
        double sum1 = 0;
        double sum2 = 0;

        for(u_int16_t i = 0; i < this->samplers.size(); ++i)
        {
            sum1 += std::get<0>(this->samplers[i]->sample(t));
            sum2 += std::get<1>(this->samplers[i]->sample(t));
        }

        return {sum1 / this->samplers.size(), sum2 / this->samplers.size()};
    }

    virtual void pulse()
    {
        this->propagate();
    }
};
