/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.c
  * @brief   This file provides code for the configuration
  *          of the TIM instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "tim.h"

/* USER CODE BEGIN 0 */
void delayMicro(uint16_t _us)
{
	__HAL_TIM_SET_COUNTER(&htim21, 0);
	while (__HAL_TIM_GET_COUNTER(&htim21) < _us);
}

/*LCD Functions*/
void LCD_Send(char _data, int _rs)
{
	HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, _rs);

	HAL_GPIO_WritePin(LCD_D7_GPIO_Port, LCD_D7_Pin, (_data >> 3) & 0x01);
	HAL_GPIO_WritePin(LCD_D6_GPIO_Port, LCD_D6_Pin, (_data >> 2) & 0x01);
	HAL_GPIO_WritePin(LCD_D5_GPIO_Port, LCD_D5_Pin, (_data >> 1) & 0x01);
	HAL_GPIO_WritePin(LCD_D4_GPIO_Port, LCD_D4_Pin, (_data >> 0) & 0x01);

	HAL_GPIO_WritePin(LCD_EN_GPIO_Port, LCD_EN_Pin, GPIO_PIN_SET);
	delayMicro(20);
	HAL_GPIO_WritePin(LCD_EN_GPIO_Port, LCD_EN_Pin, GPIO_PIN_RESET);
	delayMicro(20);
}
void LCD_SendCMD(char _cmd)
{
	LCD_Send((_cmd >> 4) & 0x0f, 0);
	LCD_Send((_cmd) & 0x0f, 0);
}

void LCD_SendData(char _data)
{
	LCD_Send((_data >> 4) & 0x0f, 1);
	LCD_Send((_data) & 0x0f, 1);
}

void LCD_Init(void)
{
	HAL_Delay(80);
	LCD_SendCMD(0x02);
	LCD_SendCMD(0x28);
	delayMicro(40);
	LCD_SendCMD(0x0E);
	delayMicro(40);
	LCD_SendCMD(0x01);
	HAL_Delay(2);
	LCD_SendCMD(0x06);
}

void LCD_Clear(void)
{
	LCD_SendCMD(0x01);
	HAL_Delay(2);
}

void LCD_GotoXY(char _row, char _col)
{
	LCD_SendCMD(0x80 + _col + (0x40 * _row));
}

void LCD_SendStr(char* _str)
{
	while(*_str) LCD_SendData(*_str++);
}

void HandleLCD(char* _currRow1, char* _prevRow1, char* _currRow2, char* _prevRow2);

/*WS2812B Functions*/
int datasentflag = 0;
int ledData[MAX_LED][5];
uint16_t pwmData[(24 * MAX_LED) + 400];

uint32_t RGB_ConvertData(int _ledNum)
{
#if USE_BRIGHTNESS
	return (((ledData[_ledNum][1])*ledData[_ledNum][4]/100) << 16 |
			((ledData[_ledNum][2])*ledData[_ledNum][4]/100) << 8 |
			((ledData[_ledNum][3])*ledData[_ledNum][4]/100)
			);
#else
	return ((ledData[_ledNum][1] << 16) | (ledData[_ledNum][2] << 8) | ledData[_ledNum][3]));
#endif
}

void RGB_SetLED(int _ledNum, int _r, int _g, int _b)
{
	ledData[_ledNum][0] = _ledNum;
	ledData[_ledNum][1] = _g;
	ledData[_ledNum][2] = _r;
	ledData[_ledNum][3] = _b;
	ledData[_ledNum][4] = 100;
}

void RGB_SetBrightness(int _ledNum, int _amount)
{
	ledData[_ledNum][4] = _amount;
}

void RGB_SetAll(int _r, int _g, int _b, int _brightness)
{
	for (int ledNum = 0; ledNum < MAX_LED; ledNum++)
	{
		RGB_SetLED(ledNum, _r, _g, _b);
		RGB_SetBrightness(ledNum, _brightness);
	}
}

void RGB_Clear(void)
{
	RGB_SetAll(0, 0, 0, 0);
}

