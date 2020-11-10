
/*########### HOW TO USE THE HANDSHAKE OF NRF24L01+ #############
*
*This code is working as the RX & TX can talk at the same time.
*
*You send a PACKAGE through the function, the RX receive the package and sending
*back to the TX the package you LOAD in the RX function.
*
*The PACKET size is 6 Byte. You must set an arr of 6 Byte, others ways, it wont work
*
*In order for this code to work, you must include the "NRF24L01_Basic.h" & "NRF24L01_HandShake.h"
*to your code.
*
*You must define one controller as RX and one as TX using the "Set_RX_Mode()" OR "Set_TX_Mode()" function
*
*######################################## TX Functions ############################################
*the function of sending data is "Transmite_Payload(sendPayload)"(return True or False). The function to get what 
*the RX has sent to you is "getRXpacket()" (return a pointer of the arr).
*
*######################################## RX Functions ############################################
*The function of checking if there is a packet waiting is "PayloadAvailable()" (return True or False). 
*The function of receiving data is "Receive_Payload(sendAckPayload)" this function return a pointer of the arr
*that has been received.  in this function you put the arr that you like to send to the TX.
*NOTE: You must send an arr to the TX always, other way, it wont work.
*
*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ END $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
*/
#define F_CPU 16000000 
#include <avr/io.h>
#include "NRF24L01_Basic.h"
#include "NRF24L01_HandShake.h"
#include "UART.h"

int main()
{
	Nrf24l01_HandShake toggle;
	Uart NR;
	toggle.Set_RX_Mode();		//code for RX controller
	//toggle.Set_TX_Mode();		// remove the "//" for the TX controler
	unsigned char sendPayload[Payload_Size] = {0xEB, 0x12, 0x01, 0x00, 0x00, 0x88};	//arr to be send
	unsigned char receivedPayload[Payload_Size];		// arr for data receiving
	
	while(1)
	{
		/****************Code For RX Controller*******************/
		if(toggle.PayloadAvailable())
		{
			unsigned char *Payload = toggle.Receive_Payload(sendPayload);
			sendPayload[5]++;
			// Print on UART BAUD_9600 what it received//
			NR.sendNum(*Payload++);
			NR.sendNum(*Payload++);
			NR.sendNum(*Payload++);				//all the printing can be removed, it is just for verify that it is working
			NR.sendNum(*Payload++);
			NR.sendNum(*Payload++);
			NR.sendNum(*Payload++);
			NR.UARTstring("\n");
			//		END of Printing    //
		}
		/************END of Code For RX Controller****************/
		
		
		/****************Code For TX Controller*******************/
		/*																						//REMOVE the "\*" FOR the TX code
		if (toggle.Transmite_Payload(sendPayload))
		{
			sendPayload[3]++;
			unsigned char *pointer;
			pointer = toggle.getRXpacket();
			//###########Printing what he received from the RX (BAUD_9600)############
			NR.UARTstring("\nACK PACKET : ");
			for(int i = 0; i < 6; i++)
			{
				NR.sendNum(*pointer++);				//all the printing can be removed, it is just for verify that it is working
			}
			NR.UARTstring("\n");
			//###########END of Printing Printing data received from the RX############
		}
		*/																						//REMOVE the "*\" FOR the TX code
		/************END of Code For TX Controller****************/
					
	}
	return 0;
}
