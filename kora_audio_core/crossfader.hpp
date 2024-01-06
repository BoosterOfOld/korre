#pragma once

#include <math.h>
#include <tuple>
#include <iostream>
#include <mutex>
#include <vector>

#include "sampler.hpp"

class crossfader : public sampler
{
private:
    std::unique_ptr<std::thread> switcher;

    std::mutex mx;

    std::unique_ptr<std::vector<float>> output_buffer_l;
    std::unique_ptr<std::vector<float>> output_buffer_r;

    float progressv = 0;

public:
    crossfader(double sample_rate, int16_t max_quant) : sampler(sample_rate, max_quant)
    {
        ;
    }

    ~crossfader() override
    {
        if (switcher != nullptr && switcher->joinable())
        {
            switcher->join();
        }
    }

    void exchange_buffers(std::vector<float> &&newl, std::vector<float> &&newr)
    {
        mx.lock();
        output_buffer_l = std::make_unique<std::vector<float>>(newl);
        output_buffer_r = std::make_unique<std::vector<float>>(newr);
        mx.unlock();
    }

    static void* crossfade_proxy(crossfader* xf)
	{
        xf->perform_crossfade();
		return nullptr;
	}

    void perform_crossfade()
    {
        update();
        propagate();
    }

    virtual void post_connect() override
    {
        exchange_buffers(std::vector<float>(s_in1()->sample_size()), std::vector<float>(s_in1()->sample_size()));

        update();
    }

    void update()
    {
        is_propagating = true;
        std::tuple<double, double> s;

        exchange_buffers(std::vector<float>(s_in1()->sample_size()), std::vector<float>(s_in1()->sample_size()));

        for (auto i(0); i < s_in1()->sample_size(); ++i)
        {
            progressv = (float)i / (float)(s_in1()->sample_size());

            s = s_in1()->sample(i);
            mx.lock();
            (*output_buffer_l)[i] = std::get<0>(s);
            (*output_buffer_r)[i] = std::get<1>(s);
            mx.unlock();
        }

        progressv = 0.f;
        is_propagating = false;
    }

    virtual size_t sample_size() override
    {
        return this->s_in1()->sample_size();
    }

    virtual std::tuple<double, double> sample(size_t t) override
    {
        size_t term_point = s_in1()->sample_size();

        size_t c = t; //= counter;
        size_t c_next = (c + (size_t)floor(term_point/2)) % term_point;

        std::tuple<double, double> ret;

        void (*crossfade_fn)(double, double &, double &, double);

        crossfade_fn = &energyCrossfadePair;

        mx.lock();
        double l_sample = (*output_buffer_l)[c];
        double l_sample_next = (*output_buffer_l)[c_next];

        double r_sample = (*output_buffer_r)[c];
        double r_sample_next = (*output_buffer_r)[c_next];
        mx.unlock();


        if (c > floor(term_point / 2))
        {
            double xfade_l_a = 0.f;
            double xfade_l_b = 0.f;

            double xfade_r_a = 0.f;
            double xfade_r_b = 0.f;

            crossfade_fn((c-((double)term_point / 2.f))/((double)term_point / 2.f), xfade_l_a, xfade_l_b, 0);
            crossfade_fn((c-((double)term_point / 2.f))/((double)term_point / 2.f), xfade_r_a, xfade_r_b, 0);

            double ly = l_sample * xfade_l_b + l_sample_next * xfade_l_a;
            double ry = r_sample * xfade_r_b + r_sample_next * xfade_r_a;

            ret = {ly, ry};
        }
        else if (c <= floor(term_point / 2))
        {
            double xfade_l_a = 0.f;
            double xfade_l_b = 0.f;

            double xfade_r_a = 0.f;
            double xfade_r_b = 0.f;

            crossfade_fn(c/((double)term_point / 2.f), xfade_l_a, xfade_l_b, 0);
            crossfade_fn(c/((double)term_point / 2.f), xfade_r_a, xfade_r_b, 0);

            double ly = l_sample * xfade_r_a + l_sample_next * xfade_r_b;
            double ry = r_sample * xfade_l_a + r_sample_next * xfade_l_b;

            ret = {ly, ry};
        }

        return ret;
    }

    static void energyCrossfadePair(double x, double &fadeIn, double &fadeOut, double p)
    {
        double x2 = 1 - x;
        double A = x*x2;
        double B = A*(1 + (double)1.4186*A);
        double C = (B + x), D = (B + x2);
        fadeIn = C*C;
        fadeOut = D*D;
    }

    static void fracPowerCrossfadePair(double x, double &fadeIn, double &fadeOut, double p)
    {
        double x2 = 1 - x;
        double A = x*x2;
        double k = -6.0026608 + p*(6.8773512 - p*1.5838104);
        double B = A*(1 + (double)k*A);
        double C = (B + x), D = (B + x2);
        fadeIn = C*C;
        fadeOut = D*D;
    }

    virtual float progress() override
    {
        return progressv;
    }

    virtual void pulse() override
    {
        if (is_propagating)
        {
            return;
        }

        if (switcher != nullptr && switcher->joinable())
        {
            switcher->join();
        }

        is_propagating = true;

        switcher = std::make_unique<std::thread>(crossfade_proxy, this);
    }

};
