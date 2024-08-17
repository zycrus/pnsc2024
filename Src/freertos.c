/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "stdio.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
/* USER CODE BEGIN Variables */
int ledStates[5] = {0, 0, 0, 0, 0};

uint16_t ldrValue;
uint16_t potValue;

char* currRow1Text;
char* prevRow1Text;
char currRow2Text[16];
char prevRow2Text[16];
/* USER CODE END Variables */
osThreadId defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
osThreadId mainUpdateTaskHandle;
osThreadId lcdUpdateTaskHandle;
osThreadId ledUpdateTaskHandle;
osThreadId rgbUpdateTaskHandle;
osThreadId adcUpdateTaskHandle;
osThreadId swUpdateTaskHandle;


extern void SetLED(int*);
extern void HandleLED(int*, uint16_t);
extern void HandleADC(uint16_t*, uint16_t*);
extern void LCD_Init(void);
extern void LCD_GotoXY(char, char);
extern void LCD_Clear(void);
extern void LCD_SendStr(char*);
extern void HandleLCD(char*, char*);

extern void RGB_SetLED(int, int, int, int);
extern void RGB_SetBrightness(int, int);
extern void RGB_SetAll(int, int, int, int);
extern void RGB_Clear(void);
extern void RGB_Send(void);
extern void RGB_Lerp(int*, int*, uint16_t, int*);

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);

void MainUpdate(void const * argument);
void LCDUpdate(void const * argument);
void LEDUpdate(void const * argument);
void RGBUpdate(void const * argument);
void ADCUpdate(void const * argument);
void SWUpdate(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  osThreadDef(mainUpdateTask, MainUpdate, osPriorityNormal, 0, 128);
  mainUpdateTaskHandle = osThreadCreate(osThread(mainUpdateTask), NULL);

  osThreadDef(lcdUpdateTask, LCDUpdate, osPriorityNormal, 0, 128);
  lcdUpdateTaskHandle = osThreadCreate(osThread(lcdUpdateTask), NULL);

  osThreadDef(rgbUpdateTask, RGBUpdate, osPriorityAboveNormal, 0, 128);
  rgbUpdateTaskHandle = osThreadCreate(osThread(rgbUpdateTask), NULL);
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

void MainUpdate(void const * argument)
{
  /* USER CODE BEGIN MainUpdate */
  /* Infinite loop */
  for(;;)
  {
	HandleADC(&ldrValue, &potValue);
	HandleLED(ledStates, potValue);
	SetLED(ledStates);
    osDelay(1);
  }
  /* USER CODE END MainUpdate */
}

void LCDUpdate(void const * argument)
{
  /* USER CODE BEGIN LCDUpdate */
  /* Infinite loop */
  LCD_Init();
  currRow1Text = "Gas Stove";
//  currRow2Text = "";
  for(;;)
  {
	sprintf(currRow2Text, "Output: %d%%", potValue*100/4095);
	LCD_Clear();
	LCD_GotoXY(0,0);
	LCD_SendStr(currRow1Text);
	LCD_GotoXY(1, 0);
	LCD_SendStr(currRow2Text);
    osDelay(200);
  }
  /* USER CODE END LCDUpdate */
}

void RGBUpdate(void const * argument)
{
  /* USER CODE BEGIN LEDUpdate */
  /* Infinite loop */
  int brightness = 0;
  int color[3];
  int color1[3] = {255, 90, 0};
  int color2[3] = {255, 0, 0};
  RGB_Clear();
  RGB_Send();
  for(;;)
  {
	brightness = potValue*100/4095;
	RGB_Lerp(color1, color2, brightness, color);
	RGB_SetAll(color[0], color[1], color[2], brightness * 0.5);
	RGB_Send();
    osDelay(1);
  }
  /* USER CODE END LEDUpdate */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

