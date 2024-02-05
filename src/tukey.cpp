#include <cmath>

void tukey(float *w, unsigned n, float r)
{
    // Tukey window.

    // This window uses a cosine-shaped ramp-up and ramp-down, with an all-one part in the middle.
    // The parameter 'r' defines the fraction of the window covered by the ramp-up and ramp-down.

    // r <= 0 is identical to a rectangular window.
    // r >= 1 is identical to a Hann window.
    //
    // In Matlab, the default value for parameter r is 0.5.

    if (n == 1)
    {
        // Special case for n == 1.
        w[0] = 1.f;
    }
    else
    {
        r = fmax(0.f, fmin(1.f, r)); // Clip between 0 and 1.

        for (unsigned i = 0; i < n; ++i)
        {
            w[i] = (cos(fmax(fabs((float)i - (n - 1) / 2.f) * (2.f / (n - 1) / r)  - (1.f / r - 1.f), 0.f) * M_PI) + 1.f) / 2.f;
        }
    }
}