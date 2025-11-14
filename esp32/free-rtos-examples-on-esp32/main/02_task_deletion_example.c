/**
 * @file task_deletion_example.c
 * @brief FreeRTOS example (ESP32): creating tasks, deleting tasks, and self-deletion.
 *
 * This example demonstrates:
 *   1. A task that prints a counter every second and deletes itself after 5 cycles.
 *   2. A control task that waits 3 seconds, then deletes the first task (if still running).
 *
 * Concepts learned:
 *   ✔ How to create tasks
 *   ✔ How to obtain a task handle
 *   ✔ How a task can delete itself
 *   ✔ How one task can delete another task using its handle
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* -------------------------------------------------------------
 * GLOBALS
 * ------------------------------------------------------------- */

/**
 * @brief Handle to the "hello_task".
 *
 * FreeRTOS uses task handles to identify and control specific tasks.
 * We store the handle globally so that the control task can delete it.
 */
TaskHandle_t task_handle_hello = NULL;


/* -------------------------------------------------------------
 * TASK: Hello Task
 * ------------------------------------------------------------- */

/**
 * @brief A task that prints a counter and deletes itself after 5 iterations.
 *
 * Behavior:
 *   - Prints the current counter value every second.
 *   - When the counter reaches 5, the task calls vTaskDelete(NULL),
 *     which tells FreeRTOS to self-terminate.
 *
 * @param pvParameters Optional parameters (unused in this example).
 */
void hello_task(void *pvParameters)
{
    int counter = 0;

    while (1) {
        printf("[Hello Task] Running… counter = %d\n", counter++);

        // Delay this task for 1000 ms (1 second)
        vTaskDelay(pdMS_TO_TICKS(1000));

        // Self-delete after 5 iterations
        if (counter >= 5) {
            printf("[Hello Task] Deleting itself...\n");
            vTaskDelete(NULL);   // Passing NULL deletes *this* task
        }
    }
}


/* -------------------------------------------------------------
 * TASK: Control Task
 * ------------------------------------------------------------- */

/**
 * @brief A task that deletes hello_task after 3 seconds (if it still exists).
 *
 * Behavior:
 *   - Prints a startup message.
 *   - Waits 3 seconds.
 *   - If the hello_task handle is valid, deletes the task.
 *   - Then deletes itself.
 *
 * @param pvParameters Optional parameters (unused).
 */
void control_task(void *pvParameters)
{
    printf("[Control Task] Started.\n");

    // Wait 3 seconds before attempting to delete hello_task
    vTaskDelay(pdMS_TO_TICKS(3000));

    // If hello_task still exists, delete it
    if (task_handle_hello != NULL) {
        printf("[Control Task] Deleting Hello Task...\n");
        vTaskDelete(task_handle_hello);
        task_handle_hello = NULL;  // Not required, but prevents re-use
    }

    printf("[Control Task] Deleting itself...\n");
    vTaskDelete(NULL);  // Self-delete
}


/* -------------------------------------------------------------
 * APP ENTRY POINT
 * ------------------------------------------------------------- */

/**
 * @brief Main application entry point for ESP-IDF.
 *
 * Creates:
 *   - hello_task    (priority 5)
 *   - control_task  (priority 4)
 *
 * Notes:
 *   - hello_task may delete itself after 5 iterations.
 *   - control_task may delete hello_task after 3 seconds.
 *   - Depending on timing, either task may end hello_task first.
 */
void app_main()
{
    printf("=== FreeRTOS Task Deletion Example Starting ===\n");

    /* Create Hello Task */
    xTaskCreate(
        hello_task,             // Function implementing the task
        "Hello Task",           // Name (useful for debugging)
        2048,                   // Stack size in words (approx. 8 KB)
        NULL,                   // Task parameters
        5,                      // Priority (higher = more important)
        &task_handle_hello      // Store the task handle here
    );

    /* Create Control Task */
    xTaskCreate(
        control_task,           // Function implementing the task
        "Control Task",         // Task name
        2048,                   // Stack size
        NULL,                   // No parameters
        4,                      // Priority lower than hello_task
        NULL                    // We don't need its handle
    );
}
