#include "stm32f10x.h"  
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h" // he thong xung nhip
#include "stm32f10x_i2c.h"
#include "stm32f10x_usart.h"
#include <stdio.h>

#define BH1750_Addr_L 0x46
#define BH1750_ON 0x01
#define BH1750_RESET 0x07
#define BH1750_CONT_H_RES_MODE 0x10 // cdo do lien tuc do phan giai cao

float Voltt = 0;
uint16_t lux = 0;
char luxstring[8];// chuoi ky tu de luu tru giá tri lux khi truyen qua UART


void systemclock_config() {
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY));
	
    RCC->CFGR &= ~(RCC_CFGR_SW);
    RCC->CFGR |= RCC_CFGR_SW_HSE;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSE);
  
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));

    RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL);
    RCC->CFGR |= (RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL9);

    RCC->CFGR &= ~(RCC_CFGR_SW);
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

    RCC->CFGR &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2);
    RCC->CFGR |= (RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE1_DIV2 | RCC_CFGR_PPRE2_DIV1);

    FLASH->ACR &= ~(FLASH_ACR_LATENCY);
    FLASH->ACR |= FLASH_ACR_LATENCY_2;
}


void Gpio_Config() {
    GPIO_InitTypeDef gpio_struct; // cau hinh chan gpio
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE); // 
    
    // C?u h?nh I2C2 (PB10-SCL2, PB11-SDA2)
    gpio_struct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    gpio_struct.GPIO_Mode = GPIO_Mode_AF_OD;
    gpio_struct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &gpio_struct);

    // C?u h?nh UART1 TX (PA9) v? RX (PA10)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
    gpio_struct.GPIO_Pin = GPIO_Pin_9;
    gpio_struct.GPIO_Mode = GPIO_Mode_AF_PP; // hd o che do day keo
    gpio_struct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio_struct);
    
    gpio_struct.GPIO_Pin = GPIO_Pin_10;
    gpio_struct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpio_struct);	
}

void I2C2_Config(void) {
    I2C_InitTypeDef I2C_InitStruct;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);

    I2C_InitStruct.I2C_ClockSpeed = 100000;  // t?c d? tiêu chu?n cho giao ti?p I2C
    I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2; // chu ky lam viec 1 : 2 giua high va low
    I2C_InitStruct.I2C_OwnAddress1 = 0x00; // dia chi I2C vi dk
    I2C_InitStruct.I2C_Ack = I2C_Ack_Enable; //  bat tinh nang xac nhan (ACK) d ph?n h?i cho thi?t b? khác r?ng d? li?u dã du?c nh?n. 
	// Ði?u này quan tr?ng cho giao ti?p I2C chu?n.
    I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; // Ð?t d?a ch? I2C d?ng 7-bit
    I2C_Init(I2C2, &I2C_InitStruct);

    I2C_Cmd(I2C2, ENABLE);
}


void UART1_config(void) {
    USART_InitTypeDef USART_InitStruct;
    USART_InitStruct.USART_BaudRate = 9600;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    
    USART_Init(USART1, &USART_InitStruct);
    USART_Cmd(USART1, ENABLE);
}


void UART_SendChar(char c) {
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, c);
}


void UART_SendString(char *str) {
    while (*str) {
        UART_SendChar(*str++);
    }
}

void BH1750_Cmd_Write(uint8_t cmd) {
    uint32_t i;
    
    I2C_GenerateSTART(I2C2, ENABLE);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C2, BH1750_Addr_L, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    I2C_SendData(I2C2, cmd);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
    I2C_GenerateSTOP(I2C2, ENABLE);

    for (i = 0; i < 100000; i++);  // Delay thay th? cho HAL_Delay(100)
}


void BH1750_Start(void) {
    BH1750_Cmd_Write(BH1750_ON);    // B?t ngu?n
    BH1750_Cmd_Write(BH1750_RESET); // Reset c?m bi?n
    BH1750_Cmd_Write(BH1750_CONT_H_RES_MODE);  // Mode d? ph?n gi?i cao li?n t?c
	
}


uint16_t BH1750_Read(void) {
    uint8_t data_re[2] = {0, 0};
    uint16_t lux = 0;
    int i;
    
    // Start I2C
    I2C_GenerateSTART(I2C2, ENABLE);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));

    // G?i d?a ch?, ch?n ch? d? nh?n
    I2C_Send7bitAddress(I2C2, BH1750_Addr_L, I2C_Direction_Receiver);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    // Nh?n 2 byte d? li?u
    for (i = 0; i < 2; i++) {
        while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED));
        data_re[i] = I2C_ReceiveData(I2C2);
    }

    I2C_GenerateSTOP(I2C2, ENABLE);
    lux = (data_re[0] << 8) | data_re[1];
    lux = lux / 1.2;  // Chuy?n d?i sang don v? lux

    for (i = 0; i < 100000; i++);  // Delay thay th?
    return lux;
}

void bai6(void) {
    lux = BH1750_Read();
    sprintf(luxstring, "%dLux", lux);
    UART_SendString("Do sang: ");
    UART_SendString(luxstring);
    UART_SendString("\r\n\r\n");
}

int main(void) {
    uint32_t i;
    
    SystemInit();
    systemclock_config(); 
    Gpio_Config(); 
    UART1_config(); 
    I2C2_Config();
    BH1750_Start();

    while(1) {
        bai6();  // ??c v? g?i gi? tr? d? s?ng
        for (i = 0; i < 5000000; i++);  // Delay 1s
    }
}
