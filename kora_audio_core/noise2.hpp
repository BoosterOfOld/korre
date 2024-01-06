#pragma once

#include <math.h>
#include <random>
#include <stdio.h>
#include <iostream>
#include <tuple>

#include "sampler.hpp"

class noise2 : public sampler
{
private:
    // Input
    int16_t cutoff_freq;
    int16_t cuton_freq;
    double amplitude;

    // Derived parameters
    double alpha;
    double beta;
    double period;

    // Noise distribution
    std::random_device *rd;
    std::mt19937 *gen;
    std::uniform_int_distribution<> *quant_distr;

    // Previous values
    double prev_x;
    double prev_y;
    double prev_y_x;

    bool lo_pass = false;

    void update_derived_parameters()
    {
        //float cutoff = 440; // Hz
        this->period = (1.f / this->sample_rate);
        this->alpha = (2.f * M_PI * period * this->cutoff_freq) / (2.f * M_PI * period * this->cutoff_freq + 1.f);
        this->beta = 1.f / (2.f * M_PI * period * this->cuton_freq + 1.f);
    }

public:
    noise2(double sample_rate, int16_t max_quant, double cuton_freq, double cutoff_freq, double amplitude) : sampler(sample_rate, max_quant)
    {
        this->cutoff_freq = cutoff_freq;
        this->cuton_freq = cuton_freq;
        this->amplitude = amplitude;

        this->update_derived_parameters();

        this->rd = new std::random_device();
        this->gen = new std::mt19937((*this->rd)());
        this->quant_distr = new std::uniform_int_distribution<>(0, this->max_quant);
    }

    virtual ~noise2()
    {
        delete this->quant_distr;
        delete this->gen;
        delete this->rd;
    }

    virtual std::tuple<double, double> sample(double t)
    {
        double x = (*this->quant_distr)(*this->gen) * this->amplitude;
        double y_x = this->alpha * x + (1.f - this->alpha) * this->prev_y;

        double y = this->beta * prev_y + this->beta * (y_x - prev_y_x);

        //double y = this->beta * prev_y + this->beta * (y_x - prev_y);

        prev_y_x = y_x;
        prev_y = y;
        prev_x = x;

        return {y, y};
    }

    virtual void pulse()
    {
        this->propagate();
    }
};
