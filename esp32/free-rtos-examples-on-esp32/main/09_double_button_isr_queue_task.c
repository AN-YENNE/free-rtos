/*
 * dual_button_isr_queue.c
 *
 * Demonstrates how to handle **two button GPIO interrupts** using a **single ISR**
 * and a **FreeRTOS queue** to pass events from ISR → task.
 *
 * Pattern used:
 *   - Keep ISR extremely short (only queue send)
 *   - Do all actual work (printing, debounce, logic) inside a FreeRTOS task
 *
 * What this example does:
 *   - Sets up two buttons on different GPIO pins
 *   - Both buttons share the SAME ISR
 *   - ISR sends the GPIO number to a queue
 *   - A FreeRTOS task waits on the queue and prints which button was pressed
 *   - Includes simple per-button software debounce
 *
 * Hardware expectation (typical ESP32 boards):
 *   - BUTTON_GPIO1 = GPIO0 (BOOT button)
 *   - BUTTON_GPIO2 = GPIO4 (aux button)
 *   - Buttons are active-low → pressed = connected to GND
 *   - Internal pull-ups enabled
 */

#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define TAG            "BUTTONS"

// Two button pins
#define BUTTON_GPIO1   0      // BOOT button
#define BUTTON_GPIO2   4      // Auxiliary button

// Debounce time (30 ms worth of ticks)
#define DEBOUNCE_TICKS pdMS_TO_TICKS(30)

// Queue handle used by ISR and task
static QueueHandle_t button_queue = NULL;


/*
 * ISR: Runs instantly when a button is pressed (falling edge).
 *
 * - Extremely fast
 * - Sends the GPIO number to the queue
 * - Optional yield if a high priority task is woken
 *
 * NOTE:
 *   You MUST NOT:
 *     - printf
 *     - delay
 *     - allocate memory
 *   inside an ISR.
 *
 *   Only xQueueSendFromISR() and similar “ISR-safe” functions are allowed.
 */
static void IRAM_ATTR button_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    BaseType_t higher_prio_woken = pdFALSE;

    // Send GPIO number to queue safely from ISR
    xQueueSendFromISR(button_queue, &gpio_num, &higher_prio_woken);

    // Yield immediately if needed
    if (higher_prio_woken) {
        portYIELD_FROM_ISR();
    }
}


/*
 * FreeRTOS task that processes button press events.
 *
 * - Blocks indefinitely waiting for queue messages
 * - When ISR sends a GPIO number, task wakes up
 * - Applies simple per-button debounce
 * - Prints which button was pressed
 *
 * Debouncing here is intentional — keep ISR clean and fast.
 */
static void button_task(void *pvParameter)
{
    (void)pvParameter;

    uint32_t io_num = 0;

    // Track last accepted event time for each button
    TickType_t last_tick_btn1 = 0;
    TickType_t last_tick_btn2 = 0;

    while (1) {
        // Wait forever for a message from ISR
        if (xQueueReceive(button_queue, &io_num, portMAX_DELAY)) {

            TickType_t now = xTaskGetTickCount();
            bool accepted = true;

            // Simple per-button debounce filter
            if (io_num == BUTTON_GPIO1) {
                if ((now - last_tick_btn1) < DEBOUNCE_TICKS) {
                    accepted = false;
                } else {
                    last_tick_btn1 = now;
                }
            }
            else if (io_num == BUTTON_GPIO2) {
                if ((now - last_tick_btn2) < DEBOUNCE_TICKS) {
                    accepted = false;
                } else {
                    last_tick_btn2 = now;
                }
            }

            // Ignore bounces
            if (!accepted) continue;

            // Print which button was pressed
            switch (io_num) {
                case BUTTON_GPIO1:
                    ESP_LOGI(TAG, "BUTTON 1 pressed on GPIO %" PRIu32 " (BOOT)", io_num);
                    break;

                case BUTTON_GPIO2:
                    ESP_LOGI(TAG, "BUTTON 2 pressed on GPIO %" PRIu32 " (AUX)", io_num);
                    break;

                default:
                    ESP_LOGW(TAG, "Unexpected GPIO event on %" PRIu32, io_num);
                    break;
            }
        }
    }
}


/*
 * app_main()
 *
 * Sets up:
 *   1. Queue to hold GPIO numbers
 *   2. GPIO input mode with pull-ups and falling-edge interrupts
 *   3. ISR service and attaches the SAME ISR to both buttons
 *   4. Task to process button events
 */
void app_main(void)
{
    printf("=== Dual Button ISR + Queue Example Starting ===\n");

    // Create queue that can buffer up to 10 button events
    button_queue = xQueueCreate(10, sizeof(uint32_t));
    if (button_queue == NULL) {
        printf("Queue creation failed! Cannot continue.\n");
        return;
    }

    // Configure both buttons at once using a bitmask
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_NEGEDGE,   // falling edge = button press
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << BUTTON_GPIO1) | (1ULL << BUTTON_GPIO2),
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE
    };
    gpio_config(&io_conf);

    // Install ISR driver
    gpio_install_isr_service(0);

    // Register ISR handler for BOTH buttons
    gpio_isr_handler_add(BUTTON_GPIO1, button_isr_handler, (void *)BUTTON_GPIO1);
    gpio_isr_handler_add(BUTTON_GPIO2, button_isr_handler, (void *)BUTTON_GPIO2);

    // Start the FreeRTOS button task
    xTaskCreate(
        button_task,
        "ButtonTask",
        2048,
        NULL,
        10,
        NULL
    );

    ESP_LOGI(TAG,
             "Setup complete. Listening on GPIO %" PRIu32 " and GPIO %" PRIu32 ".",
             (uint32_t)BUTTON_GPIO1, (uint32_t)BUTTON_GPIO2);
}
