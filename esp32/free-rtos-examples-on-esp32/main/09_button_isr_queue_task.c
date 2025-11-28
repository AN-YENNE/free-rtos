/*
 * day9_button_isr_queue.c
 *
 * Demonstrates how to handle a **button press in an interrupt (ISR)**
 * and pass information safely to a FreeRTOS task using a **queue**.
 *
 * Why this pattern is used:
 *   - ISRs must be VERY fast (no printing, no delays, no heavy work)
 *   - Queue sends from ISR are safe and non-blocking
 *   - A background task (normal FreeRTOS task) can take its time to
 *     debounce, log, or perform application-specific logic
 *
 * Hardware setup:
 *   - BUTTON_GPIO (default GPIO0) is pulled up internally
 *   - Button should connect GPIO0 → GND when pressed
 *   - This creates a **falling edge** when pressed, triggering the interrupt
 */

#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define BUTTON_GPIO 0        // BOOT button on many ESP32 DevKit boards
#define TAG "BUTTON_ISR_EX"

// Queue used to send GPIO numbers from ISR → task
static QueueHandle_t button_queue = NULL;


/*
 * Interrupt Service Routine (ISR)
 *
 * This function runs as soon as the button is pressed (falling edge).
 *
 * It:
 *   - Reads the GPIO number from the ISR argument
 *   - Sends that number into a FreeRTOS queue using xQueueSendFromISR()
 *   - Optionally triggers a context switch if a higher priority task was woken
 *
 * NOTE:
 *   - ISRs must be extremely lightweight!
 *   - You must NOT call printf, vTaskDelay, malloc, etc. inside an ISR.
 */
static void IRAM_ATTR button_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    BaseType_t must_yield = pdFALSE;

    // Send the pressed GPIO number to the queue
    xQueueSendFromISR(button_queue, &gpio_num, &must_yield);

    // If sending to the queue unblocked a higher priority task,
    // force a context switch immediately.
    if (must_yield) {
        portYIELD_FROM_ISR();
    }
}


/*
 * Button Task (normal FreeRTOS task)
 *
 * This task waits FOREVER for events from the ISR.
 *
 * Behavior:
 *   - Blocks on xQueueReceive() until the ISR sends a GPIO number.
 *   - When an event arrives, prints a message.
 *   - This is a safe place to implement:
 *        * debouncing
 *        * counting presses
 *        * toggling LEDs
 *        * sending events to other tasks
 */
void button_task(void *pvParameter)
{
    (void) pvParameter;

    uint32_t io_num = 0;

    while (1) {
        // Wait indefinitely for an ISR event
        if (xQueueReceive(button_queue, &io_num, portMAX_DELAY)) {
            ESP_LOGI(TAG, "Button pressed on GPIO %" PRIu32, io_num);

            // Debounce or handle button logic here if needed
            //
            // Example:
            // vTaskDelay(pdMS_TO_TICKS(50)); // debounce delay
        }
    }
}


/*
 * app_main()
 *
 * Entry point for ESP-IDF applications.
 *
 * What happens here:
 *   1. Create a queue to hold button events
 *   2. Configure the button pin as input with pull-up and falling-edge interrupt
 *   3. Install the GPIO ISR service
 *   4. Attach the ISR handler to BUTTON_GPIO
 *   5. Start the button processing task
 */
void app_main(void)
{
    printf("=== Button ISR + Queue Example Starting ===\n");

    // Create a queue that holds up to 10 uint32_t values (GPIO numbers)
    button_queue = xQueueCreate(10, sizeof(uint32_t));
    if (button_queue == NULL) {
        printf("Failed to create queue! Cannot continue.\n");
        return;
    }

    // Configure GPIO for button input
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_NEGEDGE,      // Trigger when button goes LOW
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << BUTTON_GPIO),
        .pull_up_en = GPIO_PULLUP_ENABLE,    // Enable internal pull-up
        .pull_down_en = GPIO_PULLDOWN_DISABLE
    };
    gpio_config(&io_conf);

    // Install ISR service with default configuration
    gpio_install_isr_service(0);

    // Attach our ISR handler to the button pin
    gpio_isr_handler_add(BUTTON_GPIO, button_isr_handler, (void *) BUTTON_GPIO);

    // Create the task that processes button events
    xTaskCreate(
        button_task,
        "ButtonTask",
        2048,
        NULL,
        10,        // High priority to react quickly to button presses
        NULL
    );
}
