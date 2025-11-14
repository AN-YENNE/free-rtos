/*============================================================================
 * STM32 + FreeRTOS: Resume Task Example
 * Demonstrates how to suspend a task and later resume it using thresholds.
 *============================================================================*/

#include <stdio.h>
#include "main.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"

/*----------------------------------------------------------
 * Threshold at which Task2 will be suspended by Task1
 *----------------------------------------------------------*/
#define SUSPEND_THRESH   250

/* TODO 1: Define threshold at which Task2 will be resumed */
#define RESUME_THRESH    250

/* UART handler for printf() output via UART2 */
UART_HandleTypeDef huart2;

/* Initialization functions */
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
void StartDefaultTask(void const * argument);  // Not used here

/*----------------------------------------------------------
 * printf redirection to UART
 * Allows using printf() for debugging in serial monitor
 *----------------------------------------------------------*/
int __io_putchar(int ch)
{
    HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 100);
    return ch;
}

/*----------------------------------------------------------
 * Task Profilers – count how many times tasks execute
 * Helps in observing runtime behavior
 *----------------------------------------------------------*/
typedef uint32_t TaskProfiler;
TaskProfiler task1Profiler = 0;
TaskProfiler task2Profiler = 0;

/* Runtime monitor variables */
uint32_t suspend_monitor = 0;

/* TODO 2: Monitor how long task has been suspended */
uint32_t resume_monitor = 0;

/* TODO 3: Suspension flag – indicates task2 is currently suspended */
uint8_t SUSPEND_FLG = 0;

/* Task Function Prototypes */
void Task1(void *pvParameter);
void Task2(void *pvParameter);

/* Task Handles */
TaskHandle_t task1_handle, task2_handle;

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART2_UART_Init();

    /* Create Task1 and Task2 with same priority (1) */
    xTaskCreate(Task1, "Task1", 100, NULL, 1, &task1_handle);
    xTaskCreate(Task2, "Task2", 100, NULL, 1, &task2_handle);

    /* Start the RTOS scheduler */
    vTaskStartScheduler();

    /* Should never reach here */
    while (1)
    {
    }
}

/*----------------------------------------------------------
 * Task1:
 * - Does dummy work (simulate load)
 * - Suspends Task2 after threshold
 * - Resumes Task2 after resume threshold
 *----------------------------------------------------------*/
void Task1(void *pvParameter)
{
    while (1)
    {
        // Simulate work (delay via busy loop)
        for (volatile int i = 1; i <= 100000; i++) {}

        // Increment suspend monitor each loop
        suspend_monitor++;

        // Suspend Task2 after reaching the threshold
        if (suspend_monitor >= SUSPEND_THRESH && SUSPEND_FLG == 0)
        {
            vTaskSuspend(task2_handle);
            printf("Task2 suspended!\r\n");

            // TODO 4: Set flag so we don’t suspend again until resumed
            SUSPEND_FLG = 1;
        }

        // TODO 5: If Task2 is suspended, check when to resume it
        if (SUSPEND_FLG == 1)
        {
            resume_monitor++;  // Count time since suspension

            if (resume_monitor > RESUME_THRESH)
            {
                vTaskResume(task2_handle);
                printf("Task2 resumed!\r\n");

                // Reset monitors and flag to allow re-suspension if needed
                SUSPEND_FLG = 0;
                suspend_monitor = 0;
                resume_monitor = 0;
            }
        }

        task1Profiler++;
    }
}

/*----------------------------------------------------------
 * Task2:
 * - Just counts how many times it runs (until suspended)
 * - Will stop when suspended, resume later
 *----------------------------------------------------------*/
void Task2(void *pvParameter)
{
    while (1)
    {
        task2Profiler++;

        // Optional: Uncomment to observe task output
        // printf("Task2 running: %lu\r\n", task2Profiler);
    }
}


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
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
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
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
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_PIN_GPIO_Port, LED_PIN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_PIN_Pin */
  GPIO_InitStruct.Pin = LED_PIN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_PIN_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
