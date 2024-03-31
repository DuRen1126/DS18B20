#include "stm32f10x.h"
#include "Buzzer.h"
#include "Delay.h"

#define Buzzer_Port GPIOA
#define Buzzer_Pin GPIO_Pin_0

extern u16 TempA,TempB;

void Buzzer_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = Buzzer_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(Buzzer_Port, &GPIO_InitStructure);
}

void Temp_Warning(u8 cmd)
{
	if(cmd)
	{
		GPIO_ResetBits(Buzzer_Port, Buzzer_Pin);
		Delay_ms(100);
	}
	else
	{
		GPIO_SetBits(Buzzer_Port, Buzzer_Pin);
		Delay_ms(100);
	}
}