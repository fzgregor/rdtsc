#define _GNU_SOURCE
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include <sys/syscall.h>
#include <sched.h>

// based on https://aufather.wordpress.com/2010/09/08/high-performance-time-measuremen-in-linux/

#ifndef RDTSC_H_GUARD
#define RDTSC_H_GUARD

static __thread uint64_t last_tick = 0;

uint64_t rdtsc() {
    // based on https://github.com/rotschopf/highwayhash/blob/master/highwayhash/tsc_timer.h
    uint64_t t;
    unsigned c;
    asm volatile(
        "lfence\n\t"
        "rdtscp\n\t"
        "shl $32, %%rdx\n\t"
        "or %%rdx, %0\n\t"
        "lfence"
        : "=a"(t), "=c"(c)
        :
        // "memory" avoids reordering. rdx = TSC >> 32.
        // "cc" = flags modified by SHL.
        : "rdx", "memory", "cc");
    if (last_tick > t) {
        fprintf(stderr, "Current tick smaller than previous!!!\n");
    }
    return t;
}

static double calibrate_ticks()
{
    struct timespec begints, endts;
    volatile uint64_t begin = 0, end = 0;
    clock_gettime(CLOCK_MONOTONIC, &begints);
    begin = rdtsc();
    volatile uint64_t i;
    for (i = 0; i < 1000000; i++); /* must be CPU intensive */
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

double elapsed_secs(uint64_t begin) {
    return elapsed_nsecs(begin)/1000000000llu;
}

#endif // RDTSC_H_GUARD
