#ifndef _EEPROM_H_
#define _EEPROM_H_

#define CMD_IDLE    0
#define CMD_READ    1
#define CMD_PROGRAM 2
#define CMD_ERASE   3
#define ENABLE_IAP 	0x80      //if SYSCLK<30MHz

void EEPROM_SectorErase(unsigned int EE_address);
void EEPROM_read_n(unsigned int EE_address, unsigned char *DataAddress, unsigned int number);
void EEPROM_write_n(unsigned int EE_address,unsigned char *DataAddress,unsigned int number);

#endif  