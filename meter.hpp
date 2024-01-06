#pragma once

#include "sampler.hpp"
#include "cmath"
#include "complex"

#include "pffft/pfft.h"

const double samplage = 0.011;

class meter : public sampler
{
public:
    double l;
    double r;

    double ln;
    double rn;

    size_t pos;
    size_t pos_max;

    PFFFT *p;

    size_t order = 9;
    size_t n = 512;
    size_t nn = 512 * 2;
    size_t roll = 16;//8;

    double bin_spacing = 0;
    size_t bin_range = 0;
    double last_bin_freq = 0;

    float *wind;  // nn
    float **wind_f; // roll nn
    float *wind_avg; // nn
    size_t current_roll;

    volatile float ceiling=1;

    explicit meter(uint32_t sample_rate, uint32_t max_quant) : sampler(sample_rate, max_quant)
    {
        order = 9; // 9 -- PUT THIS TO SETTINGS !!!!!!!
        n = pow(2, order);
        nn = n * 2;

        update_range(20000);

        wind = new float[nn];
        wind_f = new float*[roll];
        wind_avg = new float[nn];

        for(int i = 0; i<roll; ++i)
        {
            wind_f[i] = new float[nn];
        }

        p = new PFFFT(order);

        for (int i = 0; i < roll; ++i)
        {
            //std::fill(std::begin(wind_f[i]),std::begin(wind_f[i])+nn,0);
            for (int j = 0; j < nn; ++j)
            {
                wind_f[i][j] = 0;
            }
        }

        //std::fill(std::begin(wind_avg),std::begin(wind_avg)+nn,0);
        for (int j = 0; j < nn; ++j)
        {
            wind_avg[j] = 0;
        }

        current_roll = 0;
    }

    void update_range(double freq)
    {
        last_bin_freq = freq;
        bin_spacing = (double)sample_rate / (double)n;
        bin_range = (size_t)last_bin_freq / (size_t)bin_spacing;
    }

    virtual std::tuple<double, double> sample(size_t t) override
    {
        auto sample = s_in1()->sample(t);

        l = std::get<0>(sample);
        r = std::get<1>(sample);


        wind[t%nn] = (float)l;
        //wind[t%nn] = (((int32_t)l) >> 8);

        // 0.011s
        if (t!=0 && t % (size_t)(samplage*sample_rate) == 0)
        //if (wind_cnt!=0 && ((wind_cnt++ % nn) == 0))
        {
            auto pc=ceiling;

            //memcpy(wind_f[current_roll], wind, sizeof(float)*nn); // sizeof(wind_f[current_roll])

            size_t offset;
            for (int i = 0; i < nn; ++i)
            {
                offset = t%nn;
                wind_f[current_roll][i] = wind[(i + offset) % nn];
            }

            for (int i = 0; i < nn; ++i)
            {
                wind_f[current_roll][i] = wind_f[current_roll][i];// / (float)max_quant;
                auto a = wind_f[current_roll][i];
                ;
            }

            p->performFrequencyOnlyForwardTransform(wind_f[current_roll], true);

            for (int i = 0; i < nn; ++i)
            {
                wind_avg[i] = 0;
                for (int j=0; j<roll; ++j)
                {
                    wind_avg[i] += wind_f[j][i]; //pow(wind_f[j][i], 2);
                }
                wind_avg[i] /= roll;
                auto x = wind_avg[i];

                if (wind_avg[i] > ceiling)
                {
                    ceiling = wind_avg[i];
                }
            }

            if (ceiling == pc)
            {
                ceiling -= (0.009f * ceiling); //0.0009f
            }

            ++current_roll;
            current_roll %= roll;
        }



        if (t % 100 == 0)
        {
            ln = abs(l) / (float) max_quant;
            rn = abs(r) / (float) max_quant;

            pos = t;
            pos_max = s_in1()->sample_size();
        }

        /*
        ++current_roll;
        current_roll %= roll;*/

        return { l, r };
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
