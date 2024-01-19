#pragma once

#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

void g_normalize_to_bit_depth(std::shared_ptr<std::vector<double>> signal, uint16_t bit_depth)
{
    double max = 0;
    double curr;
    double absc;
    for(auto i = 0; i < signal->size(); ++i)
    {
        curr = (*signal)[i];
        absc = abs(curr);
        if ( absc > max )
        {
            max = absc;
        }
    }

    double normal = ((pow(2,bit_depth) - 1) / max) / 2;

    double mn = DBL_MAX;
    double mx = DBL_MIN;

    for(auto i = 0; i < signal->size(); ++i)
    {
        (*signal)[i] *= normal;

        if ((*signal)[i] > mx)
        {
            mx = (*signal)[i];
        }
        if ((*signal)[i] < mn)
        {
            mn = (*signal)[i];
        }
    }
}

class internal_signal
{
private:
    void normalize_to_bit_depth(std::shared_ptr<std::vector<double>> signal) const
    {
        g_normalize_to_bit_depth(std::move(signal), bit_depth);
    }
public:
    uint16_t sampling_rate;
    uint16_t bit_depth;
    std::shared_ptr<std::vector<double>> l;
    std::shared_ptr<std::vector<double>> r;
    std::string name;

    explicit internal_signal(
            uint16_t sampling_rate,
            uint16_t bit_depth,
            std::string name,
            std::shared_ptr<std::vector<double>> l,
            std::shared_ptr<std::vector<double>> r
    )
    {
        this->sampling_rate = sampling_rate;
        this->bit_depth = bit_depth;
        this->name = std::move(name);
        this->l = std::move(l);
        this->r = std::move(r);
    }
    virtual ~internal_signal()
    {
        ;
    }

    std::tuple<double, double> sample(size_t t)
    {
        return { l->at(t), r->at(t) };
    }

    void normalize()
    {
        normalize_to_bit_depth(l);
        normalize_to_bit_depth(r);
    }
};