/**
 * Demonstrates the difference between:
 *
 *   - vTaskDelay()      → relative delays (can drift over time)
 *   - vTaskDelayUntil() → absolute periodic schedule (keeps perfect timing)
 *
 * Both tasks run every 1 second, but only one maintains strict timing.
 *
 * What you will observe in the Serial Monitor:
 *   - The vTaskDelay() task prints timestamps that slowly drift (e.g., 1000, 2001, 3003…)
 *   - The vTaskDelayUntil() task stays almost exact (1000, 2000, 3000…)
 *
 * This is one of the most important timing concepts in FreeRTOS.
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


/**
 * Task using vTaskDelay()
 *
 * This uses a relative delay:
 *     vTaskDelay(1000ms)
 *
 * Every loop takes:
 *     printing time + scheduler overhead + 1000ms
 *
 * Because printing and other overhead take time, this method accumulates drift.
 */
void task_delay(void *pvParameter)
{
    (void)pvParameter;

    while (1) {
        // Print uptime in milliseconds
        printf("[vTaskDelay     ] Time = %lu ms\n", (unsigned long)(xTaskGetTickCount() * portTICK_PERIOD_MS));

        // Delay for 1000ms relative to NOW
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


/**
 * Task using vTaskDelayUntil()
 *
 * This method creates an EXACT 1-second periodic loop.
 *
 * The idea:
 *   1. Record the last wake time
 *   2. Wait UNTIL (last_wake + period)
 *
 * The task will always wake up at precise intervals
 * regardless of how long the previous iteration took (as long as it finished on time).
 */
void task_delay_until(void *pvParameter)
{
    (void)pvParameter;

    // Record the current tick count as the reference point
    TickType_t last_wake = xTaskGetTickCount();

    while (1) {
        // Print uptime in milliseconds
        printf("[vTaskDelayUntil] Time = %lu ms\n", (unsigned long)(xTaskGetTickCount() * portTICK_PERIOD_MS));

        // Wait until (last_wake + 1000ms)
        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(1000));
    }
}


/**
 * app_main()
 *
 * Creates two tasks with the same priority.
 *
 * Both run once per second, but one drifts and the other stays perfectly periodic.
 */
void app_main()
{
    printf("=== FreeRTOS Delay vs DelayUntil Example Starting ===\n");

    // Task using vTaskDelay() — relative timing
    xTaskCreate(
        task_delay,
        "TaskDelay",
        2048,
        NULL,
        5,
        NULL
    );

    // Task using vTaskDelayUntil() — precise periodic timing
    xTaskCreate(
        task_delay_until,
        "TaskDelayUntil",
        2048,
        NULL,
        5,
        NULL
    );
}
