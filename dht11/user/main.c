#include "test.h"

uint16_t u16Tim;
uint8_t u8Buff[5];
uint8_t u8CheckSum;
unsigned int i, j;

int main()
{
	Timer_Config();
	DHT11_Config();
	USART_Config();
	
	while(1){
		GPIO_ResetBits(GPIOB, GPIO_Pin_12);
		Delay_ms(20);
		GPIO_SetBits(GPIOB, GPIO_Pin_12);
		
		TIM_SetCounter(TIM2, 0);
		while(TIM_GetCounter(TIM2) < 10){
			if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)){
				break;
			}
		}
		
		u16Tim = TIM_GetCounter(TIM2);
		if(u16Tim >= 10){
			continue;
		}
		
		TIM_SetCounter(TIM2, 0);
		while(TIM_GetCounter(TIM2) < 45){
			if(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)){
				break;
			}
		}
		
		u16Tim = TIM_GetCounter(TIM2);
		if((u16Tim >= 45) || (u16Tim <= 5)){
			continue;
		}
		
		TIM_SetCounter(TIM2, 0);
		while(TIM_GetCounter(TIM2) < 90){
			if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)){
				break;
			}
		}
		
		u16Tim = TIM_GetCounter(TIM2);
		if((u16Tim >= 90) || (u16Tim <= 70)){
			continue;
		}
		
		TIM_SetCounter(TIM2,0);
		while(TIM_GetCounter(TIM2) < 95){
			if(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)){
				break;
			}
		}
		
		u16Tim = TIM_GetCounter(TIM2);
		if((u16Tim >= 95) || (u16Tim <= 75)){
			continue;
		}
		
		for(i = 0; i < 5; i++){
			for(j = 0; j < 8; j++){
				TIM_SetCounter(TIM2,0);
				while(TIM_GetCounter(TIM2) < 65){
					if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)){
						break;
					}
				}
				
				u16Tim = TIM_GetCounter(TIM2);
				if((u16Tim >= 65) || (u16Tim <= 45)){
					continue;
				}
				
				TIM_SetCounter(TIM2, 0);
				while(TIM_GetCounter(TIM2) < 80){
					if(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)){
						break;
					}
				}
				
				u16Tim = TIM_GetCounter(TIM2);
				if((u16Tim >= 80) || (u16Tim <= 10)){
					continue;
				}
				
				u8Buff[i] <<= 1;
				if(u16Tim > 45){
					u8Buff[i] |= 1;
				} else{
					u8Buff[i] &= ~1;
				}
			}
		}
		
		u8CheckSum = u8Buff[0] + u8Buff[1] + u8Buff[2] + u8Buff[3];
		if(u8CheckSum != u8Buff[4]){
			continue;
		}
		
		USART_SendString("Temperature: ");
		USART_SendNumber(u8Buff[2]);
		USART_SendString("*C\r\n");
		USART_SendString("Humidity: ");
		USART_SendNumber(u8Buff[0]);
		USART_SendString("%\r\n");
		Delay_ms(3000);
	}
}


