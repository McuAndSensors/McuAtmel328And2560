/*
 * SoftUART.h
 *
 * Created: 04/01/2018 01:21:44
 *  Author: Adiel
 */ 


#ifndef SOFTUART_H_
#define SOFTUART_H_

#ifndef BAUD
#define BAUD
enum BaudRate{BAUD4800=4800,BAUD9600=9600,BAUD19200=19200,BAUD38400=38400,BAUD57600=57600};
#endif

class SoftUart
{
	private:
	char _str[50];
	public:
	//SoftUartUart(BaudRate rate = BAUD9600);
	void setBaudRate(BaudRate rate);
	char SoftUartReceiveData(void);
	void SoftUartTransmiteData(char data);
	void UARTstring(const char *StringPtr = "No String Entered",char space = 1);
	void UARTacko();
	void verafyNum(unsigned char value);
	char stringCmp(unsigned char *ptr1,const char *ptr2);
	char stringCmp(unsigned char *ptr1, char *ptr2);
	char stringCmp(char *ptr1, char *ptr2);
	void sendNum(unsigned char num);
	void printDecimalNumInAscii(unsigned long int num);
	void UARTstring(unsigned char *ptr);
};
#endif /* SOFTUART_H_ */