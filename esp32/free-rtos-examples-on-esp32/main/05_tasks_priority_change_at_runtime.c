/**
 *
 * Demonstrates how FreeRTOS handles multiple tasks with different priorities
 * and how a task can change its own priority while running.
 *
 * What this example does:
 *   - Creates three tasks:
 *        * Low priority task  → prints every 1 second
 *        * Medium priority task → prints every 500 ms
 *        * High priority task → runs 5 times, then lowers its own priority
 *
 * What you will observe in the Serial Monitor:
 *   - High priority task runs most frequently at first.
 *   - Medium priority task runs more often than the low task.
 *   - After 5 iterations, the high task demotes itself to the LOWEST priority.
 *   - Once demoted, medium priority task becomes dominant.
 *
 * This example teaches:
 *   ✔ How task priorities affect scheduling
 *   ✔ How to change a task’s own priority using vTaskPrioritySet()
 *   ✔ How FreeRTOS scheduler chooses which task runs next
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Task handles (not strictly needed here, but useful for future control)
TaskHandle_t low_task_handle  = NULL;
TaskHandle_t med_task_handle  = NULL;
TaskHandle_t high_task_handle = NULL;


/**
 * Low-Priority Task: Prints a message once every 1 second.
 * Runs at priority 1, so it yields to medium and high priority tasks.
 */
void low_priority_task(void *pvParameter)
{
    (void)pvParameter; // not used

    while (1) {
        printf("[LOW ] Running every 1 second\n");
        vTaskDelay(pdMS_TO_TICKS(1000));  // 1 second delay
    }
}


/**
 * Medium-Priority Task
 *
 * Prints a message every 500 ms.
 * Runs at priority 2, so it preempts the low-priority task when ready.
 */
void medium_priority_task(void *pvParameter)
{
    (void)pvParameter;

    while (1) {
        printf("[MED ] Running every 500 ms\n");
        vTaskDelay(pdMS_TO_TICKS(500));  // 0.5 second delay
    }
}


/**
 * High-Priority Task
 *
 * Starts at priority 3 (the highest). It performs 5 iterations,
 * printing every 500 ms. After finishing these iterations,
 * it lowers its own priority to 1 — the lowest level.
 *
 * This shows dynamic priority change during runtime.
 */
void high_priority_task(void *pvParameter)
{
    (void)pvParameter;

    // First phase: run 5 times as the highest-priority task
    for (int i = 1; i <= 5; i++) {
        printf("[HIGH] Iteration %d\n", i);
        vTaskDelay(pdMS_TO_TICKS(500)); // 500 ms delay
    }

    // Change this task's own priority
    printf("[HIGH] Lowering my priority to LOW (1)...\n");
    vTaskPrioritySet(NULL, 1);  // NULL means "this task"

    // Second phase: now runs as a low priority task
    while (1) {
        printf("[HIGH→LOW] I am now running at lower priority...\n");
        vTaskDelay(pdMS_TO_TICKS(2000)); // run less frequently
    }
}


/**
 * app_main()
 *
 * Entry point for the ESP-IDF application.
 * Creates three tasks with different initial priorities:
 *
 *   Priority 3 → HighPriorityTask
 *   Priority 2 → MediumPriorityTask
 *   Priority 1 → LowPriorityTask
 *
 * FreeRTOS automatically schedules tasks based on:
 *   - Priority (higher runs first)
 *   - Whether a task is blocked (e.g., vTaskDelay)
 *   - Ready state
 */
void app_main(void)
{
    printf("=== FreeRTOS Three Task Priority Example Starting ===\n");

    // Lowest priority task (priority 1)
    xTaskCreate(
        low_priority_task,
        "LowPriorityTask",
        2048,
        NULL,
        1,
        &low_task_handle
    );

    // Medium priority task (priority 2)
    xTaskCreate(
        medium_priority_task,
        "MediumPriorityTask",
        2048,
        NULL,
        2,
        &med_task_handle
    );

    // Highest priority task (priority 3)
    xTaskCreate(
        high_priority_task,
        "HighPriorityTask",
        2048,
        NULL,
        3,
        &high_task_handle
    );
}
