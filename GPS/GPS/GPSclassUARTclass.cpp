/*
 * GccApplication2.cpp
 *
 * Created: 23/11/2017 17:28:01
 * Author : Adiel
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <math.h>

enum BaudRate{BAUD4800=4800,BAUD9600=9600,BAUD19200=19200,BAUD38400=38400,BAUD57600=57600};

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
		UCSR0B = (1<<RXEN0)|(1<<TXEN0);
		UCSR0C = (3<<UCSZ00);
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

class GPS : public Uart
{
private:
	Uart UartGps;
	char gpsData[90];
	char time[9];
	char speed[4];
	char latitude[14];
	char longitude[14];
	char latitudeLongitude[25];
	int _speedInKph, _degLatitude, _deglongitude;
	unsigned long int _longitude, _latitude; 
	int cmp(char *str1, const char *str2)
		{
			char count1 = 0, count2 = 0;
			while(*str1 != 0 || *str2 != 0)
			{
				if (*str1 == *str2)
				{
					str2++;
					str1++;
					count1++;
				}
				else
				{
					str2++;
					str1++;
				}
				count2++;
			}
			if (count2 == count1)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
	int read()
	{
		int count = 0;
		char dataIn = 0;
		while(dataIn !='$')
		{
			dataIn = UartGps.UARTreceive();
		}
		while(dataIn != ',')
		{
			dataIn = UartGps.UARTreceive();
			gpsData[count] = dataIn;
			count++;
		}
		count--;
		gpsData[count] = 0;
		if (cmp(gpsData,"GPRMC"))
		{
			count = 0;
			while(count < 89 && dataIn != '*')
			{
				dataIn = UartGps.UARTreceive();
				gpsData[count] = dataIn;
				count++;
			}
			gpsData[count] = 0;
		}
		else
		{
			return 0;
		}
		return 1;
	}
	int readGps()
	{
		for (char i = 0; i <= 30; i++)
		{
			if (read())
			{
				return 1;
			}
		}
		return 0;
	}
	int getPart(char part)//1= time---2&4=Latitude and Longitude-----6= speed
	{
		int count = 0;
		for (int i = 1; i <= part; i++)
		{
			count++;
			while(gpsData[count] != ',')
			{
				count++;
			}
		}
		return count+1;
	}
	int aountOfNumberTillThePoint(char part)
	{
		char counter = 0;
		int location = getPart(part);
		while(gpsData[location] != '.')
		{
			counter++;
			location++;
		}
		return counter;
	}
	int sizeOfChrechterInPart(char part)
	{
		char counter = 0;
		int location = getPart(part);
		
		while(gpsData[location] != ',')
		{
			counter++;
			location++;
		}
		return counter;
	}
	unsigned long int degCalculator(char min, unsigned long int sec)
	{
		unsigned long int divNum = 100;
		while (sec > divNum)
		{
			divNum *= 10;
		}
		double calculate = sec;
		unsigned long int result = 0;
		calculate /= divNum;
		calculate += min;
		calculate /= 60;
		calculate *= 10000000;
		result = calculate;
		//UartGps.printDecimalNumInAscii(result);
		return result;
	}
	char gpsSignal()
	{
		int index = getPart(1);
		if (gpsData[index] == 'V')
		{
			UartGps.UARTstring("No GPS signal\n");
			return 0;
		}
		else
		{
			return 1;
		}
	}
	void getLongitude()
	{
		int count = 0;
		int deg = 0, min = 0;
		unsigned long int sec = 0;
		
		count = getPart(4);
		char partSize = sizeOfChrechterInPart(4);
		char sizeToPoint = aountOfNumberTillThePoint(4);
		for (int i = count; i < (sizeToPoint + count - 2); i++)
		{
			deg *= 10;
			deg += gpsData[i] - '0';
		}
		_deglongitude = deg;
		for (int i = (count + (sizeToPoint - 2)); i < (sizeToPoint + count); i++)
		{
			min *= 10;
			min += gpsData[i] - '0';
		}
		for (int i = (count + sizeToPoint +1); i < (partSize + count)&&(sec<100000); i++)
		{
			sec *= 10;
			sec += gpsData[i] - '0';
		}
		unsigned long int passPointDeg = degCalculator(min,sec);
		_longitude = passPointDeg;
		longitude[11] = 0;
		for (int i = 10; i > 3; i--)
		{
			longitude[i] = (passPointDeg % 10) + '0';
			passPointDeg/=10;
		}
		longitude[3] = '.';
		for (int i = 2; i >= 0; i--)
		{
			longitude[i] = (deg % 10) + '0';
			deg/=10;
		}
	}
	void getLatitude()
		{
			int count = 0;
			int deg = 0, min = 0;
			unsigned long int sec = 0;
			
			count = getPart(2);
			char partSize = sizeOfChrechterInPart(2);
			char sizeToPoint = aountOfNumberTillThePoint(2);
			for (int i = count; i < (sizeToPoint + count - 2); i++)
			{
				deg *= 10;
				deg += gpsData[i] - '0';
			}
			_degLatitude = deg;
			for (int i = (count + (sizeToPoint - 2)); i < (sizeToPoint + count); i++)
			{
				min *= 10;
				min += gpsData[i] - '0';
			}
			for (int i = (count + sizeToPoint +1); i < (partSize + count)&&(sec<100000); i++)
			{
				sec *= 10;
				sec += gpsData[i] - '0';
			}
			unsigned long int passPointDeg = degCalculator(min,sec);
			_latitude = passPointDeg;
			latitude[11] = 0;
			for (int i = 10; i > 3; i--)
			{
				latitude[i] = (passPointDeg % 10) + '0';
				passPointDeg/=10;
			}
			latitude[3] = '.';
			for (int i = 2; i >= 0; i--)
			{
				latitude[i] = (deg % 10) + '0';
				deg/=10;
			}
		}
	void gpsSpeed()
	{
		int index = getPart(6);
		double speedInKnot = 0;
		int speedInKph = 0;
		char speedCharecterSize = 1;
		char sizeToPoint = aountOfNumberTillThePoint(6);
		for (int i = index; i < (sizeToPoint + index); i++)
		{
			speedInKnot *= 10;
			speedInKnot += gpsData[i] - '0';
		}
		speedInKnot *= 1.85;
		_speedInKph = speedInKph = speedInKnot;
		
		while(speedInKph>pow(10,(double)speedCharecterSize))
		{
			speedCharecterSize++;
		}
		
		speed[(int)speedCharecterSize] = 0;
		
		while(speedCharecterSize != 0)
		{
			speedCharecterSize--;
			speed[(int)speedCharecterSize] = speedInKph % 10 +'0';
			speedInKph /= 10;
		}
	}	
public:
	
	char *getLongitudeAndLatitude()
	{
		if (!readGps())
		{
			return 0;
		}
		else if(!gpsSignal())
		{
			return 0;
		}
		getLongitude();
		getLatitude();
		gpsSpeed();
		
		strcpy(latitudeLongitude,latitude);
		latitudeLongitude[strlen(latitude)] = ',';
		strcpy(latitudeLongitude+strlen(latitude)+1,longitude);
		return latitudeLongitude;
	}
	char *getTime()
	{
		int hour = 0;
		if (readGps())
		{
			for (int i = 0; i < 2; i++)
			{
				hour *= 10;
				hour += gpsData[i] - '0';
			}
			if (hour == 22)
			{
				hour = 0;
			}
			else if (hour == 23)
			{
				hour = 1;
			}
			time[0] = hour/10 + '0';
			time[1] = hour%10 + '0';
			time[2] = ':';
			time[3] = gpsData[2];
			time[4] = gpsData[3];
			time[5] = ':';
			time[6] = gpsData[4];
			time[7] = gpsData[5];
			time[8] = 0;
		}
		return time;
	}
	char *getSpeed()
	{
		if (!readGps())
		{
			return 0;
		}
		else if(!gpsSignal())
		{
			return 0;
		}
		return speed;
	}
	int getDegLatit()
	{
		return _degLatitude;
	}
	int getDegLongit()
	{
		return _deglongitude;
	}
	unsigned long int getLatit()
	{
		return _latitude;
	}
	unsigned long int getLongit()
	{
		return _longitude;
	}
	int getSpeedKph()
	{
		return _speedInKph;
	}
	
};
class MovedFromLocation
{
	private:
	Uart Move;
	GPS Location;
	unsigned long int _longitude, _latitude;
	int _degLongitude, _degLatitude;
	int _index, _indexReset;
	unsigned long int locationMemory[2][19];
	int _avrSum, _minDeviation;
	void updateLocation()
	{
		_degLatitude = Location.getDegLatit();
		_degLongitude = Location.getDegLongit();

		_latitude = Location.getLatit();
		_longitude = Location.getLongit();
	}
	void storeNewData()
	{
		if (_index >= 19)
		{
			_index = 0;
		}
		locationMemory[0][_index] = _latitude;
		locationMemory[1][_index] = _longitude;
		_index++;
	}
	//set the first Average in order to compare new income location data
	char firstAvrage()
	{
		static char firstLearning = 0;
		int arrSize = sizeof(locationMemory[0])/sizeof(unsigned long int);
		if (firstLearning)
		{
			return 1;
		}
		else
		{
			if (_index < (arrSize-1))
			{
				return 0;
			}
			_avrSum = avrageCalculator(0);
			_avrSum += avrageCalculator(1);
			_avrSum /= 2;
			firstLearning = 1;
			return 1;
		}
	}
	/********************************************************************/
	/*make average from the arr consider his size and return the average*/
	/********************************************************************/
	int avrageCalculator(int latitude0Longetude1)
		{
			int arrSize = sizeof(locationMemory[latitude0Longetude1])/sizeof(unsigned long int);
			int sum = 0;
			int currentValue = 0;
			for (int i = 0; i < (arrSize - 1); i++)
			{
				currentValue =  locationMemory[latitude0Longetude1][i] - locationMemory[latitude0Longetude1][i+1];
				currentValue = convertNegToPositive(currentValue);
				if (currentValue < 60000)
				{
					sum += currentValue;
				}
			}
			currentValue =  locationMemory[latitude0Longetude1][0] - locationMemory[latitude0Longetude1][arrSize - 1];
			currentValue = convertNegToPositive(currentValue);
			if (currentValue < 60000)
			{
				sum += currentValue;
			}
			return (sum/(arrSize));
		}
		/**************************************************************/
		/*this function get the average of the latitude & Longitude   */
		/*from avrageCalculator(), function while latitude =0 and     */
		/*Longitude=1,and check if it is exceed from average of arr+50%*/
		/*this function return 1 if fail, 0 if OK                     */
		/**************************************************************/
	int checkAvrageOfMovment()
	{
		int avr = 0;
		avr = avrageCalculator(0);
		avr += avrageCalculator(1);
		avr /= 2;
		_avrSum += avr;
		_avrSum /= 2;
		Move.UARTstring("Now Average is: ");//*************************remove
		Move.printDecimalNumInAscii(avr);
		Move.UARTstring("\n");
		if (avr > (_avrSum + (_avrSum/2)))
		{
			return avr;                   //souled be "return 1; //return avr is just for test
		}
		else
		{
			return 0;
		}
		return 0;
	}
	int convertNegToPositive(int negetiveOrPositive)
	{
		if (negetiveOrPositive < 0)
		{
			return negetiveOrPositive *= -1;
		}
		else
		{
			return negetiveOrPositive;
		}
	}
	/*****************************************************************************************/
	/*this function set average of 10 latitude & longitude values and keep it.               */
	/*this function check if there is a different between memory location & new location.    */
	/*                                                                                       */
	/*if one of the next fail it would send a massage of movement, and reset the value.      */
	/*1. the speed is over 70 Kph.                                                           */
	/*2. the current deg value is different from memory.                                     */
	/*3. value of latitude & longitude is different from memory+50%(tolerance) of average.   */
	/*4. if fail to succeed clause 3 more then 15 times.                                     */
	/*5. average not competed, and compare action took place.                                */
	/*****************************************************************************************/
	int memoriesLastGoodLocationOrCheckSignalInteruption(int speed = 1, int checkCall = 0)
	{
		static unsigned long int latitude = 0, longitude = 0;
		static int degLatitude = 0, degLongitude = 0;
		static char indexMemory = 0, tryTimes = 0, stop = 0;
		
		//if checkCall = 1, it will compare data, else it will update data if it is not up to date
		if (checkCall)
		{
			if (speed > 70 || latitude == 0 || longitude == 0)
			{
				indexMemory = 0;
				return 1;
			}
			Move.UARTstring("Chance number:");//*******************************remove
			Move.printDecimalNumInAscii(tryTimes);
			Move.UARTstring(" !/n");
			if (tryTimes <= 15)
			{
				Move.UARTstring("part 1");//*****************************
				if (convertNegToPositive(_latitude - latitude) > (_avrSum + _avrSum/2) || convertNegToPositive(_longitude- longitude) > (_avrSum+_avrSum/2))
				{
					Move.UARTstring("part 2");//***********************
					if (convertNegToPositive(degLatitude - _degLatitude) > 1 || convertNegToPositive(degLongitude - _degLongitude) > 1)
					{
						Move.UARTstring("part 3");//*****************
						stop = 0;
						indexMemory = 0;
						latitude = 0;
						longitude = 0;
						return 1;
					}
					Move.UARTstring("part 4");//*******************	*
					tryTimes++;
					return 0;
				}
				Move.UARTstring("part 5");//****************************
				return 0;
			}
			stop = 0;
			indexMemory = 0;
			latitude = 0;
			longitude = 0;
			return 1;
		} 
		else
		{
			tryTimes = 0;
			
			if (stop) //if stop = 1, the data is up to date
			{
				return 0;
			}
			else if (speed <= 1)        //data will be updated only if speed is 1Kph or lower.
			{                           //other way it will start over the process
				degLatitude = _degLatitude;
				degLongitude = _degLongitude;
				if (indexMemory <= 10)
				{
					latitude += _latitude;
					longitude += _longitude;
					indexMemory++;
				}
				else
				{
					latitude /= 10;
					longitude /= 10;
					stop = 1;
					Move.UARTstring("Compare Data is accomplished!\n");//*******************remove
					Move.printDecimalNumInAscii(latitude);
					Move.printDecimalNumInAscii(longitude);
				}
			}
			else
			{
				degLatitude = 0;
				degLongitude = 0;
				latitude = 0;
				longitude = 0;
				indexMemory = 0;
			}
		}
		return 0;
	}
	
	public:
		MovedFromLocation()
		{
			_index =  _avrSum = 0;
			_degLongitude = _degLatitude = 0;
			_longitude = _latitude = 0;
		}
	char *getLongitudeAndLatitude()
	{
		return Location.getLongitudeAndLatitude();
	}
	char *getGoogleMapsLink()
	{
	    Move.UARTstring("www.google.co.il/maps/place/",0);
	    return getLongitudeAndLatitude();
	}
    /********************************************************************************/
    /*this function check if there is a movement by setting an average of a location*/
    /*and checking if there is a difference between the arr average and the location*/
    /*that just received.                                                           */
    /*it is checking if the speed of GPS is bigger then 2 Kph, if so it will        */
    /*compare the current location with the memory stored location.                 */
    /********************************************************************************/
    //return 1 for car moving, 0 if it is not moving
	char checkIfMoved()
	{
		int speed = 0, average = 0; 
		//check if there is any data or maybe there is no signal. NULL = no data
		if (Location.getLongitudeAndLatitude())
		{
		    
			speed = Location.getSpeedKph();
			if (speed > 2)
			{
				Move.UARTstring("Check Speed Problem... ");//*****************************************remove
				//checking if the location that stored in the memory is the same one from 
				//the "Location.getLongitudeAndLatitude()", if not, you have 15 chances to 
				//pull it together! other way, a movement massage will be sent.
				return memoriesLastGoodLocationOrCheckSignalInteruption(speed,1);
			}
			updateLocation(); //updating the new "Location.getLongitudeAndLatitude()" data
			storeNewData();    //move the new value into the average arr
			Move.UARTstring("Average Movement: ");
			Move.printDecimalNumInAscii(_avrSum);
			Move.UARTstring("Speed:");
			Move.printDecimalNumInAscii(speed);
			//before any data is saved, we are calculating the first average witch is the arr
			if (firstAvrage())
			{
				average = checkAvrageOfMovment(); //temporary, just for test. 
			}
			if(average)			//original = if(checkAvrageOfMovment())	
			{
				Move.UARTstring("Check Average Problem... ");
				return memoriesLastGoodLocationOrCheckSignalInteruption(speed,1);
			}
			else
			{
				memoriesLastGoodLocationOrCheckSignalInteruption(speed);
				return 0;
			}
		
		}
		return 0;
	}
};

