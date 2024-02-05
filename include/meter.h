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
    double base = 10; // log base for logs in this file

    explicit meter(uint32_t sample_rate);
    void update_range(double freq);
    virtual std::tuple<double, double> sample(size_t t) override;
    double logfn(double base, double x);
    void process_log();
    virtual size_t sample_size() override;
    virtual void pulse() override;
};
