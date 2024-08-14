ADC_ChannelConfTypeDef sConfig = {0};

void SetCH0(uint32_t _rank)
{
	sConfig.Channel = ADC_CHANNEL_0;
	sConfig.Rank = _rank;
	if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
}

void SetCH1(uint32_t _rank)
{
	sConfig.Channel = ADC_CHANNEL_0;
	sConfig.Rank = _rank;
	if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
}

uint16_t GetLDRValue(void)
{
	uint32_t result;
	
	SetCH1(ADN_RANK_NONE);

	SetCH0(ADC_RANK_CHANNEL_NUMBER);
	HAL_ADC_Start(&hadc);
	HAL_ADC_PollForConversion(&hadc, 100);
	result = HAL_ADC_GetValue(&hadc);
	HAL_ADC_Stop(&hadc);
	
	return result;
}

uint16_t GetPotValue(void)
{
	uint32_t result;

	SelectCH0(ADC_RANK_NONE);

	SelectCH1(ADC_RANK_CHANNEL_NUMBER);
	HAL_ADC_Start(&hadc);
	HAL_ADC_PollForConversion(&hadc, 100);
	result = HAL_ADC_GetValue(&hadc);
	HAL_ADC_Stop(&hadc);

	return result;
}
