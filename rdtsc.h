#include <stdint.h>
#include <stdlib.h>
#include <time.h>

// based on https://aufather.wordpress.com/2010/09/08/high-performance-time-measuremen-in-linux/

#ifndef RDTSC_H_GUARD
#define RDRSC_H_GUARD

uint64_t rdtsc() {
    unsigned int a, d;
    asm volatile("rdtsc" : "=a" (a), "=d" (d));
    return (((uint64_t)a) | (((uint64_t)d) << 32));
}

static double calibrate_ticks()
{
    struct timespec begints, endts;
    uint64_t begin = 0, end = 0;
    clock_gettime(CLOCK_MONOTONIC, &begints);
    begin = rdtsc();
    uint64_t i;
    for (i = 0; i < 10000000; i++); /* must be CPU intensive */
    end = rdtsc();
    clock_gettime(CLOCK_MONOTONIC, &endts);
    uint64_t begnsec = begints.tv_sec*1000000000llu + begints.tv_nsec;
    uint64_t endnsec = endts.tv_sec*1000000000llu + endts.tv_nsec;
    return (double)(end - begin)/(double)(endnsec-begnsec);
}

static __thread double ticks_per_nsec = 0.0;

double elapsed_nsecs(uint64_t begin) {
    uint64_t end = rdtsc();
    if (ticks_per_nsec == 0.0) {
        ticks_per_nsec = calibrate_ticks();
    }

    return (end-begin)/ticks_per_nsec;
}

#endif // RDTSC_H_GUARD
