#pragma once

#include <memory>

#include "sampler.h"

using std::size_t;

class sink : public kora_chain_link
{
protected:
    std::shared_ptr<sampler> s_in1() { return std::dynamic_pointer_cast<sampler>(in1); }
    std::shared_ptr<sampler> s_in2() { return std::dynamic_pointer_cast<sampler>(in2); }

    std::shared_ptr<sampler> s_out1() { return std::dynamic_pointer_cast<sampler>(out1); }
    std::shared_ptr<sampler> s_out2() { return std::dynamic_pointer_cast<sampler>(out2); }

public:
    size_t counter = 0;

    sink();
    virtual ~sink();

    virtual void open() = 0;
    virtual void plug() = 0;
    virtual bool is_open() = 0;
};
