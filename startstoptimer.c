#include <stdio.h>
#include <time.h>
#include "startstoptimer.h"

// CLOCK_MONOTONIC_RAW is a Linux/Darwin extension so it might not be available
// on Posix systems. It is preferred because unaffected by any NTP adjustments
// including clock frequency (which is unrelated to CPU frequency, so even
// CLOCK_MONOTONIC will not vary with varying CPU frequency.)

// Difference calculation from https://en.cppreference.com/w/c/chrono/clock
// Not sure about implications of simplifying that, e.g. as:
// 1e3 * (t1.tv_sec - t0.tv_sec) + 1e-6 * (t1.tv_nsec - t0.tv_nsec)
// which on the face of it could have better precision.

#ifndef CLOCK_MONOTONIC_RAW
#define CLOCK_MONOTONIC_RAW CLOCK_MONOTONIC
#endif

static struct timespec t0 = {0}, t1 = {0};

void starttimer(void)
{
    // Warn on Raspberry Pi if not running at max performance
    FILE *f = fopen("/sys/devices/system/cpu/cpufreq/policy0/scaling_governor", "r");
    if (f) {
        if (fgetc(f) != 'p' || fgetc(f) != 'e')
            fprintf(stderr,
                "Warning: CPU not optimised for performance.\n"
                "  Resolve: echo performance | sudo tee /sys/devices/system/cpu/cpufreq/policy0/scaling_governor\n"
                "  Setting will be restored to default 'ondemand' at reboot.\n");
        fclose(f);
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &t0);
}

double stoptimer_ns(void)
{
    clock_gettime(CLOCK_MONOTONIC_RAW, &t1);
    return 1e9 * t1.tv_sec + 1.0 * t1.tv_nsec - (1e9 * t0.tv_sec + 1.0 * t0.tv_nsec);
}

double stoptimer_us(void)
{
    clock_gettime(CLOCK_MONOTONIC_RAW, &t1);
    return 1e6 * t1.tv_sec + 1e-3 * t1.tv_nsec - (1e6 * t0.tv_sec + 1e-3 * t0.tv_nsec);
}

double stoptimer_ms(void)
{
    clock_gettime(CLOCK_MONOTONIC_RAW, &t1);
    return 1e3 * t1.tv_sec + 1e-6 * t1.tv_nsec - (1e3 * t0.tv_sec + 1e-6 * t0.tv_nsec);
}

double stoptimer_s(void)
{
    clock_gettime(CLOCK_MONOTONIC_RAW, &t1);
    return 1.0 * t1.tv_sec + 1e-9 * t1.tv_nsec - (1.0 * t0.tv_sec + 1e-9 * t0.tv_nsec);
}
