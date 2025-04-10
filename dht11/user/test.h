#ifndef __TEST__
#define __TEST__

#include "stm32f10x.h"                 
#include "stm32f10x_gpio.h" 
#include "stm32f10x_usart.h" 
#include "stm32f10x_rcc.h"              
#include "stm32f10x_tim.h"              
           
#include "string.h"
#include <stdio.h>

void Timer_Config(void);
void Delay_ms(uint32_t ms);
void Delay_us(uint32_t us);
void USART_Config(void);
void USART_SendString(char *str);
void DHT11_Config(void);
void USART_SendNumber(uint8_t num);

#endif
