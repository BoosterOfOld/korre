#pragma once

#include <tuple>
#include <cstdint> // Necessary for uint32_t

class kora_chain_link : public std::enable_shared_from_this<kora_chain_link>
{
protected:
    std::shared_ptr<kora_chain_link> in1 = nullptr;
    std::shared_ptr<kora_chain_link> in2 = nullptr;

    std::shared_ptr<kora_chain_link> out1 = nullptr;
    std::shared_ptr<kora_chain_link> out2 = nullptr;

    void propagate()
    {
        if (this->out1 != nullptr)
        {
            this->out1->pulse();
        }
        if (this->out2 != nullptr)
        {
            this->out2->pulse();
        }
    }

public:
    bool is_propagating = false;

    virtual float progress()
    {
        return 1.f;
    }

    virtual void pulse() = 0;

    virtual void post_connect()
    {
        ;
    }

    void connect_to(const std::shared_ptr<kora_chain_link>& to_link, uint8_t from_pin, uint8_t to_pin)
    {
        if (to_pin == 1)
        {
            to_link->in1 = shared_from_this();
        }
        else if (to_pin == 2)
        {
            to_link->in2 = shared_from_this();
        }

        if (from_pin == 1)
        {
            this->out1 = to_link;
        }
        else if (from_pin == 2)
        {
            this->out2 = to_link;
        }

        post_connect();
    }
};

class sampler : public kora_chain_link
{
protected:
    // Input
    double sample_rate;
    uint32_t max_quant;
    double last_t = 0;
public:
    //size_t counter = 0;

    std::shared_ptr<sampler> s_in1() { return std::dynamic_pointer_cast<sampler>(in1); }
    std::shared_ptr<sampler> s_in2() { return std::dynamic_pointer_cast<sampler>(in2); }

    std::shared_ptr<sampler> s_out1() { return std::dynamic_pointer_cast<sampler>(out1); }
    std::shared_ptr<sampler> s_out2() { return std::dynamic_pointer_cast<sampler>(out2); }

    sampler(double sample_rate, uint32_t max_quant)
    {
        this->sample_rate = sample_rate;
        this->max_quant = max_quant;
    }

    virtual std::tuple<double, double> sample(size_t t) = 0;

    virtual size_t sample_size()
    {
        return 0;
    }

    virtual ~sampler()
    {
        ;
    }
};
