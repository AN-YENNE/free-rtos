/*===========================
 *    Basic FreeRTOS Demo
 *    STM32 Nucleo (F Series)
 *===========================*/

/* TODO 1: Include stdio.h so we can use printf() function*/
#include <stdio.h>

#include "main.h"
#include "cmsis_os.h"  // CMSIS-RTOS API header

/* Global UART handle for USART2 (used for serial communication) */
UART_HandleTypeDef huart2;

/* Function Prototypes */
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
void StartDefaultTask(void const * argument);

/*===========================================
 * Retarget printf() to UART using __io_putchar
 * This lets you use printf() to send messages
 * over UART to your PC serial monitor.
 *===========================================*/
int __io_putchar(int ch)
{
    HAL_UART_Transmit(&huart2 /*UART handle address*/, (uint8_t *)&ch/*Char to write*/, 1/*no.of chars to write*/, 100/*time to try before timeout*/);  // Send one character over UART
    return ch; //on success, returns char
}

/*==============================
 *           main()
 *==============================
 * Program entry point.
 * Initializes hardware, UART, and GPIOs.
 * Prints messages over UART using printf().
 */
int main(void)
{
    HAL_Init();               // Initialize the Hardware Abstraction Layer (HAL)
    SystemClock_Config();     // Configure system clock (72 MHz, for example)
    MX_GPIO_Init();           // Initialize GPIO (for LED or buttons)
    MX_USART2_UART_Init();    // Initialize USART2 for UART communication

    /* Infinite loop */
    while (1)
    {
        /* TODO 3: Test printf retargeting */
        printf("Hello from STM32F411RE\n\r");  // Send string over UART
        HAL_Delay(500);                 // Wait 500 ms
    }
}

/*==============================
 *  System Clock Configuration
 *==============================
 * Configures PLL and bus clocks.
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /* Enable power controller clock and configure voltage scaling */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /* Configure the internal high-speed oscillator (HSI) and PLL */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 16;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
    RCC_OscInitStruct.PLL.PLLQ = 4;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();  // Handle configuration error
    }

    /* Set up AHB, APB1, and APB2 clocks */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();  // Handle clock error
    }
}

/*====================================
 *  USART2 UART Initialization
 *====================================
 * Configures USART2 for 115200 baud,
 * 8 data bits, no parity, 1 stop bit.
 */
static void MX_USART2_UART_Init(void)
{
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        Error_Handler();  // UART init failed
    }
}

/*===========================
 *      GPIO Initialization
 *===========================
 * Enables GPIO clocks and sets
 * LED pin as output.
 */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Enable GPIO clocks */
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* Set LED_PIN low by default */
    HAL_GPIO_WritePin(LED_PIN_GPIO_Port, LED_PIN_Pin, GPIO_PIN_RESET);

    /* Configure LED pin as push-pull output */
    GPIO_InitStruct.Pin = LED_PIN_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_PIN_GPIO_Port, &GPIO_InitStruct);
}

/*==========================================
 *  FreeRTOS Default Task Function Stub
 *==========================================
 * This will be used once FreeRTOS is enabled
 */
void StartDefaultTask(void const * argument)
{
    for(;;)
    {
        osDelay(1);  // FreeRTOS delay (1 ms)
    }
}

/*===============================
 *      Error Handler
 *===============================
 * Called on HAL error.
 * Disables interrupts and loops forever.
 */
void Error_Handler(void)
{
    __disable_irq();  // Disable all interrupts
    while (1)
    {
        // Optional: Blink LED to indicate error
    }
}

/*==================================
 *   assert_failed (Optional Debug)
 *==================================
 * Called when an assert fails
 */
#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
    /* User can log the error here */
    // Example: printf("Assert failed in file %s at line %d\n", file, line);
}
#endif
