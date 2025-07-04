/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "liquidcrystal_i2c.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;

osSemaphoreId_t buttonSemaphoreHandle;
osSemaphoreId_t switchSemaphoreHandle;
osSemaphoreId_t blockSemaphoreHandle;


const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for BTNTask */
osThreadId_t BTNTaskHandle;
const osThreadAttr_t BTNTask_attributes = {
  .name = "BTNTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for LCDTask */
osThreadId_t LCDTaskHandle;
const osThreadAttr_t LCDTask_attributes = {
  .name = "LCDTaskBTN",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for holdLED */
osThreadId_t holdLEDHandle;
const osThreadAttr_t holdLED_attributes = {
  .name = "holdLED",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
osThreadId_t holdSwitchHandle;
const osThreadAttr_t SwitchTask_attributes = {
  .name = "LCDTaskSwitch",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
void StartDefaultTask(void *argument);
void buttonTask(void *argument);
void ledTask(void *argument);
void lcdTaskBTN(void *argument);
void LCDTaskSwitch(void *argument);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
const osSemaphoreAttr_t buttonSemaphore_attributes = {
  .name = "buttonSemaphore"
};

const osSemaphoreAttr_t switchSemaphore_attributes = {
  .name = "switchSemaphore"
};
const osSemaphoreAttr_t blockSemaphore_attributes = {
  .name = "blockSemaphore"
};
//volatile uint8_t button_pressed = 0;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0); // приоритет не выше configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

  /* USER CODE BEGIN 2 */
    HD44780_Init(2);
    HD44780_Clear();
    HD44780_SetCursor(0,0);
 HD44780_PrintStr("BTN don't press");
  HD44780_SetCursor(0,1);
   HD44780_PrintStr("Switch don't press");
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  buttonSemaphoreHandle = osSemaphoreNew(1, 0, &buttonSemaphore_attributes);
  switchSemaphoreHandle = osSemaphoreNew(1, 0, &switchSemaphore_attributes);
  blockSemaphoreHandle = osSemaphoreNew(1, 0, &blockSemaphore_attributes);
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of BTNTask */
 // BTNTaskHandle = osThreadNew(buttonTask, NULL, &BTNTask_attributes);

  /* creation of LCDTask */
//LCDTaskHandle = osThreadNew(ledTask, NULL, &LCDTask_attributes);

  /* creation of holdLED */
  holdLEDHandle = osThreadNew(lcdTaskBTN, NULL, &holdLED_attributes);
  holdSwitchHandle = osThreadNew(LCDTaskSwitch, NULL, &SwitchTask_attributes);
  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */

  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();
  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */
  /* USER CODE END I2C1_Init 2 */
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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PB14 */
    GPIO_InitStruct.Pin = GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the buttonTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if(GPIO_Pin == GPIO_PIN_14) {
        osSemaphoreRelease(buttonSemaphoreHandle); // или xSemaphoreGiveFromISR для чистого FreeRTOS
    }
    if(GPIO_Pin == GPIO_PIN_15) {
        osSemaphoreRelease(switchSemaphoreHandle);
    }
}

void buttonTask(void *argument)
{
    uint8_t last_stateSwitch = GPIO_PIN_RESET; // Исходное состояние - кнопка отпущена
    uint8_t last_stateBTN = GPIO_PIN_RESET;
    //osSemaphoreRelease(switchSemaphoreHandle);
    //osSemaphoreAcquire(switchSemaphoreHandle, portMAX_DELAY);
    for(;;)
    {
        uint8_t current_stateBTN = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14);
        // Устанавливаем курсор на нужную позицию (пример: 13-й столбец, 3-я строка)

        	// Обработка изменения состояния
        		uint8_t current_stateSwitch = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15);

    	        if(current_stateSwitch != last_stateSwitch)
    	        {
    	            if(current_stateSwitch == GPIO_PIN_RESET)
    	            {
    	            	osSemaphoreRelease(switchSemaphoreHandle);
    	             }

    	            last_stateSwitch = current_stateSwitch;
    	        }

    	        osDelay(50);
    	                // Обработка изменения состояния кнопки
    	                if(current_stateBTN != last_stateBTN)
    	                {
    	                    if(current_stateBTN == GPIO_PIN_RESET)
    	                    {
    	                        osSemaphoreRelease(buttonSemaphoreHandle);
    	                    }

    	                    last_stateBTN = current_stateBTN;
    	                }
        osDelay(50); // Задержка для стабильности опроса
    }
}
void LCDTaskSwitch(void *argument)
{
	for(;;)
		{
		    if(osSemaphoreAcquire(switchSemaphoreHandle, portMAX_DELAY) == osOK)
		    {
		          HD44780_ClearRow(1);
		          HD44780_SetCursor(0,1);
		          HD44780_PrintStr("Switch press");
		          osDelay(2000);
		    	   HD44780_SetCursor(0,1);
		    	   HD44780_PrintStr("Switch don't press");
		    	   osSemaphoreAcquire(switchSemaphoreHandle, 0);
		    }
		}
		    osDelay(50);
		}
void ledTask(void *argument)
{
    uint8_t led_on = 0;
    for(;;)
    {
       // Ждем семафор с таймаутом 100 мс
        if(osSemaphoreAcquire(buttonSemaphoreHandle, 100) == osOK)
        {
            // получили сигнал об изменении, читаем актуальное состояние кнопки
        }
        uint8_t button_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15);

        if(button_state == GPIO_PIN_RESET)
        {
            // Кнопка нажата — светодиод горит постоянно
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
            led_on = 1;

            osDelay(5000);
        }
        else
        {
            led_on = !led_on;
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, (led_on) ? GPIO_PIN_RESET : GPIO_PIN_SET);
        }
    }
    osDelay(50);
}

void lcdTaskBTN(void *argument)
{
	for(;;)
	{
	    if(osSemaphoreAcquire(buttonSemaphoreHandle, portMAX_DELAY) == osOK)
	    {
	    	    //osSemaphoreRelease(blockSemaphoreHandle);
	            HD44780_ClearRow(0);
	            HD44780_SetCursor(0,0);
	            HD44780_PrintStr("BTN press");
                osDelay(2000);
                HD44780_SetCursor(0,0);
	            HD44780_PrintStr("BTN don't press");
		    	osSemaphoreAcquire(buttonSemaphoreHandle, 0);
	    }
	    osDelay(50);

	}
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
