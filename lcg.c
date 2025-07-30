#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define SAMPLE_SIZE 10000
#define HIST_BUCKETS 10

// Linear Congruential Generator
static inline uint32_t lcg(uint64_t *state, uint64_t a, uint64_t c, uint64_t m) {
    *state = (a * (*state) + c) % m;
    return (uint32_t)(*state);
}

// Analyze generated samples
void analyze_randomness(uint32_t *samples, size_t size, uint64_t m) {
    double mean = 0, variance = 0;
    int histogram[HIST_BUCKETS] = {0};
    size_t collisions = 0;

    // 1-byte bitmap for simple duplicate detection
    uint8_t seen[SAMPLE_SIZE] = {0};

    for (size_t i = 0; i < size; i++) {
        double norm = (double)samples[i] / m;
        mean += norm;

        int bucket = (int)(norm * HIST_BUCKETS);
        if (bucket >= HIST_BUCKETS) bucket = HIST_BUCKETS - 1;
        histogram[bucket]++;

        size_t index = samples[i] % size;
        if (seen[index]) collisions++;
        else seen[index] = 1;
    }

    mean /= size;

    for (size_t i = 0; i < size; i++) {
        double norm = (double)samples[i] / m;
        variance += (norm - mean) * (norm - mean);
    }

    variance /= size;

    printf("\n--- Randomness Analysis ---\n");
    printf("Sample Size         : %zu\n", size);
    printf("Mean                : %.6f\n", mean);
    printf("Variance            : %.6f\n", variance);
    printf("Estimated Collisions: %zu\n", collisions);

    printf("\nHistogram (normalized buckets):\n");
    for (int i = 0; i < HIST_BUCKETS; i++) {
        printf("[%2d-%3d%%]: ", i * 10, (i + 1) * 10);
        int stars = histogram[i] * 100 / size;
        for (int j = 0; j < stars; j++) printf("*");
        printf("\n");
    }
}

int main() {
    uint64_t seed, a, c, m;

    printf("Enter LCG parameters:\n");
    printf("Seed (X₀)       : "); scanf("%llu", &seed);
    printf("Multiplier (a)  : "); scanf("%llu", &a);
    printf("Increment (c)   : "); scanf("%llu", &c);
    printf("Modulus (m)     : "); scanf("%llu", &m);

    if (m == 0) {
        printf("Error: Modulus cannot be zero.\n");
        system("pause");
        return 1;
    }

    uint32_t *samples = malloc(SAMPLE_SIZE * sizeof(uint32_t));
    if (!samples) {
        printf("Error: Memory allocation failed.\n");
        system("pause");
        return 1;
    }

    clock_t start = clock();

    // Generate random numbers
    uint64_t state = seed;
    for (size_t i = 0; i < SAMPLE_SIZE; i++) {
        samples[i] = lcg(&state, a, c, m);
    }

    // Stop timer
    clock_t end = clock();
    double elapsed = ((double)(end - start)) * 1000.0 / CLOCKS_PER_SEC;
    size_t mem_used = SAMPLE_SIZE * sizeof(uint32_t);

    analyze_randomness(samples, SAMPLE_SIZE, m);

    printf("\n--- Generation Summary ---\n");
    printf("Numbers generated   : %d\n", SAMPLE_SIZE);
    printf("Memory used         : %.2f KB (%zu bytes)\n", mem_used / 1024.0, mem_used);
    printf("Execution time      : %.2f ms\n", elapsed);

    free(samples);
    system("pause");  // Windows only
    return 0;
}
