#pragma once

#include <tuple>
#include <memory>
#include <string>

class kora_chain_link : public std::enable_shared_from_this<kora_chain_link>
{
protected:
    std::string name;

    std::shared_ptr<kora_chain_link> in1 = nullptr;
    std::shared_ptr<kora_chain_link> in2 = nullptr;

    std::shared_ptr<kora_chain_link> out1 = nullptr;
    std::shared_ptr<kora_chain_link> out2 = nullptr;

    void propagate();

public:
    bool is_propagating = false;

    virtual float progress();
    virtual void pulse() = 0;
    virtual void post_connect();

    void connect_to(const std::shared_ptr<kora_chain_link>& to_link, uint8_t from_pin, uint8_t to_pin);
};

class sampler : public kora_chain_link
{
protected:
    double sample_rate;
    uint32_t max_quant;
    double last_t = 0;

public:
    std::shared_ptr<sampler> s_in1() { return std::dynamic_pointer_cast<sampler>(in1); }
    std::shared_ptr<sampler> s_in2() { return std::dynamic_pointer_cast<sampler>(in2); }

    std::shared_ptr<sampler> s_out1() { return std::dynamic_pointer_cast<sampler>(out1); }
    std::shared_ptr<sampler> s_out2() { return std::dynamic_pointer_cast<sampler>(out2); }

    sampler(double sample_rate, uint32_t max_quant);
    virtual ~sampler();

    virtual std::tuple<double, double> sample(size_t t) = 0;
    virtual size_t sample_size();
};
