#pragma once

#include <vector>

#include "sampler.h"
#include "wave.h"
#include "internal_signal.h"

class wave_source : public sampler
{
public:
    wave *w;

    std::vector<std::shared_ptr<internal_signal>> iss;
    int selected_signal = -1;

    explicit wave_source(wave *w, uint32_t sample_rate);
    ~wave_source() override;

    void add_signal(std::shared_ptr<internal_signal> sig);
    void select_signal(int sn);

    virtual std::tuple<double, double> sample(size_t t) override;
    virtual size_t sample_size() override;
    virtual void pulse() override;
};