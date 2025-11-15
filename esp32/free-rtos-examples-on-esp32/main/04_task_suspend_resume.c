/**
 *
 * Demonstrates how FreeRTOS task priorities and task suspension work on ESP32.
 *
 * What this example does:
 *  - Creates a low-priority task (task_low) that prints once per second.
 *  - Creates a high-priority task (task_high) that prints twice per second.
 *  - The high-priority task occasionally SUSPENDS task_low for 3 seconds,
 *    then RESUMES it.
 *
 * What you will observe in the Serial Monitor:
 *  - Normally, both tasks print (high task prints more often because it runs twice as fast).
 *  - When the low task is suspended, only the high task prints messages.
 *  - After 3 seconds, the low task resumes and starts printing again.
 *
 * Notes:
 *  - vTaskSuspend() stops a task immediately; it will not run until vTaskResume() is called.
 *  - ESP-IDF uses configUSE_PREEMPTION = 1 by default, so higher priority tasks preempt lower ones.
 *  - Never suspend system tasks like Idle or Timer — only suspend your own tasks.
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Handle for the low-priority task so the high-priority task can suspend/resume it.
static TaskHandle_t g_low_task_handle = NULL;


/**
 * Low-priority task (priority 3)
 *
 * This task prints its core number once every second.
 * The high-priority task can suspend and resume this task at any time.
 */
void task_low(void *pvParameter)
{
    (void) pvParameter; // Not used, but prevents compiler warnings

    while (1) {
        printf("[LOW ] Core %d: running\n", xPortGetCoreID());
        vTaskDelay(pdMS_TO_TICKS(1000));  // Run once per second
    }
}


/**
 * High-priority task (priority 8)
 *
 * This task prints twice per second.
 * Every few iterations it suspends the low-priority task for 3 seconds.
 * During those 3 seconds, only the high task will print messages,
 * showing clearly that the low task is paused.
 */
void task_high(void *pvParameter)
{
    (void) pvParameter;

    int iter = 0;
    const TickType_t suspend_time = pdMS_TO_TICKS(3000);  // 3 seconds

    while (1) {
        printf("[HIGH] Core %d: running (iter=%d)\n", xPortGetCoreID(), iter);

        // Every 6 iterations (500ms * 6 = 3 seconds), suspend the low task
        if ((iter % 6) == 0 && g_low_task_handle != NULL) {

            printf("[HIGH] Suspending LOW task for 3 seconds...\n");
            vTaskSuspend(g_low_task_handle);   // Pause the low-priority task

            // While LOW is suspended, the HIGH task continues running alone
            TickType_t start = xTaskGetTickCount();
            while ((xTaskGetTickCount() - start) < suspend_time) {
                printf("[HIGH] LOW task is suspended...\n");
                vTaskDelay(pdMS_TO_TICKS(500)); // Print every 500ms
            }

            printf("[HIGH] Resuming LOW task now.\n");
            vTaskResume(g_low_task_handle);    // Bring the low task back
        }

        vTaskDelay(pdMS_TO_TICKS(500)); // High task runs twice per second
        iter++;
    }
}


/**
 * app_main()
 *
 * Entry point for ESP-IDF applications.
 * Creates both tasks with appropriate priorities.
 */
void app_main()
{
    // Create the low-priority task (priority 3)
    xTaskCreate(
        task_low,
        "LowPriority",
        2048,
        NULL,
        3,
        &g_low_task_handle     // We save the task handle so we can suspend/resume it
    );

    // Create the high-priority task (priority 8)
    xTaskCreate(
        task_high,
        "HighPriority",
        2048,
        NULL,
        8,
        NULL // We don't need its handle
    );
}