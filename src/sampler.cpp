#include "sampler.h"

void kora_chain_link::propagate()
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

float kora_chain_link::progress()
{
    return 1.f;
}

void kora_chain_link::post_connect()
{
    ;
}

void kora_chain_link::connect_to(const std::shared_ptr<kora_chain_link>& to_link, uint8_t from_pin, uint8_t to_pin)
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



sampler::sampler(double sample_rate, uint32_t max_quant)
{
    this->sample_rate = sample_rate;
    this->max_quant = max_quant;
}

sampler::~sampler()
{
    ;
}

size_t sampler::sample_size()
{
    return 0;
}
