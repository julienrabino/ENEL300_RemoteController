/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define CONFIGURE_HC05 0 // 1 = AT mode, 0 = Data mode

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_ADC1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void configureHC05(){
	char resp[50] = {0};

	    // 1. Send AT
	    HAL_UART_Transmit(&huart1, (uint8_t*)"AT\r\n", 4, 1000);

	    // 2. Receive exactly 4 bytes (OK\r\n)
	    HAL_StatusTypeDef status = HAL_UART_Receive(&huart1, (uint8_t*)resp, 4, 1000);

	    if (status == HAL_OK) {
	        printf("HC-05 says: %s\r\n", resp);
	        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET); // LED ON
	    } else if (status == HAL_TIMEOUT) {
	        printf("ERROR: Timeout - No response from module\r\n");
	    } else {
	        printf("ERROR: UART Error Code %d\r\n", status);
	    }


	// set controller as "master"
	char cmd1[] = "AT+ROLE=1\r\n";
	HAL_UART_Transmit(&huart1, (uint8_t*)cmd1, strlen(cmd1), HAL_MAX_DELAY);
	HAL_UART_Receive(&huart1, (uint8_t*)resp, 4, 200);
	printf("Role Response: %s\r\n", resp); // Print the "OK"
	HAL_Delay(500);
	memset(resp, 0, sizeof(resp));

	char cmd_pw[] = "AT+PSWD=\"1234\"\r\n";
	HAL_UART_Transmit(&huart1, (uint8_t*)cmd_pw, strlen(cmd_pw), HAL_MAX_DELAY);
	HAL_UART_Receive(&huart1, (uint8_t*)resp, sizeof(resp), 200);
	printf("Password Response: %s\r\n", resp);
	HAL_Delay(500);
	memset(resp, 0, sizeof(resp));

#if 0
	char cmd2[] = "AT+BIND=2025,08,004A3B\r\n";
	HAL_UART_Transmit(&huart1, (uint8_t*)cmd2, strlen(cmd2), HAL_MAX_DELAY);
	HAL_UART_Receive(&huart1, (uint8_t*)resp, sizeof(resp), 200);
	HAL_Delay(500);
#endif
	char cmd3[] = "AT+CMODE=1\r\n";
	HAL_UART_Transmit(&huart1, (uint8_t*)cmd3, strlen(cmd3), HAL_MAX_DELAY);
	HAL_UART_Receive(&huart1, (uint8_t*)resp, sizeof(resp), 200);
	printf("CMode Response: %s\r\n", resp); // Print the "OK"

	HAL_Delay(500);

	memset(resp, 0, sizeof(resp));

	// set data mode baud rate = 9600
	char cmd4[] = "AT+UART=9600,0,0\r\n";
	HAL_UART_Transmit(&huart1, (uint8_t*)cmd4, strlen(cmd4), HAL_MAX_DELAY);
	HAL_UART_Receive(&huart1, (uint8_t*)resp, sizeof(resp), 200);
	HAL_Delay(500);

}


uint32_t abs(int n) {return ((n < 0) ? -n : n);}


void convertInput(uint32_t x_in, uint32_t y_in, uint8_t* cmd) {

    int x = x_in - 3652 + 600 - 100;
    int y = -y_in + 2532 + 600 + 100;

    if (abs(x) < (5)) x = 0; // the 16 accounts for shifting, 5 is the deadzone we see in terminal
    if (abs(y) < (5)) y = 0;

    int left  = x + y;
    int right = y - x;

    if (left >= 4096) (left = 4096);
    if (right >= 4096) (right = 4096);
    if (left <= -4096) (left = -4096);
    if (right <= -4096) (right = -4096);


    cmd[0] = (left > 0) ? 1 : 0;
    cmd[1] = cmd[0] == 1 ? (uint8_t)((abs(left) >> 4)) : (uint8_t)(255 - (abs(left) >> 4));

    cmd[2] = (right > 0) ? 1 : 0;
    cmd[3] = cmd[2] == 1 ? (uint8_t)((abs(right) >> 4)) : (uint8_t)(255 - (abs(right) >> 4));

}
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
  MX_USART2_UART_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
# if CONFIGURE_HC05
  HAL_GPIO_WritePin(HC05_EN_GPIO_Port, HC05_EN_Pin, GPIO_PIN_SET);  // set EN HIGH
  configureHC05();

	// important: after running this ONCE, change CONFIGURE_HC05 to 0 and also the baud rate to 9600.
  while (1){} // stop execution so normal controller code doesn't run
#endif
  HAL_GPIO_WritePin(HC05_EN_GPIO_Port, HC05_EN_Pin, GPIO_PIN_RESET);  // set EN LOW
# if 0
  uint8_t cmd[4] = {0,100,0,100};
  HAL_UART_Transmit(&huart1, cmd, 4, HAL_MAX_DELAY);
  printf("cmd array: ");
  for (int i = 0; i < 4; i++) {
      printf("%02X ", cmd[i]);  // %02X prints two-digit hex with leading zero
  }
  printf("\r\n");
  HAL_Delay(100);
#endif
#if 1

	  uint32_t adc_x;
	  uint32_t adc_y;
	  HAL_ADC_Start(&hadc1);

	  HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
	  adc_x = HAL_ADC_GetValue(&hadc1);

	  HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
	  adc_y = HAL_ADC_GetValue(&hadc1);

	  HAL_ADC_Stop(&hadc1);

	  //printf("X:%lu Y:%lu\r\n", adc_val1, adc_val2);

	  uint8_t cmd[4];
	  convertInput(adc_x, adc_y, cmd);
	 // printf("Raw X: %lu, Raw Y: %lu\r\n", adc_x, adc_y);
#if 1
	    for (int i = 0; i < 4; i++) {
	        printf("%d ", cmd[i]);  // %02X prints two-digit hex with leading zero
	    }
	    printf("\r\n");
	    for(int i = 0; i < 4; i++){
	        HAL_UART_Transmit(&huart1, &cmd[i], 1, HAL_MAX_DELAY);
	        HAL_Delay(1); // 1 ms delay
	    }
	  HAL_Delay(100);
#endif
#endif

#if 0
	  uint8_t msg[] = "Hello Car!\r\n";
	  HAL_UART_Transmit(&huart1, msg, strlen((char*)msg), 10);
	  HAL_Delay(1000);
#endif
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

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
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
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
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 2;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_84CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */
  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  huart2.Init.BaudRate = 38400;
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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(HC05_EN_GPIO_Port, HC05_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : HC05_EN_Pin */
  GPIO_InitStruct.Pin = HC05_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(HC05_EN_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
int __io_putchar(int ch)
{
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
  return ch;
}
/* USER CODE END 4 */

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
