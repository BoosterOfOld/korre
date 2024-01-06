#pragma once

#include <math.h>
#include <tuple>

#include "sampler.hpp"

class sine : public sampler
{
private:
    // Input
    double frequency;
    double amplitude;

public:
    sine(double sample_rate, int16_t max_quant, double frequency, double amplitude) : sampler(sample_rate, max_quant)
    {
        this->frequency = frequency;
        this->amplitude = amplitude;
    }

    virtual std::tuple<double, double> sample(size_t t)
    {
        // cos(fh(k)*(2*pi*1 + .003*sin(pi*1)
        //double y = cos(2.f * M_PI * this->frequency * t) * this->amplitude * (double)this->max_quant;

        double y = cos(this->frequency * (2.f * M_PI * t + .003*sin(M_PI * t))) * this->amplitude * (double)this->max_quant;

        return {y, y};
    }

    sine *harmonic(int16_t order, double amplitude)
    {
        return new sine(this->sample_rate, this->max_quant, this->frequency * (double)order, amplitude);
    }

    virtual void pulse()
    {
        this->propagate();
    }
};
