#include "intrins.h"

void Delay1ms(unsigned int t)		//@24.000MHz
{
	unsigned char i, j;
	for(t; t>0; t--)
	{
		_nop_();
		i = 32;
		j = 40;
		do
		{
			while (--j);
		} while (--i);
	}
}
void Delay1us(unsigned int t)		//@24.000MHz
{
	unsigned char i;
	for(t; t>0; t--)
	{
		i = 6;
		while (--i);
	}
}
