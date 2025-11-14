/*=============================
 *  FreeRTOS Task Creation Demo
 *  STM32 + FreeRTOS + UART
 *=============================*/

#include <stdio.h>          // For printf()
#include "main.h"
#include "cmsis_os.h"       // CMSIS-RTOS wrapper layer

/* TODO 1: Include FreeRTOS kernel headers */
#include "FreeRTOS.h"       // Core FreeRTOS definitions
#include "task.h"           // Task-related APIs

/* UART handle for serial communication (USART2) */
UART_HandleTypeDef huart2;

/* Function Prototypes */
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
void StartDefaultTask(void const * argument); // Unused here

/*==============================
 *  Retarget printf to UART
 *==============================*/
int __io_putchar(int ch)
{
    HAL_UART_Transmit(&huart2, (uint8_t* )&ch, 1, 100); // Send one byte over UART
    return ch;
}

/*===================================
 * TODO 2: Declare task profiler variables
 * These are used to count how many times
 * each task runs (useful for debugging).
 *===================================*/
typedef uint32_t TaskProfiler;
TaskProfiler task1Profiler = 0;
TaskProfiler task2Profiler = 0;

/* TODO 5: Declare task functions */
void Task1(void *pvParameter);  // Task1 function prototype
void Task2(void *pvParameter);  // Task2 function prototype

/*==============================
 *           main()
 *==============================
 * Initializes system and creates two tasks.
 */
int main(void)
{
    HAL_Init();                // Initialize HAL (low-level drivers)
    SystemClock_Config();      // Configure system clock
    MX_GPIO_Init();            // Initialize GPIOs
    MX_USART2_UART_Init();     // Initialize UART2 (for printf)

    /* TODO 6: Create Task1 */
    // Parameters:
    // - Function pointer (Task1)
    // - Name (for debugging)
    // - Stack size in words (not bytes!)
    // - Task parameter (NULL here)
    // - Priority (1)
    // - Task handle (not used here, so NULL)
    xTaskCreate(Task1, "Task1", 100, NULL, 1, NULL);

    /* TODO 7: Create Task2 */
    xTaskCreate(Task2, "Task2", 100, NULL, 1, NULL);

    /* TODO 8: Start the FreeRTOS scheduler */
    // From this point, FreeRTOS takes control and starts running tasks.
    vTaskStartScheduler();

    // Code should never reach here unless there's insufficient RAM for the scheduler.
    while (1)
    {
    }
}

/*===========================
 * TODO 3: Task1 Function
 *===========================
 * A simple FreeRTOS task that runs forever,
 * incrementing a counter to simulate work.
 */
void Task1(void *pvParameter)
{
    while(1)
    {
        task1Profiler++;    // Simulate task work (increment counter)
        // Optional: add delay to simulate real-time task
        // vTaskDelay(1);
    }
}

/*===========================
 * TODO 4: Task2 Function
 *===========================
 * Same as Task1 but for Task2.
 */
void Task2(void *pvParameter)
{
    while(1)
    {
        task2Profiler++;   // Simulate task work
        // vTaskDelay(1);
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
