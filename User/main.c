#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"
#include "DS18B20.h"
#include "Buzzer.h"

u8 romA[]={0x28,0x23,0x6f,0x65,0xc1,0x21,0x06,0xbe,};
u8 romB[]={0x28,0xee,0x12,0xf7,0x43,0x20,0x01,0x66,};
u16 TempA,TempB;

int main(void)
{
	OLED_Init();
	Serial_Init();
	DS18B20_Init();
	Buzzer_Init();
	
//	DS18B20_Read_ROM();
	OLED_ShowString(1,1,"TempA:");
	OLED_ShowString(2,1,"TempB:");
	while (1)
	{
		DS18B20_Read_Temp(romA,&TempA);
		DS18B20_Read_Temp(romB,&TempB);

		if(TempA==300)
		{
			Temp_Warning(1);
			Delay_ms(100);
		}
		else
		{
			Temp_Warning(0);
			Delay_ms(100);
		}

		OLED_ShowNum(1,7,TempA/100%10,1);
		OLED_ShowNum(1,8,TempA/10%10,1);
		OLED_ShowChar(1,9, '.');
		OLED_ShowNum(1,10,TempA%10,1);

		OLED_ShowNum(2,7,TempB/100%10,1);
		OLED_ShowNum(2,8,TempB/10%10,1);
		OLED_ShowChar(2,9, '.');
		OLED_ShowNum(2,10,TempB%10,1);
	}
}
