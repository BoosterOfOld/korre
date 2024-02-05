#pragma once

#include <cmath>
#include <complex>

#include "sampler.h"
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

    size_t compact_columns = 100; // How many averaged bins are crated for display (in "compacted")
    float *compacted; // Final display array.
    size_t step = 0; // How many samples will be averaged (computed)

    double bin_spacing = 0; // Frequency step between the output FFT bins [Hz].
    size_t bin_range = 0; // The number of the FFT bins that corresponds to last_bin_freq.
    double last_bin_freq = 0; // Last frequency to display [Hz].

    float *wind;  // [nn], Input/Output buffer for FFT. Circular buffer.
    float **wind_f; // [roll][nn], Sliding mean frame buffer.
    float *wind_avg; // [nn], Processed, viewable bins.
    float *wind_avg_log; // [nn], Processed, viewable bins. Log10 X.
    float *wind_avg_log_bins; // Helper -- number of bins crammed into one nn compartment.

    size_t current_roll; // Number of the current sliding mean frame.

    double log_b = 0; // Computed. B scaling factor for rendering log X axis.

    float *hann; // Hanning window.
    bool apply_hann = false;

    float ceiling=1; // The ceiling of the bin display. It should be always 1 now.

    explicit meter(uint32_t sample_rate) : sampler(sample_rate, 0)
    {
        name = "Meter";

        order = 12; // 9, 12 -- PUT THIS TO SETTINGS !!!!!!!
        n = pow(2, order);
        nn = n * 2;

        log_b = (double)nn / logfn(base, nn);

        update_range(20000);

        wind = new float[nn];
        wind_f = new float*[roll];
        wind_avg = new float[nn];
        wind_avg_log = new float[nn];
        wind_avg_log_bins = new float[nn];
        compacted = new float[compact_columns];

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
            wind_avg_log[j] = 0;
            wind_avg_log_bins[j] = 0;
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
        bin_range = log_b * logfn(base, bin_range + 1); // This changes also with log x.
        step = bin_range / compact_columns;
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
            ceiling = 0; // This now only serves to normalize FFT bins, not for display.

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
            }

            // Logarithmic bins.
            process_log();

            // Normalization of log bins.
            for (int i = 0; i < nn; ++i)
            {
                if (wind_avg_log[i] > ceiling)
                {
                    ceiling = wind_avg_log[i];
                }
            }
            for (int i = 0; i < nn; ++i)
            {
                wind_avg_log[i] /= ceiling;
            }

            // More reduction for final display.
            for (size_t i = 0; i < compact_columns; ++i)
            {
                compacted[i] = 0;
                for(size_t j = 0; j < step; ++j)
                {
                    compacted[i] += wind_avg_log[(step*i)+j];
                }
                compacted[i] /= (float)step;
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

    double logfn(double base, double x)
    {
        return (log(x)/log(base));
    }

    double base = 10; // log base for logs in this file

    void process_log()
    {
        double log_value = 0;
        int log_index = 0;

        for (int i = 0; i < nn; ++i)
        {
            wind_avg_log[i] = 0;
            wind_avg_log_bins[i] = 0;
        }

        // Forward fill of the log bins.
        for (int i = 0; i < nn; ++i)
        {
            log_value = log_b * logfn(base,i + 1);
            log_index = ceil(log_value);

            if (log_index > i)
            {
                wind_avg_log[log_index] += wind_avg[i];
                ++wind_avg_log_bins[log_index];
            }
        }

        double roll_call = 1;

        // Backward fill the spaces. (Multiple linear bins make it to a single log bin,
        // but that also means that some log bins don't get any linear bins and must have
        // the value of the nearest filled bin). Why from the right? I don't know, I just tried it.
        for (int i = nn - 1; i >= 0; --i)
        {
            if (wind_avg_log_bins[i] > 0)
            {
                wind_avg_log[i] /= wind_avg_log_bins[i];
                roll_call = wind_avg_log[i];
            }
            else
            {
                wind_avg_log[i] = roll_call;
            }
        }
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
