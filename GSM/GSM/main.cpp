/*
 * GSM.cpp
 *
 * Created: 19/12/2017 12:52:07
 * Author : Adiel
 */ 
#define  F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <string.h>

enum BaudRate{BAUD4800=4800,BAUD9600=9600,BAUD19200=19200,BAUD38400=38400,BAUD57600=57600};
class Uart
{
	private:
	char _Str[20];
	public:
	Uart(BaudRate rate = BAUD9600)
	{
		setBaudRate(rate);
	}
	void setBaudRate(BaudRate rate)
	{
		unsigned int BAUD_PRESCALLER = (((F_CPU / (rate * 16UL))) - 1);
		UBRR0H = (unsigned int)(BAUD_PRESCALLER>>8);
		UBRR0L = (unsigned int)(BAUD_PRESCALLER);
		UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
		UCSR0C = (3<<UCSZ00);
		
		/***********************remove**************************/
		UBRR3H = (unsigned int)(BAUD_PRESCALLER>>8);
		UBRR3L = (unsigned int)(BAUD_PRESCALLER);
		UCSR3B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
		UCSR3C = (3<<UCSZ00);
		/******************************************************/
	}
	char checkForData()
	{
		for(int i = 0; i < 500; i++)
		{
			if ((UCSR0A & (1<<RXC0)))
			{
				return 1;
			}
			_delay_us(150);
		}
		return 0;
	}
	char getData()
	{
		return UDR0;
	}
	char checkForData3()
	{
		for(int i = 0; i < 500; i++)
		{
			if ((UCSR3A & (1<<RXC3)))
			{
				return 1;
			}
			_delay_us(150);
		}
		return 0;
	}
	char getData3()
	{
		return UDR3;
	}
	void UARTsend3( unsigned char data)
	{
		while(!(UCSR3A & (1<<UDRE3)));
		UDR3 = data;
	}
	unsigned char UARTreceive3(void)
	{
		while(!(UCSR3A & (1<<RXC3)));
		return UDR3;
	}
	void UARTstring3(const char *StringPtr = "No String Entered",char space = 1)
	{
		if (space)
		{
			UARTsend3((const char)*" ");
		}
		while(*StringPtr != 0)
		{
			UARTsend3(*StringPtr);
			StringPtr++;
		}
		if (space)
		{
			UARTsend3((const char)*" ");
		}
	}
	unsigned char UARTreceive(void)
	{
		while(!(UCSR0A & (1<<RXC0)));
		return UDR0;
	}
	char *UARTreceiveString(char size = 20)
	{
		int i = 0;
		
		while(UARTreceive() != 0 && i > size)
		{
			_Str[i] = UARTreceive();
			i++;
		}
		_Str[i] = 0;
		return _Str;
	}
	void UARTsend( unsigned char data)
	{
		while(!(UCSR0A & (1<<UDRE0)));
		UDR0 = data;
	}
	void UARTstring(const char *StringPtr = "No String Entered",char space = 1)
	{
		if (space)
		{
			UARTsend((const char)*" ");
		}
		while(*StringPtr != 0)
		{
			UARTsend(*StringPtr);
			StringPtr++;
		}
		if (space)
		{
			UARTsend((const char)*" ");
		}
	}
	void UARTacko()
	{
		char data;
		data=UARTreceive();
		UARTsend(data);
	}
	void verafyNum(unsigned char value)
	{
		switch (value)
		{
			case 0:UARTsend('0'); break;
			case 1:UARTsend('1'); break;
			case 2:UARTsend('2'); break;
			case 3:UARTsend('3'); break;
			case 4:UARTsend('4'); break;
			case 5:UARTsend('5'); break;
			case 6:UARTsend('6'); break;
			case 7:UARTsend('7'); break;
			case 8:UARTsend('8'); break;
			case 9:UARTsend('9'); break;
			case 10:UARTsend('A'); break;
			case 11:UARTsend('B'); break;
			case 12:UARTsend('C'); break;
			case 13:UARTsend('D'); break;
			case 14:UARTsend('E'); break;
			case 15:UARTsend('F'); break;
		}
	}
	char stringCmp(unsigned char *ptr1,const char *ptr2)
	{
		while(*ptr1 == 0 || *ptr2 == 0)
		{
			if (*ptr1 == *ptr2)
			{
				ptr2++;
				ptr1++;
			}
			else
			{
				return 0;
			}
		}
		return 1;
	}
	char stringCmp(unsigned char *ptr1, char *ptr2)
	{
		while(*ptr1 == 0 || *ptr2 == 0)
		{
			if (*ptr1 == *ptr2)
			{
				ptr2++;
				ptr1++;
			}
			else
			{
				return 0;
			}
		}
		return 1;
	}
	char stringCmp(char *ptr1, char *ptr2)
	{
		while(*ptr1 == 0 || *ptr2 == 0)
		{
			if (*ptr1 == *ptr2)
			{
				ptr2++;
				ptr1++;
			}
			else
			{
				return 0;
			}
		}
		return 1;
	}
	void sendNum(unsigned char num)
	{
		verafyNum(num/16);
		verafyNum(num%16);
		UARTsend(',');
		UARTsend(' ');
	}
	void printDecimalNumInAscii(unsigned long int num)
	{
		int count = 1;
		int arr[10] = {0};
		while(num != 0)
		{
			arr[count] = num % 10;
			num/= 10;
			count++;
		}
		while(count != 0)
		{
			UARTsend(arr[count]+'0');
			count--;
		}
		UARTstring(",",1);
	}
	void UARTstring(unsigned char *ptr)
	{
		while(*ptr != 0)
		{
			UARTsend(*ptr);
			ptr++;
		}
	}
};

