/*
 * bluetooth_AT_09.h
 *
 * Created: 14/07/2018 14:33:40
 *  Author: Adiel
 */ 


#ifndef BLUETOOTH_AT_09_H_
#define BLUETOOTH_AT_09_H_

class bluetooth: public Uart
{
	private:
	char ackPacket[151];
	void setup();
	char sendCommand(const char *cmd);
	bool checkForOK();
	const int dataIsolator(const char *ackData);
	public:
	bool setDeviceName(const char *name);
	bool setDeviceAsSlave();
	bool setDeviceAsMaster();
	bool setDeviceBaudRate(int baud0_4 = 4);
	bool getDeviceMACaddress(char *MAC);
	bool connectToDevice(const char *MAC);
	bool setNewPassword(const char *password);
	void setDeviceBackToDefault();
	bool getData(char *dataIn);
	void sendData(char *dataOut);
	bluetooth();
	~bluetooth();
};



#endif /* BLUETOOTH_AT_09_H_ */