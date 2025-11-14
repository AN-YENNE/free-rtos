/**
 * @file task_core_affinity.c
 * @brief example demonstrating FreeRTOS task core affinity on ESP32.
 *
 * ESP32 has **2 CPU cores**:
 *   - Core 0 (PRO CPU)
 *   - Core 1 (APP CPU)
 *
 * In FreeRTOS on ESP32:
 *  - You can let a task run on **any available core** (default behavior)
 *  - Or you can **pin a task** to a specific core using xTaskCreatePinnedToCore()
 *
 * This example shows both approaches:
 *   1. task_unpinned      → can run on Core 0 *or* Core 1
 *   2. task_pinned_core1  → always runs on Core 1
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/**
 * @brief Task that runs on ANY core (scheduler decides).
 *
 * NOTES:
 *  - This task is created with xTaskCreate() → NOT pinned to any CPU core.
 *  - The ESP32 scheduler may move this task between cores based on load.
 *  - xPortGetCoreID() returns the ID of the core the task is currently running on.
 */
void task_unpinned(void *pvParameters)
{
    while (1) {
        printf("[Unpinned] Running on Core %d\n", xPortGetCoreID());

        // Delay the task for 1000 ms (1 second)
        // vTaskDelay() gives CPU time to other tasks.
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/**
 * @brief Task that is pinned to CORE 1 only.
 *
 * NOTES:
 *  - Created with xTaskCreatePinnedToCore()
 *    → The task ALWAYS runs on the core we specify (Core 1 here).
 *  - Pinning tasks is useful when:
 *      * Using peripherals restricted to a core
 *      * Ensuring timing consistency
 *      * Reducing context switch jitter
 *  - ESP32 Core numbers:
 *      * Core 0 = PRO CPU
 *      * Core 1 = APP CPU
 */
void task_pinned_core1(void *pvParameters)
{
    while (1) {
        printf("[Pinned]   Running on Core %d\n", xPortGetCoreID());
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/**
 * @brief Application entry point (called automatically by ESP-IDF).
 *
 * Creates two tasks:
 *   1. task_unpinned → scheduled on ANY core (0 or 1)
 *   2. task_pinned_core1 → forced to run on Core 1
 */
void app_main()
{
    printf("FreeRTOS Core Affinity Example Starting...\n");

    /**
     * Create a task with NO core affinity (free to run on any core).
     *
     * xTaskCreate(
     *      task function,
     *      task name (for debugging),
     *      stack size in BYTES,
     *      task parameters (none),
     *      priority (higher number = higher priority),
     *      task handle (not needed here)
     * );
     */
    xTaskCreate(
        task_unpinned,
        "Task_Unpinned",
        2048,
        NULL,
        5,
        NULL
    );

    /**
     * Create a task PINNED TO CORE 1.
     *
     * xTaskCreatePinnedToCore(
     *      task function,
     *      name,
     *      stack size,
     *      params,
     *      priority,
     *      handle,
     *      CORE ID (0 or 1)
     * );
     */
    xTaskCreatePinnedToCore(
        task_pinned_core1,
        "Task_Core1",
        2048,
        NULL,
        5,
        NULL,
        1        // <-- Core 1
    );
}
