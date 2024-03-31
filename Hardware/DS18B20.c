#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"

#define DS18B20 GPIO_Pin_1
#define DS18B20_PORT GPIOA
#define DS18B20_LOW GPIO_ResetBits(DS18B20_PORT,DS18B20)
#define DS18B20_HIGH GPIO_SetBits(DS18B20_PORT,DS18B20)

/*本程序使用单总线挂载多个DS18B20*/

/*******************************************************************************
* 函 数 名					: DS18B20_Init
* 函数功能					: DS18B20初始化   
* 输    入				: 无
* 输    出				: 无
* 备    注				:
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
* 函 数 名					:DS18B20_Output_Intput
* 函数功能					:切换DS18B20的引脚状态
* 输    入				:0 or 1
* 输    出				:无
* 备    注				:0-上拉输入模式 1-推挽输出模式
*******************************************************************************/
void DS18B20_Output_Intput(u8 cmd)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	if(cmd)
	{
		GPIO_InitStructure.GPIO_Pin = DS18B20;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出模式
		GPIO_Init(DS18B20_PORT, &GPIO_InitStructure);
	}
	else
	{
		GPIO_InitStructure.GPIO_Pin = DS18B20;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入模式
	}
	GPIO_Init(DS18B20_PORT, &GPIO_InitStructure);
}


/*******************************************************************************
* 函 数 名					:DS18B20_Startup
* 函数功能					:DS18B20启动时序配置，并返回值检验是否挂载DS18B20，0挂载，1无挂载
* 输    入				:无
* 输    出				:0 or 1
* 备    注				:无
*******************************************************************************/
u8 DS18B20_Startup(void)
{
	u8 data;
	
	DS18B20_Output_Intput(1);
	DS18B20_LOW;
	Delay_us(480);
	DS18B20_HIGH;
	Delay_us(30);
	data = GPIO_ReadInputDataBit(DS18B20_PORT,DS18B20);//60~240us内返回值
	Delay_us(200);
	
	return data;
}


/*******************************************************************************
* 函 数 名					:DS18B20_Write_Byte
* 函数功能					:向DS18B20写入一字节数据
* 输    入				:一字节数据
* 输    出				:无
* 备    注				:无
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
* 函 数 名					:DS18B20_Read_Byte
* 函数功能					:向DS18B20读一字节数据
* 输    入				:无
* 输    出				:从DS18B20中读取到的一字节数据
* 备    注				:无
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
* 函 数 名					:DS18B20_Read_ROM
* 函数功能					:读取DS18B20的64位ROM地址
* 输    入				:无
* 输    出				:无
* 备    注				:每个DS18B20都有唯一的64位ROM地址，需要依次拆装依次读取
*******************************************************************************/
void DS18B20_Read_ROM(void)
{
	u8 rom[8];
	
	DS18B20_Write_Byte(0x33);
	for(u8 i=0;i<8;i++)
	{
		rom[i] = DS18B20_Read_Byte();
		printf("%#.2x,",rom[i]);//串口打印出当前挂载的DS18B20的64位ROM地址
	}
	printf("\r\n");
}


/*******************************************************************************
* 函 数 名					:DS18B20_Match_ROM
* 函数功能					:匹配符合输入ROM地址的DS18B20
* 输    入				:64位的ROM地址
* 输    出				:无
* 备    注				:无
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
* 函 数 名					:DS18B20_Read_Temp
* 函数功能					:读取符合输入ROM地址的DS18B20采集的温度值
* 输    入				:64位ROM地址和数据存放的变量
* 输    出				:温度值
* 备    注				:9bit-93.75ms，10bit-187.5ms，11bit-375ms，12bit-750ms
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
