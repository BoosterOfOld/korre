#pragma once

#include <math.h>
#include <random>
#include <stdio.h>
#include <iostream>
#include <tuple>

#include "sampler.hpp"

class noise3 : public sampler
{
private:
    // Input
    int16_t target_freq;
    int16_t bandwidth;
    double amplitude;

    // Derived parameters
    double f;
    double bw;
    double R;
    double Rsq;
    double cosf2;
    double K;
    double a0;
    double a1;
    double a2;
    double b1;
    double b2;

    // Noise distribution
    std::random_device *rd;
    std::mt19937 *gen;
    std::uniform_int_distribution<> *quant_distr;

    // Previous values
    double x_1 = 0;
    double x_2 = 0;
    double y_1 = 0;
    double y_2 = 0;

    void update_derived_parameters()
    {
        //float cutoff = 440; // Hz
        this->f = this->target_freq / this->sample_rate;
        this->bw = this->bandwidth / this->sample_rate;
        this->R = 1.f - (3.f * this->bw);
        this->Rsq = R * R;
        this->cosf2 = 2.f * cos(2.f * M_PI * this->f);
        this->K = (1.f - this->R * this->cosf2 + this->Rsq) / (2.f - this->cosf2);
        this->a0 = 1.f - this->K;
        this->a1 = 2.f * (this->K - this->R) * this->cosf2;
        this->a2 = this->Rsq - this->K;
        this->b1 = 2.f * this->R * this->cosf2;
        this->b2 = -this->Rsq;
    }

public:
    noise3(double sample_rate, int16_t max_quant, double target_freq, double bandwidth, double amplitude) : sampler(sample_rate, max_quant)
    {
        this->target_freq = target_freq;
        this->bandwidth = bandwidth;
        this->amplitude = amplitude;

        this->update_derived_parameters();

        this->rd = new std::random_device();
        this->gen = new std::mt19937((*this->rd)());
        this->quant_distr = new std::uniform_int_distribution<>(0, this->max_quant);
    }

    virtual ~noise3()
    {
        delete this->quant_distr;
        delete this->gen;
        delete this->rd;
    }

    virtual std::tuple<double, double> sample(double t)
    {
        double x = sin(2.f * M_PI * 440 *t);//(double)(*this->quant_distr)(*this->gen) * this->amplitude;

        double y = (this->a0 * x) +
                   (this->a1 * this->x_1) +
                   (this->a2 * this->x_2) +
                   (this->b1 * this->y_1) +
                   (this->b2 * this->y_2);

        std::cout << x << " -> " << K << " -> " << y << std::endl;
        fflush( stdout );

        if (t> 0.001)
        {
            throw 0;
        }

        this->x_2 = this->x_1;
        this->x_1 = x;
        this->y_2 = this->y_1;
        this->y_1 = y;

        return {y, y};
    }

    virtual void pulse()
    {
        this->propagate();
    }
};
