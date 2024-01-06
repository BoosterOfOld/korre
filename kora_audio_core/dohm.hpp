#pragma once

#include <math.h>
#include <tuple>
#include <iostream>
#include <vector>
#include <algorithm>
#include <mutex>

#include "wave.hpp"
#include "sampler.hpp"
#include "pitch_shift.hpp"
#include "utils.hpp"


template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}


class dohm : public sampler
{
private:
    wave w;

public:
    dohm(double sample_rate, int16_t max_quant) : sampler(sample_rate, max_quant)
    {
        w.load_wav("/home/kora/projects/kora/senn.wav");
        //w.load_wav("/home/kora/projects/kora/sample69a.wav");

    }

    virtual std::tuple<double, double> sample(size_t t)
    {
        std::tuple<double, double> ret = {(*(w.l))[t],(*(w.r))[t]};

        return ret;
    }

    virtual size_t sample_size()
    {
        return w.l->size();
    }

    virtual void pulse()
    {
        this->propagate();
    }

    virtual ~dohm()
    {

    }
};
