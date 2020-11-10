/*
 * FingerPrint.cpp
 *
 * Created: 14/12/2017 10:07:07
 * Author : Adiel
 */ 
#define  F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
#include <avr/interrupt.h>

enum BaudRate{BAUD4800=4800,BAUD9600=9600,BAUD19200=19200,BAUD38400=38400,BAUD57600=57600};
enum commands {GenImage = 0x01,Img2Tz ,Match,Serach,RegModel,Store,LoadChar,UpChar,
	DownChar,UpImg,DownImg,DelChar,Empty,SetSysPara,ReadSysPara,
	SetPwd = 0x12,VfyPwd,GetRandomCode,SetAdder,Control = 0x17,WriteNotePad,
	ReadNotPad,HiSpeedSearch = 0x1B,TemplateNum = 0x1D};
enum packType {command = 0x01, dataPack,ackPack = 7, endDataPack};

class Uart
{
	private:
	
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
		//UCSR0C = ((1<<UCSZ00)|(1<<UCSZ01));
		UCSR0C = (3<<UCSZ00);
	}
	char checkForData()
	{
		if (UCSR0A & (1<<RXC0))
		{
			return 1;
		}
		return 0;
	}
	char getData()
	{
		return UDR0;
	}
	unsigned char UARTreceive(void)
	{
		while(!(UCSR0A & (1<<RXC0)));
		return UDR0;
	}
	unsigned char UARTreceiveString(void)
	{
		int i = 0;
		unsigned char Str[20];
		
		while(UARTreceive() == 0)
		{
			Str[i] = UARTreceive();
			i++;
		}
		return *Str;
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
/*
	00h: commad execution complete;
	01h: error when receiving data package;
	02h: no finger on the sensor;
	03h: fail to enroll the finger;
	06h: fail to generate character file due to the over-disorderly fingerprint image;
	07h: fail to generate character file due to lackness of character point or over-smallness of fingerprint image
	08h: finger doesn’t match;
	09h: fail to find the matching finger;
	0Ah: fail to combine the character files;
	0Bh: addressing PageID is beyond the finger library;
	0Ch: error when reading template from library or the template is invalid;
	0Dh: error when uploading template;
	0Eh: Module can’t receive the following data packages.
	0Fh: error when uploading image;
	10h: fail to delete the template;
	11h: fail to clear finger library;
	13h: wrong password!
	15h: fail to generate the image for the lackness of valid primary image;
	18h: error when writing flash;
	19h: No definition error;
	1Ah: invalid register number;
	1Bh: incorrect configuration of register;
	1Ch: wrong notepad page number;
	1Dh: fail to operate the communication port;
*/
class FingerPrint
{
private:
	Uart FingerPrintUart;
	char _dataPackage[45];
	char _ackPacket[150];
	char _length;
	void setHeader(char FirstByte = 0xEF, char SeconedByte = 0x01)
	{
		_dataPackage[0] = FirstByte;
		_dataPackage[1] = SeconedByte;
	}
	void SetAddress(unsigned long int addressVal = 0xFFFFFFFF)
	{
		char shift = 24;
		for (int i = 2; i < 6; i++)
		{
			_dataPackage[i] = addressVal >> shift;
			shift -= 8;
		}
	}
	void SetPassword(unsigned long int pwd = 0x00)
	{
		_dataPackage[10] = pwd >> 24;
		_dataPackage[11] = pwd >> 16;
		_dataPackage[12] = pwd >> 8;
		_dataPackage[13] = pwd;
	}
	void setPackagePID(char PID = command)
	{
		//01H = command;  02H = DATA packet; 07H = Ack packet; 08H = End of DATA packet
		_dataPackage[6] = PID;
	}
	void setPacketLength(char length = 0x07)
	{
		//Refers to the length of package content (command packets and data packets)
		//plus the length of Checksum( 2 bytes)   !!!MAX is 256 Byte!!!
		/**** {07H} = 4byte Password, 1Byte IndecationCode (command), 2Byte CheckSum ****/
		_dataPackage[7] = 0;
		_dataPackage[8] = length;
	}
	void setAcommand(commands commandValue)
	{
		_dataPackage[9] = commandValue;
	}
	void getAckPackage(char ackPackSize = 12)
	{
		int ackDataIndex = 0;
		char ackData = 0;
		while(ackDataIndex < ackPackSize)
		{
			/*********************************************************************/
			/*
			-purples, need to avoid any data before the 0xEF Head start data value
			while(ackData != 0xEF)
			{
				ackData = FingerPrintUart.UARTreceive();
				_ackPacket[0] = ackData;
			}
			ackDataIndex++;
			*/
			/********************************need to test************************/
			//check if there is data, if so, receive the data
			//-This avoid the program from endless loop in wait for data
			if (FingerPrintUart.checkForData())
			{
				ackData = FingerPrintUart.getData();
				_ackPacket[ackDataIndex] = ackData;
				ackDataIndex++;
			}
		}
		for (int i = 0; i<ackPackSize;i++)
		{
			FingerPrintUart.UARTsend(_ackPacket[i]);
		}
	}
public:
	FingerPrint()
	{
		FingerPrintUart.setBaudRate(BAUD57600);
		setHeader();
		SetAddress();
		setup();
	}
	~FingerPrint(){}
	void setCheckSum(int sum, int index)
	{
		_dataPackage[index] = sum >> 8;
		_dataPackage[index + 1] = sum;	
	}
	bool verifyPassword()
	{
		setPackagePID(command);				//package type
		setPacketLength(7);					//package size after address in Byte
		_dataPackage[9] = VfyPwd;			//setAcommand
		SetPassword();						//enter value, defult = 0x00000000
		setCheckSum(VfyPwd+command+0x07,14);//calculate sum of data after address
		sendAndGetAck(VfyPwd); //enter size of package to be sent
		return checkAckPackError(VfyPwd);
	}
	void setSystem(char parameterNum, char content)
	{
		//filing _dataPacket arr, index 0 to 5 is filled automatic by class contractor
		setPackagePID(command);   //1 = command  ;index 6
		setPacketLength(5);				//index 7,8
		setAcommand(SetSysPara);		//index 9
		_dataPackage[10] = parameterNum;
		_dataPackage[11] = content;
		setCheckSum(5+SetSysPara+parameterNum+content,12); //index 12,13
	}
	//need to test function
	char *getFingerImg()
	{
		//code not fully complete
			setPackagePID(command);				//1 = command  ;index 6
			setPacketLength(0x03);				//index 7,8
			setAcommand(GenImage);				//index 9
			setCheckSum(command+0x03+GenImage,10);		//index 10, 11
			sendAndGetAck(GenImage);
			if (checkAckPackError(GenImage))
			{
			//	if (updateFinger())
				{
					for (int i = 0; i < 103; i++)
					{
						FingerPrintUart.UARTreceive();
					}
					for (int i = 0; i < 103; i++)
					{
						FingerPrintUart.UARTsend(_ackPacket[i]);
					}
					return _ackPacket;
				}
			}
			return 0;
}
	void blinkTheSensor(char blinkTimes = 1)
	{
		for(int i = 0; i < blinkTimes; i++)
		{
			_delay_ms(1500);
			setPackagePID(command);						//1 = command  ;index 6
			setPacketLength(0x03);						//index 7,8
			setAcommand(GenImage);						//index 9
			setCheckSum(command+0x03+GenImage,10);		//index 10, 11
			sendAndGetAck(GenImage);
		}
	}
	bool combineTwofingersIntoOne()
	{
		setPacketLength(0x03);								//index 7,8
		setAcommand(RegModel);								//index 9
		setCheckSum(command+0x03+RegModel,13);	//index 10, 11
		sendAndGetAck(RegModel);
		return checkAckPackError(RegModel);
	}
	bool setNewFinger(char fingerID = 0x01)
	{
		if (genFingerImg())
		{
			blinkTheSensor(3);
			if (genFingerImg(0x02))
			{
				if (combineTwofingersIntoOne())
				{
					setPacketLength(0x06);								//index 7,8
					setAcommand(Store);									//index 9
					_dataPackage[10] = 0x01;							//buffer ID
					_dataPackage[11] = 0x00;							//page ID msByte
					_dataPackage[12] = fingerID;						//page ID lsByte
					setCheckSum(command+0x06+Store+fingerID+0x01,13);	//index 13, 14
					sendAndGetAck(Store);
					if (checkAckPackError(Store))
					{
						blinkTheSensor(5);
						return 1;
					}
				}
			}
		}
		return 0;
	}
	bool genFingerImg(char bufferID = 0x01)
	{
		setPackagePID(command);						//1 = command  ;index 6
		setPacketLength(0x03);						//index 7,8
		setAcommand(GenImage);						//index 9
		setCheckSum(command+0x03+GenImage,10);		//index 10, 11
		sendAndGetAck(GenImage);
		if (checkAckPackError(GenImage))
		{
			setPacketLength(0x04);						//index 7,8
			setAcommand(Img2Tz);						//index 9
			_dataPackage[10] = bufferID;				//buffer ID
			setCheckSum(command+0x04+Img2Tz+0x01,11);	//index 10, 11
			sendAndGetAck(Img2Tz);
			if (checkAckPackError(Img2Tz))
			{
					return 1;
			}
		}
		return 0;
	}
	char searchFinger()
	{
		if (genFingerImg())
		{
			setPacketLength(0x08);							//index 7,8
			setAcommand(Serach);							//index 9
			_dataPackage[10] = 0x01;						//buffer ID
			_dataPackage[11] = 0x00;						//page ID msByte
			_dataPackage[12] = 0x00;						//page ID lsByte
			_dataPackage[13] = 0x00;						//page ID msByte
			_dataPackage[14] = 0xFF;						//page ID lsByte
			setCheckSum(command+0x08+Serach+0x01+0xFF,15);	//index 10, 11
			sendAndGetAck(Serach);
			if (checkAckPackError(Serach))
			{
				return _ackPacket[11];
			}
		}
	return 0;
	}
	void sendPackage(int size)
{
	for (int i = 0; i < size; i++)
	{
		FingerPrintUart.UARTsend(_dataPackage[i]);
	}
}
	char findPackSize(int cmdType, char packSendOrRecieve)
	 {
		 //packSendOrRecieve ; 1 send; 0 recieve
		 switch (cmdType)
		 {
			 /****************value of ack receive package is 12****************/
			 case VfyPwd:{packSendOrRecieve == 1? cmdType = 16 : cmdType = 12; break;}
			 case SetAdder:{packSendOrRecieve == 1? cmdType = 16 : cmdType = 12; break;}
			 case SetSysPara:{packSendOrRecieve == 1? cmdType = 14 : cmdType = 12; break;}
			 case Control:{packSendOrRecieve == 1? cmdType = 13 : cmdType = 12; break;}
			 case GenImage:{packSendOrRecieve == 1? cmdType = 12 : cmdType = 12; break;}
			 case UpImg:{packSendOrRecieve == 1? cmdType = 12 : cmdType = 12; break;}
			 case DownImg:{packSendOrRecieve == 1? cmdType = 12 : cmdType = 12; break;}
			 case Img2Tz:{packSendOrRecieve == 1? cmdType = 13 : cmdType = 12; break;}
			 case RegModel:{packSendOrRecieve == 1? cmdType = 12 : cmdType = 12; break;}
			 case UpChar:{packSendOrRecieve == 1? cmdType = 13 : cmdType = 12; break;}
			 case Store:{packSendOrRecieve == 1? cmdType = 15 : cmdType = 12; break;}
			 case LoadChar:{packSendOrRecieve == 1? cmdType = 15 : cmdType = 12; break;}
			 case DelChar:{packSendOrRecieve == 1? cmdType = 16 : cmdType = 12; break;}
			 case Empty:{packSendOrRecieve == 1? cmdType = 12 : cmdType = 12; break;}
			 case WriteNotePad:{packSendOrRecieve == 1? cmdType = 45 : cmdType = 12; break;}
			 /****************value of ack receive package is 11****************/
			 case SetPwd:{if(packSendOrRecieve) cmdType = 16;else cmdType = 11;break;}
			 /****************value of ack receive package is 26****************/
			 case ReadSysPara: {packSendOrRecieve == 1? cmdType = 12 : cmdType = 26; break;}
			 /****************value of ack receive package is 14****************/
			 case Match:{packSendOrRecieve == 1? cmdType = 12 : cmdType = 14; break;}
			 case TemplateNum:{packSendOrRecieve == 1? cmdType = 12 : cmdType = 14; break;}
			 /****************value of ack receive package is 16****************/
			 case GetRandomCode:{packSendOrRecieve == 1? cmdType = 12 : cmdType = 16; break;}
			 case Serach:{packSendOrRecieve == 1? cmdType = 17 : cmdType = 16; break;}
			 /****************value of ack receive package is 44****************/
			 case ReadNotPad:{packSendOrRecieve == 1? cmdType = 13 : cmdType = 44; break;}
		 }
		 return cmdType;
	 }
	char getAckPackConfirmCodeIndex(char commandVal)
	{
		char packSize = findPackSize(commandVal,0);
		char confIndex = 0;
		
		//get confirmation code index value
		if (commandVal == ReadNotPad)
		{
			confIndex = packSize - 34;
		}
		else if (commandVal == GetRandomCode || commandVal == Serach)
		{
			confIndex = packSize - 6;
		} 
		else if (commandVal == Match || commandVal == TemplateNum)
		{
			confIndex = packSize - 4;
		}
		else if(commandVal == ReadSysPara)
		{
			confIndex = packSize - 18;
		}
		else
		{
			confIndex = packSize - 2;
		}
		for (int i = 0; i < packSize;i++)
		{
			while(_ackPacket[i] != 0xEF && i < packSize)
			{
				i++;
			}
			
			//return confirmation code index
			return i+confIndex;
		}
		return confIndex;
	}
	bool reSendPack(int confIndex, int cmd ,int reTryTimes = 1)
	{
		for (int i = 0; i < reTryTimes; i++)
		{
			sendAndGetAck(cmd);
			if (_ackPacket[confIndex] == 0)
			{
				return 1;
			}
		}
		return 0;
	}
	bool checkAckPackError(char cmd)
	{
		int confCodeIndex = (int)getAckPackConfirmCodeIndex(cmd);
		char confCode = _ackPacket[confCodeIndex - 1];
		switch(confCode)
		{
			case 0x00: return 1;
			case 0x01: return reSendPack(confCodeIndex,cmd,5);
			case 0x02: return 0;
			case 0x06: return reSendPack(confCodeIndex,cmd,3);
			case 0x07: return reSendPack(confCodeIndex,cmd,4);
			case 0x09: 
			case 0x0A: 
			case 0x0C: 
			case 0x0D: 
			case 0x0F: 
			case 0x15: 
			case 0x18: return reSendPack(confCodeIndex,cmd,5);
			case 0x19: return 1;
		}
		return 0;
	}
	void sendAndGetAck(int cmd)
	{
		sendPackage(findPackSize(cmd,1));
		getAckPackage(findPackSize(cmd,0));
	}
	bool writeToNotePad(const char arr[],char pageNum_0_To_15 = 0)
	{
		int sumVal = 0;
		setPacketLength(36);							//index 7,8
		setAcommand(WriteNotePad);						//index 9
		_dataPackage[10] = pageNum_0_To_15;				//index 10 page number
		
		for(int i = 0; i < 32; i++)
		{
			_dataPackage[i+11] = arr[i];
			sumVal += arr[i];
		}
		setCheckSum(command+36+WriteNotePad+pageNum_0_To_15+sumVal,43);	//index 43, 44
		sendAndGetAck(WriteNotePad);
		return checkAckPackError(WriteNotePad);
	}
	char *readFromNotPad(char pageNum_0_To_15 = 0)
	{
		setPacketLength(0x04);										//index 7,8
		setAcommand(ReadNotPad);									//index 9
		_dataPackage[10] = pageNum_0_To_15;							//Page Number
		setCheckSum(command+0x04+ReadNotPad+pageNum_0_To_15,11);	//index 11, 12
		sendAndGetAck(ReadNotPad);
		if (!checkAckPackError(ReadNotPad))
		{
			return _ackPacket+10;
		}
		return 0;
	}
	bool deleteAllFingers()
	{
		setPackagePID(command);					//1 = command  ;index 6
		setPacketLength(0x03);					//index 7,8
		setAcommand(Empty);						//index 9
		setCheckSum(command+0x03+Empty,10);		//index 10, 11
		sendAndGetAck(Empty);
		return checkAckPackError(Empty);
	}
	char *readSysProperties()
	{
		setPackagePID(command);						//1 = command  ;index 6
		setPacketLength(0x03);						//index 7,8
		setAcommand(ReadSysPara);					//index 9
		setCheckSum(command+0x03+ReadSysPara,10);	//index 10, 11
		sendAndGetAck(ReadSysPara);
		if (!checkAckPackError(ReadSysPara))
		{
			return _ackPacket+10;
		}
		return 0;
	}
	//need to test function
	bool delFinger(char fingerID)
	{
		setPackagePID(command);									//1 = command  ;index 6
		setPacketLength(0x07);									//index 7,8
		setAcommand(DelChar);									//index 9
		_ackPacket[10] = 0x00;									//lsByte fingerID
		_ackPacket[11] = fingerID;								//msByte fingerID
		_ackPacket[12] = 0x01;									//how many fingers to delete
		setCheckSum(command+0x07+DelChar+fingerID+0x01,13);		//index 13, 14
		sendAndGetAck(DelChar);
		return checkAckPackError(DelChar);
	}
	void setup(BaudRate baud = BAUD57600, char securityLevel = 1, char packMaxLength = 0)
	{
		_delay_ms(220);
		verifyPassword();
			
		setSystem(4,baud/9600); //set baud rate
		sendAndGetAck(SetSysPara);
		checkAckPackError(SetSysPara);
			
		setSystem(5,securityLevel); //set the level of compare fingerprint 1 to 5, while 1 is highest
		sendAndGetAck(SetSysPara);
		checkAckPackError(SetSysPara);
			
		setSystem(6,packMaxLength); //length 0=32, 1=64, 2=128, 3=256 (Byte)
		sendAndGetAck(SetSysPara);
		checkAckPackError(SetSysPara);	
	}
};
int main(void)
{
	Uart a;
	a.setBaudRate(BAUD57600);
	FingerPrint Test;
	char id = 0;
	while(!Test.setNewFinger(4));
    while (1) 
    {
		_delay_ms(100);
		//Test.setNewFinger();
		id = Test.searchFinger();
		if (id)
		{
			a.UARTstring("The Finger Id is:");
			a.sendNum(id);
		}
    }
	/********************************TEST************************************/
	/*
				if(checkAckPackError(VfyPwd))
				{
					for (int i = 0; i < 10; i++)
					{
						FingerPrintUart.UARTsend(0x11);
					}
				}
	*/
	/*************************************************************************/
	
}

