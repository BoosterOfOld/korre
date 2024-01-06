/*
  ==============================================================================

    PFFT.h
    Created: 29 Aug 2022 1:08:12pm
    Author:  Justin Johnson

  ==============================================================================
*/

#pragma once

#include <cmath>
#include <complex>

#include "pffft/pffft.h"

/**
 * C++ Wrapper for pffft, a reasonably fast FFT library.
 *  The class here reflects closely the Juce FFT class and is a drop
 *  in replacement.
 *  See: https://bitbucket.org/jpommier/pffft/src/master/
 */
class PFFFT
{
public:
    PFFFT(int order)
    {
        size_ = 1 << order;
        scale_ = 1.f / size_;
        pSetup_ = pffft_new_setup(size_, PFFFT_REAL);
    }

    PFFFT()
    {
        pffft_destroy_setup(pSetup_);
    }

    void performRealOnlyForwardTransform(float *pBuffer, bool onlyCalculateNonNegativeFrequencies = false)
    {
        pffft_transform_ordered(pSetup_, pBuffer, pBuffer, NULL, PFFFT_FORWARD);
    }

    void performRealOnlyInverseTransform(float *pBuffer)
    {
        pffft_transform_ordered(pSetup_, pBuffer, pBuffer, NULL, PFFFT_BACKWARD);

        for (int i = 0; i < size_; ++i)
        {
            pBuffer[i] *= scale_;
        }
    }

    void performFrequencyOnlyForwardTransform(float *inputOutputData, bool ignoreNegativeFreqs = false) const noexcept
    {
        if (size_ == 1) return;

        pffft_transform_ordered(pSetup_, inputOutputData, inputOutputData, NULL, PFFFT_FORWARD);

        auto *out = reinterpret_cast<std::complex<float> *>(inputOutputData);

        const auto limit = ignoreNegativeFreqs ? (size_ / 2) + 1 : size_;

        for (int i = 0; i < limit; ++i)
        {
            inputOutputData[i] = std::abs(out[i]);
        }

        std::fill(inputOutputData + limit, inputOutputData + size_ * 2, 0.f);
    }

    [[nodiscard]] int getSize() const noexcept { return size_; }

private:
    int size_;
    float scale_;

    PFFFT_Setup *pSetup_;
};