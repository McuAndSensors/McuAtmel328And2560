/*
 * TEST1.cpp
 *
 * Created: 03/01/2018 18:36:42
 * Author : Adiel
 */ 
#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>
#include "UART.h"
#include "SoftUART.h"
Uart test;
SoftUart test1;

char SoftUartReceiveData()
{
	char data = 0;
	while(bit_is_set(PINB,0));
	if (!bit_is_set(PINB,0))
	{
		_delay_us(104.5);
		for(char i = 0; i < 8; i++)
		{
			_delay_us(51);
			data |= bit_is_set(PINB,0) ? (1 << i) : (0 << i);
			_delay_us(51);
		}
		_delay_us(10);
		if (bit_is_set(PINB,0))
		{
			_delay_us(94);
			return data;
		}
	}
	return 0;
}

int main(void)
{
	DDRC |= (1<<0)|(1<<1);
	char data = 0;
    /* Replace with your application code */
    while (1) 
    {
		data = test1.SoftUartReceiveData();
		test.UARTsend(data);
    }
}