class SMS
{
private:
	Uart gsmSMS;
	char _phoneBook[10][11];
	char _sendPacket[500];
	char _recievedPack[500];
	int _SMSindex;
	bool _flagRXC;
	char _RXackValue;
	bool sendCommand(const char ptr[], bool state = false){
		int condition;
		strcpy(_sendPacket,ptr);
		//gsmSMS.UARTstring(_sendPacket,0);
		//gsmSMS.UARTsend(13);
		gsmSMS.UARTstring3(_sendPacket,0);
		gsmSMS.UARTsend3(13);
		if (state)
		{
			condition = checkForACkPacket();
			return (condition > 0) ? compWordInArr(_recievedPack,"OK",2,condition): condition;
		} 
		else
		{
			return 1;
		}
	}
	int checkForACkPacket(){
		int i = 0;
		char data = 0;
		do 
		{
			data = gsmSMS.UARTreceive3();
			_recievedPack[i++] = data;
		}while(gsmSMS.checkForData3());
		
		_recievedPack[i] = 0;
		return i > 0 ? i: 0;
	}
	char checkSignal(){
		if (sendCommand("AT+CSQ"))
		{
			if (_recievedPack[7]-'0' >= 0 && _recievedPack[7]-'0' <= 9)
			{
				return (_recievedPack[6]-'0')*10 + (_recievedPack[7]-'0');
			}
			else
			{
				return _recievedPack[6]-'0';
			}
		}
		return 0;
	}
	bool compWordInArr(const char *arr,const char *word, char wordSize, char arrMaxSize) const
	{
		bool status = true;
		for(int i = 0; i < arrMaxSize; i++)
		{
			if (arr[i] == *word)
			{
				for (int j = 0; j < wordSize; j++,i++)
				{
					if (!(arr[i] == word[j]))
					{
						status = false;
					}
				}
				if (status)
				{
					return status;
				}
			}
		}
		return false;
	}
	void setSMSindex()
	{
		int index = 3;
		while(_recievedPack[index++] != ',');
		_SMSindex = _recievedPack[index++] - '0';
		if (_recievedPack[index]-'0' >= 0 && _recievedPack[index]-'0' <= 9)
		{
			_SMSindex *= 10;
			_SMSindex += _recievedPack[index] - '0';
		}
		if (_SMSindex > 20)
		{
			cli();
			while(_SMSindex != 0)
			{
				deleteSMS(_SMSindex--);	
			}
			sei();
		}
	}
public:
	SMS(){setup(BAUD9600);}
	~SMS(){}
	bool checkForACkPacket1()
	{
		return _RXackValue != 0 ? true : false;
	}
	friend void flagDataReceived(SMS& flag)
	{
		cli();
		flag.checkStatus();
		flag._flagRXC = true;
		sei();
	}
	bool flagStatus(){
		return _flagRXC;
	}
	int getRXackID(){
		int ack = (int)_RXackValue;
		_flagRXC = false;
		_RXackValue = 0;
		return ack;
	}
	void setBaudRate(BaudRate baud){
		gsmSMS.setBaudRate(baud);
	}
	bool setup(BaudRate BAUD = BAUD9600){
		//_delay_ms(3000);
		TCCR1B |= ((1 << CS10) | (1 << CS12)); // start the timer at 20MHz/1024
		gsmSMS.setBaudRate(BAUD);
		char ptr[] = {'A','T','+','C','S','C','S','=','"','G','S','M','"',0};
		sendCommand(ptr,true);
		return sendCommand("AT+CMGF=1",true);
	}
	bool sendSMS(const char *ptr,int index){
		int massageSize = 0;
		cli();
		strcpy(_sendPacket,"AT+CMGS=");								//copy command
		_sendPacket[8] ='"';										//set <"> for phone number
		strcpy(_sendPacket+9,"+972");								//copy state code
		strcpy(_sendPacket+13,_phoneBook[index]+1);					//copy phone number
		_sendPacket[22] ='"';										//put end of string NULL by adding dummy char
		_sendPacket[23] = 0;										//close number with <">, run over the dummy char
		
		sendCommand(_sendPacket,true);
		
		massageSize = strlen(ptr);									//SMS size
		strcpy(_sendPacket,ptr);									//copy massage from ptr
		_sendPacket[massageSize+1] = 0;								//put end of string
		sendCommand(_sendPacket);									//send the massage
		_sendPacket[0] = 26;										//put CTRL-Z
		_sendPacket[1] = 0;											//put end of string
		sei();
		return sendCommand(_sendPacket);							//send the CTRL-Z
	return false;
	}
	void checkStatus(){
		int i = 0;
		while(gsmSMS.checkForData3())
		{
			_recievedPack[i++] = gsmSMS.getData3();
		}
		_recievedPack[i] = 0;
		if (i > 0)
		{
			if(compWordInArr(_recievedPack,"OK",2,i))
			{
				_RXackValue = 1;
			}
			if(compWordInArr(_recievedPack,">",1,i))
			{
				_RXackValue = 1;
			}
			else if (compWordInArr(_recievedPack,"ERROR",5,i))
			{
				_RXackValue = 2;
			}
			else if (compWordInArr(_recievedPack,"RING",4,i))
			{
				_RXackValue = 3;
			}
			else if (compWordInArr(_recievedPack,"+CMTI",5,i)||compWordInArr(_recievedPack,"SM",4,i))
			{
				setSMSindex();
				_RXackValue = 4;
			}
			else if(compWordInArr(_recievedPack,"+CMGS",5,i)||compWordInArr(_recievedPack,"CMGS",4,i))
			{
				_RXackValue = 5;
			}
			else if(compWordInArr(_recievedPack,"+CPIN",5,i)||compWordInArr(_recievedPack,"+CFUN",5,i))
			{
				_RXackValue = 6;
			}
			else
			{
				_RXackValue = 0;
				_flagRXC = false;
			}
		}
		else
		{
			_RXackValue = 0;
			_flagRXC = false;
		}
	}
	char *readSMS(){
		int indexNum = 13, indexData = 0, indexOk = 0;
		cli();
		strcpy(_sendPacket,"AT+CMGR=");
		_sendPacket[8]= _SMSindex > 9? _SMSindex/10+'0' : _SMSindex+'0';
		_sendPacket[9]= _SMSindex > 9? _SMSindex%10+'0' : 0;
		_sendPacket[10] = 0;
		if (sendCommand(_sendPacket,true))
		{
			//get phone index number
			while(!(_recievedPack[++indexNum] == '"' && _recievedPack[++indexNum] == '+'));
			indexData = indexNum;	//data mast be after number
			
			//get data, index number.
			while(_recievedPack[indexData++] != '\n');
			indexOk = indexData;	//OK must be after data
			
			//get end of data index number (it will get down a line and send OK)
			while(_recievedPack[indexOk++] != '\n');
			_recievedPack[indexNum+13] = 0;		//set end of string for strcpy()
			
			//copy the phone number
			strcpy(_recievedPack,_recievedPack+indexNum+3);
			
			//set phone start number, always 0
			_recievedPack[0] = '0';
			
			_recievedPack[10] = '\n';	//get down a line and put data received
			
			_recievedPack[indexOk] = 0;	//put end of string after the end of data
			strcpy(_recievedPack+11,_recievedPack+indexData); //copy data
			
			sei();
			return _recievedPack;
		}
		return 0;
	}
	bool deleteSMS(char SMSindex = 1){
		if (SMSindex < 10)
		{
			strcpy(_sendPacket,"AT+CMGD= ");
			_sendPacket[8] = SMSindex+'0';
		}
		else
		{
			strcpy(_sendPacket,"AT+CMGD=  ");
			_sendPacket[8] = SMSindex/10+'0';
			_sendPacket[9] = SMSindex%10+'0';
		}
		return sendCommand(_sendPacket,true);
	}
	void setNewNum(const char *ptr,int index = 0){
		strcpy(_phoneBook[index],ptr);
	}
	bool compPhoneNum(const char *ptr){
		bool state = true;
		for(int j = 0; j < 10; j++)
		{
			for(int i = 0; i < 10; i++)
			{
				if (!(*ptr++==_phoneBook[i][j]))
				{
					state = false;
				}
			}
			if (state)
			{
				return state;
			}
		}
		return false;
	}
	char *getPhoneNumber(int index){
		return _phoneBook[index];}
	bool call(int index){
		strcpy(_sendPacket,"ATD");
		strcpy(_sendPacket+3,_phoneBook[index]);
		_sendPacket[13] = ';';
		_sendPacket[14] = 0;
		return sendCommand(_sendPacket);
		}
	bool hangUpCall(){
		return sendCommand("ATH");
	}
};

