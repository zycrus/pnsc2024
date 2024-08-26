# pnsc2024

LCD and LED ring driver is in src/tim.c\
ADC driver is in src/adc.c\
\
set clock to 32MHz\
\
TIM2:\
Counter Period: 40-1\
DMA: Memory to Peripheral\
    -Mode: Normal\
    -Increment Address: Memory\
    -Data Width: Half Word\
\
TIM21:\
Prescaler: 32-1\
Counter Period: 65535\
