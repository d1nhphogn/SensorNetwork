#include "test.h"

void Timer_Config(void) 
{
    TIM_TimeBaseInitTypeDef Timer; 

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    Timer.TIM_Period = 0xFFFF;
    Timer.TIM_Prescaler = 72 - 1;
    Timer.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &Timer);
    TIM_Cmd(TIM2, ENABLE);
}

void Delay_us(uint32_t us)
{
    TIM_SetCounter(TIM2, 0);
    while (TIM_GetCounter(TIM2) < us);
}

void Delay_ms(uint32_t ms)
{
    while (ms--){
        Delay_us(1000);
    }
}

void USART_Config(void) 
{
    GPIO_InitTypeDef gpioInit; 
    USART_InitTypeDef uartInit;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    // PA9 - TX
    gpioInit.GPIO_Pin = GPIO_Pin_9;
    gpioInit.GPIO_Mode = GPIO_Mode_AF_PP;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioInit);

    // PA10 - RX
    gpioInit.GPIO_Pin = GPIO_Pin_10;
    gpioInit.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpioInit);

    uartInit.USART_BaudRate = 9600;
    uartInit.USART_WordLength = USART_WordLength_8b;
    uartInit.USART_StopBits = USART_StopBits_1;
    uartInit.USART_Parity = USART_Parity_No;
    uartInit.USART_Mode = USART_Mode_Tx;
    uartInit.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &uartInit);

    USART_Cmd(USART1, ENABLE);
}

void USART_SendString(char *str)
{
    while (*str) {
        USART_SendData(USART1, *str);
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
        str++;
    }
}

void USART_SendNumber(uint8_t num) 
{
    char buffer[5];
    sprintf(buffer, "%d", num);
    USART_SendString(buffer);
}

void DHT11_Config(void) 
{
    GPIO_InitTypeDef gpioInit;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    gpioInit.GPIO_Pin = GPIO_Pin_12;
    gpioInit.GPIO_Mode = GPIO_Mode_Out_OD;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &gpioInit);
}


