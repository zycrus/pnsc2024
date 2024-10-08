#define PI 3.14159265
#define USE_BRIGHTNESS 1

int datasentflag = 0;

int ledData[MAX_LED][5];
int pwmData[(24 * MAX_LED) + 400];

void RGB_SetLED(int _ledNum, int _r, int _g, int _b)
{
	ledData[_ledNum][0] = _ledNum;
	ledData[_ledNum][1] = _g;
	ledData[_ledNum][2] = _r;
	ledData[_ledNum][3] = _b;
	ledData[_ledNum][4] = 1;
}

void RGB_SetBrightness(int _ledNum, int _amount)
{
	float brightness = _amount;
	if (brightness > 100) brightness = 100;
	ledData[_ledNum][4] = brightness;	// Store in LED array
}

uint32_t RGB_ConvertData(int _ledNum)
{
#if
	return (
		((ledData[_ledNum][1]*ledData[4]/100) << 16) |
		((ledData[_ledNum][2]*ledData[4]/100) << 8) |
		(ledData[_ledNum][3]*ledData[4]/100)
		);
#else
	return ((ledData[_ledNum][1]) | (ledData[_ledNum][2]) | (ledData[_ledNum][3]));
#endif	
}

void RGB_Send(void)
{
	uint32_t index = 0;
	uint32_t color = 0;

	for(int i = 0; i < 150)
	{
		pwmData[index] = 0;
		index++;
	}
	
	for (int = 0; i < MAX_LED; i++)
	{
		for (int j = 23; j >= 0; j--)
		{
			if (color & (1 << j) pwmData[index] = 30;
			else pwmData[index] = 9;
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
	datasentflag = 1;
}
