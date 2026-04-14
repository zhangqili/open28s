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
#include "keyboard.h"
#include "analog.h"
#include "usbd_user.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ADC_BUFFER_LENGTH 9
#define ADC_CHANNEL_NUM 9
#define DMA_LENGTH (ADC_BUFFER_LENGTH*ADC_CHANNEL_NUM)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

I2C_HandleTypeDef hi2c1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void HAL_PCD_MspInit(PCD_HandleTypeDef *hpcd)
{
  __HAL_RCC_USB_CLK_ENABLE();
  HAL_NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
}

uint32_t adc_buffer[2][ADC_BUFFER_LENGTH*ADC_CHANNEL_NUM];
uint32_t debug;
uint32_t debug1;
uint32_t next_tick;

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
  HAL_GPIO_WritePin(MUX_GPIO_Port, MUX_Pin, GPIO_PIN_SET);
  uint32_t adc_buf[ADC_CHANNEL_NUM] = {0};
  for (int i = 1; i < ADC_BUFFER_LENGTH; i++)
  {
    adc_buf[0] += adc_buffer[0][i*ADC_CHANNEL_NUM+0];
    adc_buf[1] += adc_buffer[0][i*ADC_CHANNEL_NUM+1];
    adc_buf[2] += adc_buffer[0][i*ADC_CHANNEL_NUM+2];
    adc_buf[3] += adc_buffer[0][i*ADC_CHANNEL_NUM+3];
    adc_buf[4] += adc_buffer[0][i*ADC_CHANNEL_NUM+4];
    adc_buf[5] += adc_buffer[0][i*ADC_CHANNEL_NUM+5];
    adc_buf[6] += adc_buffer[0][i*ADC_CHANNEL_NUM+6];
    adc_buf[7] += adc_buffer[0][i*ADC_CHANNEL_NUM+7];
    adc_buf[8] += adc_buffer[0][i*ADC_CHANNEL_NUM+8];
  }
  ringbuf_push(&g_adc_ringbufs[0], adc_buf[0]/8);
  ringbuf_push(&g_adc_ringbufs[1], adc_buf[1]/8);
  ringbuf_push(&g_adc_ringbufs[2], adc_buf[2]/8);
  ringbuf_push(&g_adc_ringbufs[3], adc_buf[3]/8);
  ringbuf_push(&g_adc_ringbufs[4], adc_buf[4]/8);
  ringbuf_push(&g_adc_ringbufs[5], adc_buf[5]/8);
  ringbuf_push(&g_adc_ringbufs[6], adc_buf[6]/8);
  ringbuf_push(&g_adc_ringbufs[7], adc_buf[7]/8);
  ringbuf_push(&g_adc_ringbufs[8], adc_buf[8]/8);
  
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
  HAL_GPIO_WritePin(MUX_GPIO_Port, MUX_Pin, GPIO_PIN_RESET);
  uint32_t adc_buf[ADC_CHANNEL_NUM] = {0};
  for (int i = 1; i < ADC_BUFFER_LENGTH; i++)
  {
    adc_buf[0] += adc_buffer[1][i*ADC_CHANNEL_NUM+0];
    adc_buf[1] += adc_buffer[1][i*ADC_CHANNEL_NUM+1];
    adc_buf[2] += adc_buffer[1][i*ADC_CHANNEL_NUM+2];
    adc_buf[3] += adc_buffer[1][i*ADC_CHANNEL_NUM+3];
    adc_buf[4] += adc_buffer[1][i*ADC_CHANNEL_NUM+4];
    adc_buf[5] += adc_buffer[1][i*ADC_CHANNEL_NUM+5];
    adc_buf[6] += adc_buffer[1][i*ADC_CHANNEL_NUM+6];
    adc_buf[7] += adc_buffer[1][i*ADC_CHANNEL_NUM+7];
    adc_buf[8] += adc_buffer[1][i*ADC_CHANNEL_NUM+8];
  }
  ringbuf_push(&g_adc_ringbufs[9+0], adc_buf[0]/8);
  ringbuf_push(&g_adc_ringbufs[9+1], adc_buf[1]/8);
  //ringbuf_push(&g_adc_ringbufs[9+2], adc_buf[2]/8);
  ringbuf_push(&g_adc_ringbufs[9+2], adc_buf[3]/8);
  ringbuf_push(&g_adc_ringbufs[9+3], adc_buf[4]/8);
  ringbuf_push(&g_adc_ringbufs[9+4], adc_buf[5]/8);
  ringbuf_push(&g_adc_ringbufs[9+5], adc_buf[6]/8);
  ringbuf_push(&g_adc_ringbufs[9+6], adc_buf[7]/8);
  ringbuf_push(&g_adc_ringbufs[9+7], adc_buf[8]/8);
}
uint16_t keys;
//AnalogRawValue advanced_key_read_raw(AdvancedKey *advanced_key)
//{
//  if (advanced_key->key.id == 0)
//  {
//    return keys;
//  }
//  
//    return ringbuf_avg(&g_adc_ringbufs[g_analog_map[advanced_key->key.id]]);
//}
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
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  HAL_ADCEx_Calibration_Start(&hadc1);
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, DMA_LENGTH*2);
  //HAL_GPIO_WritePin(KEY_BUS_GPIO_Port, KEY_BUS_Pin, GPIO_PIN_SET);
  keyboard_init();
  usb_init(0, USB_BASE);
  analog_calibrate();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  { 
    uint16_t temp_keys = 0;
    temp_keys |= HAL_GPIO_ReadPin(KEY_1_GPIO_Port, KEY_1_Pin) ? BIT(0) : 0;
    temp_keys |= HAL_GPIO_ReadPin(KEY_2_GPIO_Port, KEY_2_Pin) ? BIT(1) : 0;
    temp_keys |= HAL_GPIO_ReadPin(KEY_3_GPIO_Port, KEY_3_Pin) ? BIT(2) : 0;
    temp_keys |= HAL_GPIO_ReadPin(KEY_4_GPIO_Port, KEY_4_Pin) ? BIT(3) : 0;
    keys = temp_keys;
    keyboard_task();
    keyboard_process();
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC|RCC_PERIPHCLK_USB;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
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

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 9;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = ADC_REGULAR_RANK_3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = ADC_REGULAR_RANK_4;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = ADC_REGULAR_RANK_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = ADC_REGULAR_RANK_6;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_7;
  sConfig.Rank = ADC_REGULAR_RANK_7;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_8;
  sConfig.Rank = ADC_REGULAR_RANK_8;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_9;
  sConfig.Rank = ADC_REGULAR_RANK_9;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

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
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

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
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, MUX_Pin|KEY_1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB10 KEY_DOWN_Pin KEY_5_Pin KEY_LEFT_Pin
                           KEY_3_Pin KEY_4_Pin KEY_RIGHT_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_10|KEY_DOWN_Pin|KEY_5_Pin|KEY_LEFT_Pin
                          |KEY_3_Pin|KEY_4_Pin|KEY_RIGHT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : MUX_Pin KEY_1_Pin */
  GPIO_InitStruct.Pin = MUX_Pin|KEY_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : KEY_UP_Pin KEY_2_Pin */
  GPIO_InitStruct.Pin = KEY_UP_Pin|KEY_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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
#ifdef USE_FULL_ASSERT
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
