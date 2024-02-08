#pragma once

#include <cfloat>
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>
#include <string>

extern void g_normalize_to_bit_depth(std::shared_ptr<std::vector<double>> signal, uint16_t bit_depth);

class internal_signal
{
private:
    void normalize_to_bit_depth(std::shared_ptr<std::vector<double>> signal) const;
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
    );
    virtual ~internal_signal();

    std::tuple<double, double> sample(size_t t);

    void normalize();
};