#pragma once

#include <cmath>
#include <complex>

#include "sampler.hpp"
#include "tukey.h"

#include "pffft/pfft.h"

class meter : public sampler
{
public:
    double l;
    double r;

    size_t pos = 0;
    size_t pos_max = 1;

    PFFFT *p;
    const double fft_calc_time = 0.011; // The time at which to calculate FFT.

    size_t order = 9; // Order of the FFT.
    size_t n = 512; // Block size of the FFT (2^order).
    size_t nn = 512 * 2; // Input/Output array size (2*n).
    size_t roll = 16; // Size of the window of the sliding mean of the FFT frames [samples].

    double bin_spacing = 0; // Frequency step between the output FFT bins [Hz].
    size_t bin_range = 0; // The number of the FFT bin that corresponds to last_bin_freq.
    double last_bin_freq = 0; // Last frequency to display [Hz].

    float *wind;  // [nn], Input/Output buffer for FFT. Circular buffer.
    float **wind_f; // [roll][nn], Sliding mean frame buffer.
    float *wind_avg; // [nn], Processed, viewable bins.
    size_t current_roll; // Number of the current sliding mean frame.

    float *hann; // Hanning window.
    bool apply_hann = false;

    float ceiling=1; // The ceiling of the bin display

    explicit meter(uint32_t sample_rate) : sampler(sample_rate, 0)
    {
        name = "Meter";

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
            for (int j = 0; j < nn; ++j)
            {
                wind_f[i][j] = 0;
            }
        }

        for (int j = 0; j < nn; ++j)
        {
            wind_avg[j] = 0;
        }

        current_roll = 0;

        hann = new float[nn];
        tukey(hann, nn, 0.5);
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

        // Circular buffer.
        wind[t%nn] = ((float)l + (float)r) / 2.f;

        // Apply Hanning window.
        if (apply_hann)
        {
            wind[t%nn] *= hann[t%nn];
        }

        // Calculates every 0.011s.
        if (t!=0 && t % (size_t)(fft_calc_time * sample_rate) == 0)
        {
            if (ceiling > 4294967296.f)
            {
                ceiling = 0;
            }
            // Ceiling before its update.
            auto pc = ceiling;

            size_t offset;
            // Copy the buffer into the FFT input buffer, which is the current sliding mean frame.
            for (int i = 0; i < nn; ++i)
            {
                offset = t%nn; // Offset of the circular buffer in wind.
                wind_f[current_roll][i] = wind[(i + offset) % nn];
            }

            // FFT itself.
            p->performFrequencyOnlyForwardTransform(wind_f[current_roll], true);

            // Calculate the average of the sliding mean FFT frames.
            for (int i = 0; i < nn; ++i)
            {
                wind_avg[i] = 0;
                for (int j=0; j<roll; ++j)
                {
                    wind_avg[i] += wind_f[j][i];
                }
                wind_avg[i] /= roll;

                // Raise ceiling.
                if (wind_avg[i] > ceiling)
                {
                    ceiling = wind_avg[i];
                }
            }

            // If the ceiling wasn't raised, start dropping it in order to utilize the entire range
            // of the graph.
            if (ceiling == pc)
            {
                // Constant set experimentally -- works well.
                ceiling -= (0.009f * ceiling);
            }

            // Advance sliding window framw.
            ++current_roll;
            current_roll %= roll;

            // Update track position.
            pos = t;
            pos_max = s_in1()->sample_size();
        }

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