int main(void)
{	
	Uart MyUART;
	GPS MyGPS;
	MovedFromLocation checkMove;
	MyUART.setBaudRate(BAUD9600);
	MyUART.UARTstring("hello world, Port 9");
	char *ptr1, *ptr2;
	
	//unsigned char *ptrl = "Good";
	MovedFromLocation a;
    while (1) 
    {
		/*
		MyUART.UARTstring("good morning");
		MyUART.UARTsend(0);
		MyUART.UARTstring("Looks Like working");
		MyUART.UARTsend(0);
		
		*/
		//MyUART.UARTstring("AT",0);
		//MyUART.UARTacko();
		
		if (a.checkIfMoved())
		{
			MyUART.UARTstring("The car is MOVING!!!\n");
		}
		else
		{
			MyUART.UARTstring("Car is OK!\n");
		}
		MyUART.UARTstring("https://www.google.co.il/maps/place/",0);
		ptr2 = MyGPS.getLongitudeAndLatitude();
		MyUART.UARTstring(ptr2,0);
		MyUART.UARTstring("\n",0);
		
		/*
		ptr1 = MyGPS.getTime();
		MyUART.UARTstring(",",0);
		ptr1 = MyGPS.getLongitude();
		MyUART.UARTstring(ptr1,0);
		MyUART.UARTstring("\n");
		
		*/
		
    }
  
}




