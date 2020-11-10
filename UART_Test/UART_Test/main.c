/*
 * UART_Test.c
 *
 * Created: 23/11/2017 21:30:57
 * Author : Aגןקך
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

#define USART_BAUDRATE 9600 //9600 default

#define UBRR_VALUE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

void init(void){
	// Set baud rate
	UBRR0H = (uint8_t)(UBRR_VALUE>>8);
	UBRR0L = (uint8_t)UBRR_VALUE;
	// Set frame format to 8 data bits, no parity, 1 stop bit
	UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00);
	//enable transmission and reception
	UCSR0B |= (1<<RXEN0)|(1<<TXEN0);
}

void sendByte(uint8_t u8Data){
	//wait while previous byte is completed
	while(!(UCSR0A&(1<<UDRE0))){};
	// Transmit data
	UDR0 = u8Data;
}

uint8_t ReceiveByte(){
	// Wait for byte to be received
	while(!(UCSR0A&(1<<RXC0))){};
	// Return received data
	return UDR0;
}

void write(const char* text){
	for(unsigned int nr = 0; nr < strlen(text); nr++){ // strlen <=> String.h
		sendByte((char)(text[nr]));
	}
}

int main (void){
	uint8_t u8TempData;
	//Initialize USART0
	init();
	
	sendByte(0x0C);
	
	write("\nHallo World.\n\n");
	
	while(1){
		// Receive data
		u8TempData = ReceiveByte();
		//Send back to terminal
		sendByte(u8TempData);
	}
}
