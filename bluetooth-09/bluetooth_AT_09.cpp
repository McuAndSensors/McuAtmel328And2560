/*
 * bluetooth_AT_09.cpp
 *
 * Created: 14/07/2018 14:33:12
 *  Author: Adiel
 */ 
#define F_CPU 16000000UL
#include "UART.h"
#include "bluetooth_AT_09.h"
#include <string.h>
#include <util/delay.h>

	char ackPacket[151];
	void bluetooth::setup(){
		setBaudRate(BAUD9600);
	}
	char bluetooth::sendCommand(const char *cmd){
		int counter = 0;
		
		//a little delay between command due to slow respond of the device
		_delay_ms(100);
		
		UARTstring(cmd,0); //sending the command by UART
		UARTstring("\r\n");	//sending ENTER, the device must get enter in the end of the string
		
		//here we set the arr back to NULL to avoid rereading the data if new data is not received
		ackPacket[0] = '\0';
		while(checkForData())
		{
			//make sure that there is no sliding from defined size
			if (counter < 150)
			{
				ackPacket[counter++] = UARTreceive();
				ackPacket[counter+1] = '\0';
			}
			else
			{
				break;
			}
		}
		return 1;
	}
	bool bluetooth::checkForOK(){
		int counter = 0;
		UARTstring(ackPacket);
		while(counter < 70 && ackPacket[counter] != '\0')
		{
			if ((ackPacket[counter] == 'O' && ackPacket[counter+1] == 'K')|| ackPacket[counter] == 'K')
			{
				return true;
			}
			counter++;
		}
		return false;
	}
	const int bluetooth::dataIsolator(const char *ackData){
		int counter = 0;
		if (ackData[counter] != '\0')
		{
			while(counter < 10 && ackData[counter] != '=')
			{
				counter++;
			}
		}
		return counter;
	}
	bool bluetooth::setDeviceName(const char *name){
		char arrName[50] = "AT+NAME";
		strcpy(arrName+7,name);
		sendCommand(arrName);
		return checkForOK();
	}
	bool bluetooth::setDeviceAsSlave(){
		sendCommand("AT+ROLE0");
		return checkForOK();
	}
	bool bluetooth::setDeviceAsMaster(){
		sendCommand("AT+ROLE1");
		return checkForOK();
	}
	bool bluetooth::setDeviceBaudRate(int baud0_4){
		char cmd[9] = "AT+BAUD";
		cmd[7] = baud0_4+'0';
		sendCommand(cmd);
		return checkForOK();
	}
	bool bluetooth::getDeviceMACaddress(char *MAC){
		int index = 0;
		sendCommand("AT+LADDR");
		index = dataIsolator(ackPacket);
		strcpy(MAC,ackPacket+index);
		if (index > 0)
		{
			return true;
		}
		return false;
	}
	bool bluetooth::connectToDevice(const char *MAC){
		char connectCmd[25] = "AT+CONA";
		strcpy(connectCmd+7,MAC);
		sendCommand(connectCmd);
		return checkForOK();
	}
	bool bluetooth::setNewPassword(const char *password){
		char newPassword[17] = "AT+PIN";
		strcpy(newPassword+6,password);
		sendCommand(newPassword);
		return checkForOK();
	}
	void bluetooth::setDeviceBackToDefault(){
		setNewPassword("123456");
		setDeviceName("?");
		setDeviceAsSlave();
		setDeviceBaudRate();
	}
	bool bluetooth::getData(char *dataIn){
		//are size mast be 20 sells
		int counter = 0;
		while(checkForData3())
		{
			//make sure that there is no sliding from defined size
			if (counter < 20)
			{
				dataIn[counter++] = UARTreceive3();
				dataIn[counter+1] = '\0';
			}
			else
			{
				break;
			}
		}
		if (counter > 0)
		{
			return true;
		}
		return false;
	}
	void bluetooth::sendData(char *dataOut){
		UARTstring(dataOut);
	}
	bluetooth::bluetooth(){
		setup();
	}
	bluetooth::~bluetooth(){}