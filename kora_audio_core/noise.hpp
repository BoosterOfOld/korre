#pragma once

#include <math.h>
#include <random>
#include <stdio.h>
#include <iostream>
#include <tuple>

#include "sampler.hpp"

class noise : public sampler
{
private:
    // Input
    int16_t cutoff_freq;
    double amplitude;

    // Derived parameters
    double alpha;
    double period;

    // Noise distribution
    std::random_device *rd;
    std::mt19937 *gen;
    std::uniform_int_distribution<> *quant_distr;

    // Previous values
    double prev_x;
    double prev_y;

    void update_derived_parameters()
    {
        //float cutoff = 440; // Hz
        this->period = (1.f / this->sample_rate);
        this->alpha = (2.f * M_PI * period * this->cutoff_freq) / (2.f * M_PI * period * this->cutoff_freq + 1.f);
    }

public:
    noise(double sample_rate, int16_t max_quant, double cutoff_freq, double amplitude) : sampler(sample_rate, max_quant)
    {
        this->cutoff_freq = cutoff_freq;
        this->amplitude = amplitude;

        this->update_derived_parameters();

        this->rd = new std::random_device();
        this->gen = new std::mt19937((*this->rd)());
        this->quant_distr = new std::uniform_int_distribution<>(0, this->max_quant);
    }

    virtual ~noise()
    {
        delete this->quant_distr;
        delete this->gen;
        delete this->rd;
    }

    virtual std::tuple<double, double> sample(double t)
    {
        double x = (*this->quant_distr)(*this->gen) * this->amplitude;
        //double y = this->alpha * (double)x + (1.f - this->alpha) * (double)this->prev_y;
        double y = this->prev_y + this->alpha * (x - this->prev_y);

        prev_y = y;
        prev_x = x;

        return {y, y};
    }

    virtual void pulse()
    {
        this->propagate();
    }
};
