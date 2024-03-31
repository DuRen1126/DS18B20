#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"

#define DS18B20 GPIO_Pin_1
#define DS18B20_PORT GPIOA
#define DS18B20_LOW GPIO_ResetBits(DS18B20_PORT,DS18B20)
#define DS18B20_HIGH GPIO_SetBits(DS18B20_PORT,DS18B20)

/*������ʹ�õ����߹��ض��DS18B20*/

/*******************************************************************************
* �� �� ��					: DS18B20_Init
* ��������					: DS18B20��ʼ��   
* ��    ��				: ��
* ��    ��				: ��
* ��    ע				:
*******************************************************************************/
void DS18B20_Init(void)
{
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = DS18B20;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	
 	GPIO_Init(DS18B20_PORT, &GPIO_InitStructure);
}

/*******************************************************************************
* �� �� ��					:DS18B20_Output_Intput
* ��������					:�л�DS18B20������״̬
* ��    ��				:0 or 1
* ��    ��				:��
* ��    ע				:0-��������ģʽ 1-�������ģʽ
*******************************************************************************/
void DS18B20_Output_Intput(u8 cmd)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	if(cmd)
	{
		GPIO_InitStructure.GPIO_Pin = DS18B20;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//�������ģʽ
		GPIO_Init(DS18B20_PORT, &GPIO_InitStructure);
	}
	else
	{
		GPIO_InitStructure.GPIO_Pin = DS18B20;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������ģʽ
	}
	GPIO_Init(DS18B20_PORT, &GPIO_InitStructure);
}


/*******************************************************************************
* �� �� ��					:DS18B20_Startup
* ��������					:DS18B20����ʱ�����ã�������ֵ�����Ƿ����DS18B20��0���أ�1�޹���
* ��    ��				:��
* ��    ��				:0 or 1
* ��    ע				:��
*******************************************************************************/
u8 DS18B20_Startup(void)
{
	u8 data;
	
	DS18B20_Output_Intput(1);
	DS18B20_LOW;
	Delay_us(480);
	DS18B20_HIGH;
	Delay_us(30);
	data = GPIO_ReadInputDataBit(DS18B20_PORT,DS18B20);//60~240us�ڷ���ֵ
	Delay_us(200);
	
	return data;
}


/*******************************************************************************
* �� �� ��					:DS18B20_Write_Byte
* ��������					:��DS18B20д��һ�ֽ�����
* ��    ��				:һ�ֽ�����
* ��    ��				:��
* ��    ע				:��
*******************************************************************************/
void DS18B20_Write_Byte(u8 data)
{
	for(u8 i=0;i<8;i++)
	{
		DS18B20_Output_Intput(1);
		DS18B20_LOW;
		Delay_us(2);
		(data&0x01)?DS18B20_HIGH:DS18B20_LOW;
		Delay_us(45);
		DS18B20_HIGH;
		
		data >>=1;
	}
}


/*******************************************************************************
* �� �� ��					:DS18B20_Read_Byte
* ��������					:��DS18B20��һ�ֽ�����
* ��    ��				:��
* ��    ��				:��DS18B20�ж�ȡ����һ�ֽ�����
* ��    ע				:��
*******************************************************************************/
u8 DS18B20_Read_Byte(void)
{
	
	u8 data = 0;
	
	for(u8 i=0;i<8;i++)
	{
		data >>=1;
		DS18B20_Output_Intput(1);
		DS18B20_LOW;
		Delay_us(2);
		DS18B20_HIGH;
		DS18B20_Output_Intput(0);
		if(GPIO_ReadInputDataBit(DS18B20_PORT,DS18B20) == SET)
		{
			data|=0x80;
		}
		Delay_us(45);
	}
	return data;
}


/*******************************************************************************
* �� �� ��					:DS18B20_Read_ROM
* ��������					:��ȡDS18B20��64λROM��ַ
* ��    ��				:��
* ��    ��				:��
* ��    ע				:ÿ��DS18B20����Ψһ��64λROM��ַ����Ҫ���β�װ���ζ�ȡ
*******************************************************************************/
void DS18B20_Read_ROM(void)
{
	u8 rom[8];
	
	DS18B20_Write_Byte(0x33);
	for(u8 i=0;i<8;i++)
	{
		rom[i] = DS18B20_Read_Byte();
		printf("%#.2x,",rom[i]);//���ڴ�ӡ����ǰ���ص�DS18B20��64λROM��ַ
	}
	printf("\r\n");
}


/*******************************************************************************
* �� �� ��					:DS18B20_Match_ROM
* ��������					:ƥ���������ROM��ַ��DS18B20
* ��    ��				:64λ��ROM��ַ
* ��    ��				:��
* ��    ע				:��
*******************************************************************************/
void DS18B20_Match_ROM(u8 *rom)
{

	DS18B20_Startup();
	DS18B20_Write_Byte(0x55);
	for(u8 i=0;i<8;i++)
	{
		DS18B20_Write_Byte(*rom++);
	}
}


/*******************************************************************************
* �� �� ��					:DS18B20_Read_Temp
* ��������					:��ȡ��������ROM��ַ��DS18B20�ɼ����¶�ֵ
* ��    ��				:64λROM��ַ�����ݴ�ŵı���
* ��    ��				:�¶�ֵ
* ��    ע				:9bit-93.75ms��10bit-187.5ms��11bit-375ms��12bit-750ms
*******************************************************************************/
void DS18B20_Read_Temp(u8 *rom,u16 *data)
{
	
	u8 LSB=0,MSB=0;
	u16 Temp=0;
	
	DS18B20_Match_ROM(rom);
	DS18B20_Write_Byte(0x44);
	Delay_ms(750);//12bit-750ms
	DS18B20_Match_ROM(rom);
	DS18B20_Write_Byte(0xbe);
	
	LSB = DS18B20_Read_Byte();
	MSB = DS18B20_Read_Byte();
	
	Temp = (MSB << 8) | LSB;
	
	if((Temp&0xf800) == 0xf800)
	{
		*data = (((~Temp+0x01)*-0.0625)+0.5)*10.0;
	}
	else
	{
		*data = ((Temp*0.0625)+0.5)*10.0;
	}
}
