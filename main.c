#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pico/stdlib.h>
#include "rnnoise.h"

int main()
{
    // Initialize USB standard I/O and GPIO for the LED
    stdio_init_all();
    sleep_ms(20000);
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Add a small delay to allow time for the serial connection to stabilize

    // Blink LED to indicate the program has started
    gpio_put(LED_PIN, 1);  // Turn LED on
    sleep_ms(500);
    gpio_put(LED_PIN, 0);  // Turn LED off

    // Print a message to indicate the benchmark is starting
    printf("Starting RNNoise benchmark...\n");

    // Create RNNoise denoise state
    DenoiseState* st = rnnoise_create(NULL);

    #define FRAME_SIZE 480
    float x[FRAME_SIZE];

    // run once without timing to initialize RNNoise
    memset(x, 0x00, sizeof(x));
    rnnoise_process_frame(st, x, x);

    const int benchmark_iterations = 100;
    absolute_time_t rnnoise_process_frame_time = 0;

    // Main benchmark loop
    for (int j = 0; j < benchmark_iterations; j++) {
        // Blink LED for each iteration to show activity
        gpio_put(LED_PIN, 1);  // LED on
        sleep_ms(100);         // Blink to indicate running
        gpio_put(LED_PIN, 0);  // LED off

        // Generate random data for the frame
        for (int i = 0; i < FRAME_SIZE; i++) {
            x[i] = ((float)rand()) / ((float)RAND_MAX) * 0x7fff;
        }

        // Measure the time taken for processing a single frame
        absolute_time_t usec_rnnoise_process_frame_start = get_absolute_time();
        rnnoise_process_frame(st, x, x);
        absolute_time_t usec_rnnoise_process_frame_end = get_absolute_time();

        rnnoise_process_frame_time += (usec_rnnoise_process_frame_end - usec_rnnoise_process_frame_start);

        // Print progress every 10 iterations
        if (j % 10 == 0) {
            printf("Completed iteration %d\n", j);
        }
    }

    // Output the average time taken per frame in microseconds
    printf("RNNoise process frame average time: %llu usec\n", (rnnoise_process_frame_time / benchmark_iterations));

    // Blink the LED continuously after completing the benchmark
    while (1) {
        gpio_put(LED_PIN, 1);  // Turn LED on
        sleep_ms(500);         // Wait 500ms
        gpio_put(LED_PIN, 0);  // Turn LED off
        sleep_ms(500);         // Wait 500ms
    }

    return 0;
}
