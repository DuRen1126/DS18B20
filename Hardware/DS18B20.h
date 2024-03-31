#ifndef __DS18B20_H
#define __DS18B20_H

void DS18B20_Init(void);
void DS18B20_Output_Intput(u8 cmd);
u8 DS18B20_Startup(void);
void DS18B20_Write_Byte(u8 data);
u8 DS18B20_Read_Byte(void);
void DS18B20_Read_ROM(void);
void DS18B20_Match_ROM(u8 *rom);
void DS18B20_Read_Temp(u8 *rom,u16 *data);

#endif