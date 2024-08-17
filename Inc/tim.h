/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.h
  * @brief   This file contains all the function prototypes for
  *          the tim.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIM_H__
#define __TIM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern TIM_HandleTypeDef htim2;

extern TIM_HandleTypeDef htim21;

/* USER CODE BEGIN Private defines */
#define MAX_LED 12
#define USE_BRIGHTNESS 1
#define PI 3.14159265
/* USER CODE END Private defines */

void MX_TIM2_Init(void);
void MX_TIM21_Init(void);

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* USER CODE BEGIN Prototypes */

void delayMicro(uint16_t);
/*LCD Functions*/
void LCD_Send(char, int);
void LCD_SendCMD(char);
void LCD_SendData(char);
void LCD_Init(void);
void LCD_Clear(void);
void LCD_GotoXY(char, char);
void LCD_SendStr(char*);
void HandleLCD(char*, char*, char*, char*);

/*WS2812B Functions*/
uint32_t RGB_ConvertData(int);
void RGB_SetLED(int, int, int, int);
void RGB_SetBrightness(int, int);
void RGB_SetAll(int, int, int, int);
void RGB_Clear(void);
void RGB_Send(void);
void RGB_Lerp(int*, int*, uint16_t, int*);


/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __TIM_H__ */

