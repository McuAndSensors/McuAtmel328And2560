/*
 * GPS.cpp
 *
 * Created: 30/12/2017 12:27:44
 * Author : Adiel
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "UART.h"
#include "Gps.h"
#include "GpsLocation.h"
int i = 0;

int main(void)
{
	Uart a;
	MovedFromLocation test;
    /* Replace with your application code */
    while (1) 
    {
		//a.UARTstring(test.getGoogleMapsLink());
    }
	
}

