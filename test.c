#include <stdio.h>
#include <stdint.h>

#include "rdtsc.h"


int main() {
    uint32_t i = 0;

    uint64_t begin = rdtsc();
    printf("Empty rdtsc takes %f nsecs.\n", elapsed_nsecs(begin));

    begin = rdtsc();
    for (i=0; i<10000; i++) {}
    printf("Empty for-loop with 10000 iterations takes %f nsecs.\n", elapsed_nsecs(begin));
}
