void LCD_SendByte(char _byte, int _rs)
{
  HAL_GPIO_WritePin(RS_GPIOP_Port, RS_Pin, _rs);
  
  HAL_GPIO_WritePin(D4_GPIO_Port, D4_pin, (_byte<<0) & 0x01);
  HAL_GPIO_WritePin(D4_GPIO_Port, D5_pin, (_byte<<1) & 0x01);
  HAL_GPIO_WritePin(D4_GPIO_Port, D6_pin, (_byte<<2) & 0x01);
  HAL_GPIO_WritePin(D4_GPIO_Port, D7_pin, (_byte<<3) & 0x01);

  HAL_GPIO_WritePin(EN_GPIO_Port, EN_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(EN_GPIO_Port, EN_Pin, GPIO_PIN_RESET);
}

void LCD_SendCMD(char _cmd)
{
  LCD_SendByte((_cmd>>4) & 0x0f, 0);
  LCD_SendByte((_cmd) & 0x0f, 0);
}

void LCD_SendData(char _data)
{
  LCD_SendByte((_data>>4) & 0x0f, 1);
  LCD_SendByte((_data) & 0x0f, 1);
}

void LCD_GoToXY(int x, int y)
{
  LCD_SendCMD(0x80 + _col + (_row * 0x40), _htim);
}

void LCD_Init()
{
  HAL_Delay(80);
  LCD_SendCMD(0x02);
  LCD_SendCMD(0x28);
  delayMicro(40);
  LCD_SendCMD(0x08);
  delayMicro(40);
  LCD_SendCMD(0x01);
  HAL_Delay(2);
  LCD_SendCMD(0x06);
  LCD_SendCMD(0x0C);
}

void LCD_SendString(char *_str)
{
  while (*_str) LCD_SendData(*_str++);
}

void LCD_Clear()
{
  LCD_SendCMD(0x01);
}