SMS test;
ISR(USART3_RX_vect)
{
	flagDataReceived(test);
}
int main(void)
{
	Uart check;
	sei();
	test.setNewNum("0526164258",0);
	test.setNewNum("0527899361",1);
	test.setNewNum("0525308748",2);
	check.setBaudRate(BAUD9600);
	unsigned char data = 0;
	int i = 0, mem = 0;
	unsigned char arr[100];
    while (1) 
    {
		//check.UARTstring("\n",0);
		
		//__enable_interrupt();
		//RESET ARR
		for(int j = 0;j < 100; j++)
		{
			arr[j] = 0;
		}
		if (test.flagStatus())
		{
			switch(test.getRXackID())
			{
				case 3:check.UARTstring("received a call");break;
				case 4:
				{
					check.UARTstring("SMS was received");
					test.sendSMS("Thank you, the location is\nKiryat Bialike Dror 8\nfor Further information please call Krisoosh!\nnaaaaaai!!!!",2);
					break;
				}
				case 5:check.UARTstring("SMS was sent");break;
				case 6:check.UARTstring("device restarting");break;
			}
		}
		//CHECK FOR DATA ON THE SERIAL COMUNICATION FROGRAM
		while(check.checkForData())
		{
			data = check.getData();
			if (data != 0)
			{
				if (data == '*' || data == '!')
				{
					arr[i++] = (data == '*')? 0x1A : 13 ;
				}
				else
				{
					arr[i++] = data;
				}
			}
			
			if (data == '&')
			{
				if (test.sendSMS("THIS is a test massage number 34 and counting",2))
				{
					check.UARTstring("SMS was sent");
				}
			}
			else if(data == '^')
			{
				check.UARTstring(test.readSMS(),0);
			}
			if(data == '%')
			{
				if (test.deleteSMS(1))
				{
					check.UARTstring("SMS was deleted");
				}
			}
		}
		
		/*
		//IF DATA RECIEVED, SEND IT THROUGH UART3
		if (i > 0)
		{
			mem = i+1;
			i = 0;
			while(i < mem)
			{
				check.UARTsend3(arr[i++]);
			}
			i = 0;
			
			//RESET ARR
			for(int j = 0;j < 100; j++)
			{
				arr[j] = 0;
			}
			
			//CHECK FOR DATA RECEIVED FROM UART 3
			while(check.checkForData3())
			{
				data = check.getData3();
				if (data != '\n')
				{
					arr[i++] = data;
				}
			}
			
			mem = i+1;
			i = 0;
			while(i < mem)
			{
				check.UARTsend(arr[i++]);
			}
			i = 0;
			check.UARTstring("\n",0);
		}
		
		if (check.checkForData3())
		{
			while(check.checkForData3())
			{
				data = check.getData3();
				arr[i++] = data;
			}
			mem = i+1;
			i = 0;
			while(i < mem)
			{
				check.UARTsend(arr[i++]);
			}
			i = 0;
			check.UARTstring("\n",0);
		}
		*/
    }
}

