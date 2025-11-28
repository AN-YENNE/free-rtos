/*
 *
 * Demonstrates how to:
 *   - Create a FreeRTOS queue
 *   - Send data to the queue from a producer task
 *   - Receive data from the queue in a consumer task
 *   - Monitor how "full" the queue is using uxQueueMessagesWaiting()
 *
 * You will see three types of messages in the Serial Monitor:
 *   - Producer: Sent N           → producer successfully put a value into the queue
 *   - Consumer: Received N       → consumer successfully took a value from the queue
 *   - Monitor: Queue has X ...   → number of items currently stored in the queue
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

// Queue configuration
#define QUEUE_LENGTH     10              // Max number of items the queue can hold
#define QUEUE_ITEM_SIZE  sizeof(int)     // Size of each item in bytes

// Handle to the queue so all tasks can use it
static QueueHandle_t queue = NULL;


/*
 * Producer Task
 *
 * This task generates an increasing integer (0, 1, 2, 3, ...)
 * and sends it to the queue.
 *
 * Behavior:
 *   - Tries to send a value into the queue
 *   - If the queue is full (can't send within 100 ms), prints "Queue full"
 *   - Waits 200 ms between sends to simulate some work
 */
void producer_task(void *pvParameters)
{
    (void) pvParameters;

    int count = 0;

    while (1) {
        // Try to send 'count' to the queue. Wait up to 100 ms if queue is full.
        if (xQueueSend(queue, &count, pdMS_TO_TICKS(100)) == pdPASS) {
            printf("[Producer] Sent %d\n", count);
            count++;
        } else {
            printf("[Producer] Queue full! Could not send.\n");
        }

        // Simulate some workload (producer runs every 200 ms)
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}


/*
 * Consumer Task
 *
 * This task receives integers from the queue.
 *
 * Behavior:
 *   - Waits up to 500 ms to receive an item
 *   - If an item is received, prints the value
 *   - If no item arrives in time, prints "Queue empty"
 *   - Waits 300 ms between receive attempts to simulate slower processing
 */
void consumer_task(void *pvParameters)
{
    (void) pvParameters;

    int value = 0;

    while (1) {
        // Try to receive an item from the queue. Wait up to 500 ms.
        if (xQueueReceive(queue, &value, pdMS_TO_TICKS(500)) == pdPASS) {
            printf("[Consumer] Received %d\n", value);
        } else {
            printf("[Consumer] Queue empty! Nothing to receive.\n");
        }

        // Simulate some processing time (consumer is slower)
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}


/*
 * Monitor Task
 *
 * This task does NOT send or receive data.
 * It only **observes** the queue and reports how many messages are waiting.
 *
 * Behavior:
 *   - Every 1 second:
 *       * Calls uxQueueMessagesWaiting(queue)
 *       * Prints how many items are currently stored in the queue
 *
 * This is useful for:
 *   - Debugging queue usage (is it always full? always empty?)
 *   - Tuning producer/consumer rates
 */
void monitor_task(void *pvParameters)
{
    (void) pvParameters;

    while (1) {
        // Get number of messages currently waiting in the queue
        UBaseType_t waiting = uxQueueMessagesWaiting(queue);

        printf("[Monitor ] Queue has %lu messages waiting\n",
               (unsigned long) waiting);

        vTaskDelay(pdMS_TO_TICKS(1000));  // Check once per second
    }
}


/*
 * app_main()
 *
 * Entry point for ESP-IDF applications.
 *
 * Steps:
 *   1. Create the queue.
 *   2. If creation succeeds, start three tasks:
 *        - Producer (priority 2)
 *        - Consumer (priority 2)
 *        - Monitor (priority 1, lower priority, just observing)
 */
void app_main(void)
{
    printf("=== FreeRTOS Queue Monitor Example Starting ===\n");

    // Create a queue capable of holding 'QUEUE_LENGTH' integers
    queue = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);

    if (queue == NULL) {
        printf("Failed to create queue! Stopping.\n");
        return;  // Cannot continue without a queue
    }

    // Create Producer task (priority 2)
    xTaskCreate(
        producer_task,
        "Producer",
        2048,
        NULL,
        2,
        NULL
    );

    // Create Consumer task (priority 2)
    xTaskCreate(
        consumer_task,
        "Consumer",
        2048,
        NULL,
        2,
        NULL
    );

    // Create Monitor task (priority 1 – lower priority)
    xTaskCreate(
        monitor_task,
        "Monitor",
        2048,
        NULL,
        1,
        NULL
    );
}