void RGB_Send(void)
{
	uint32_t index = 0;
	uint32_t color = 0;

	for (int i = 0; i < 150; i++)
	{
		pwmData[index] = 0;
		index++;
	}

	for (int ledNum = 0; ledNum < MAX_LED; ledNum++)
	{
		color = RGB_ConvertData(ledNum);
		for (int i = 23; i >= 0; i--)
		{
			if (color & (0x01 << i)) pwmData[index] = 30;
			else pwmData[index] = 10;
			index++;
		}
	}

	for (int i = 0; i < 100; i++)
	{
		pwmData[index] = 0;
		index++;
	}

	HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_1, (uint32_t*)pwmData, index);
	while(!datasentflag){};
	datasentflag = 0;
}

void RGB_Lerp(int* _color1, int* _color2, uint16_t _amount, int* _result)
{
	_result[0] = _color1[0] + ((_color2[0] - _color1[0]) * _amount/100);
	_result[1] = _color1[1] + ((_color2[1] - _color1[1]) * _amount/100);
	_result[2] = _color1[2] + ((_color2[2] - _color1[2]) * _amount/100);
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	HAL_TIM_PWM_Stop_DMA(&htim2, TIM_CHANNEL_1);
	datasentflag = 1;
}

/* USER CODE END 0 */

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim21;
DMA_HandleTypeDef hdma_tim2_ch1;

/* TIM2 init function */
void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 40-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}
/* TIM21 init function */
void MX_TIM21_Init(void)
{

  /* USER CODE BEGIN TIM21_Init 0 */

  /* USER CODE END TIM21_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM21_Init 1 */

  /* USER CODE END TIM21_Init 1 */
  htim21.Instance = TIM21;
  htim21.Init.Prescaler = 32-1;
  htim21.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim21.Init.Period = 65535;
  htim21.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim21.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim21) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim21, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim21, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM21_Init 2 */

  /* USER CODE END TIM21_Init 2 */

}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* tim_pwmHandle)
{

  if(tim_pwmHandle->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspInit 0 */

  /* USER CODE END TIM2_MspInit 0 */
    /* TIM2 clock enable */
    __HAL_RCC_TIM2_CLK_ENABLE();

    /* TIM2 DMA Init */
    /* TIM2_CH1 Init */
    hdma_tim2_ch1.Instance = DMA1_Channel5;
    hdma_tim2_ch1.Init.Request = DMA_REQUEST_8;
    hdma_tim2_ch1.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_tim2_ch1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_tim2_ch1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_tim2_ch1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_tim2_ch1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_tim2_ch1.Init.Mode = DMA_NORMAL;
    hdma_tim2_ch1.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_tim2_ch1) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(tim_pwmHandle,hdma[TIM_DMA_ID_CC1],hdma_tim2_ch1);

  /* USER CODE BEGIN TIM2_MspInit 1 */

  /* USER CODE END TIM2_MspInit 1 */
  }
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM21)
  {
  /* USER CODE BEGIN TIM21_MspInit 0 */

  /* USER CODE END TIM21_MspInit 0 */
    /* TIM21 clock enable */
    __HAL_RCC_TIM21_CLK_ENABLE();
  /* USER CODE BEGIN TIM21_MspInit 1 */

  /* USER CODE END TIM21_MspInit 1 */
  }
}
void HAL_TIM_MspPostInit(TIM_HandleTypeDef* timHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(timHandle->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspPostInit 0 */

  /* USER CODE END TIM2_MspPostInit 0 */

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**TIM2 GPIO Configuration
    PA5     ------> TIM2_CH1
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF5_TIM2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN TIM2_MspPostInit 1 */

  /* USER CODE END TIM2_MspPostInit 1 */
  }

}

void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef* tim_pwmHandle)
{

  if(tim_pwmHandle->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspDeInit 0 */

  /* USER CODE END TIM2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM2_CLK_DISABLE();

    /* TIM2 DMA DeInit */
    HAL_DMA_DeInit(tim_pwmHandle->hdma[TIM_DMA_ID_CC1]);
  /* USER CODE BEGIN TIM2_MspDeInit 1 */

  /* USER CODE END TIM2_MspDeInit 1 */
  }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM21)
  {
  /* USER CODE BEGIN TIM21_MspDeInit 0 */

  /* USER CODE END TIM21_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM21_CLK_DISABLE();
  /* USER CODE BEGIN TIM21_MspDeInit 1 */

  /* USER CODE END TIM21_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
