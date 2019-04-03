/*********************************************************************************************/
/*
 * BG96 NB-IoT Arduino library V 2.0
 * Created by Manuel Montenegro, April 03, 2019.
 * Developed for MOTAM proyect. 
 * 
 *  This library manages communication with BG96 NB-IoT Module.
 *
 *	Compatible boards with this library: Arduino/Genuino 101 with AVNET BG96 Shield.
*/
/*********************************************************************************************/

#ifndef __BG96NBIOT_H__
#define __BG96NBIOT_H__


#include "Arduino.h"

// #define DEBUGMODE						// UNCOMMENT THIS LINE FOR DEBUG MESSAGES BY SERIAL PORT

#define DEBUG 			Serial			// Serial port for comunnication by USB
#define BG96 			Serial1			// Serial port for comunnication with NB-IoT module
#define networkOperator "21401"			// Vodafone's network operator code
#define LOCAL_PORT		16000			// Local port for internet connection


class Bg96NbIot {
public:
	Bg96NbIot ();
	bool begin ();						// Init serial port and register the module in the network
	String getIP();						// Return IP of BG96 module
	String getIMEI();					// Return IMEI of card inserted in SODAQ module
	bool openSocket (String sAddress, int sPort);	// Open TCP socket
	bool openSslSocket (String sAdrress, int sPort);// Open SSL socket
	bool sendData (String data);		// Send data by socket connection
	bool sendDataBySsl (String data);	// Send data by SSL socket connection
	String receiveData (); 				// Return received data from network
	String receiveDatabySsl ();			// Return received data from network by SSL socket
	bool shutdown ();					// Shutdown the module

private:
	String ip;							// IP that network gives to BG96 module
	String imei;						// IMEI of SIM card inserted in SODAQ module
	uint8_t socket;						// BG96 socket currently opened

	bool setUpBg96 ( );					// Start sending AT commands for module configuration

	bool isAlive ();					// Check if BG96 module responds to commands
	bool confNbiotConnection ();		// Configure parameters for NB-IoT connection
	bool networkRegistration ();		// Select the operator and register SIM in network
	bool askForIp ();					// Take IP of BG96 module from the network
	bool askForImei ();					// Take IMEI's SIM from BG96 module

	bool checkRespForOk (int timeOut);	// Check if OK is in the response from BG96 module 
	bool checkRespForReg ();			// Check response looking for network registration
	bool checkRespForOpSelection ();	// Check response looking for Ok or error on operator selection
	String checkRespForIp();			// Check response looking for IP given by network
	bool checkRespForSocket();			// Check response looking for socket opened
	bool checkRespForSslSocket();		// Check response looking for SSL socket opened
	String checkRespForImei();			// Check response looking for IMEI
	bool checkRespIfDataSent();			// Check if data was sent
	bool checkRespIfDataReceived();		// Return if there are data received from host Server
	bool checkRespIfDataReceivedBySsl();// Return if there are data received from SSL socket from host Server 
	String checkRespForHostResponse();	// Check response looking for data received from host server
	String checkRespForHostResponseBySsl();	// Check response looking for data received from host server by SSL socket
	bool checkRespIfInputDataChar();	// Check response for '>' character: module is waiting for data
	bool checkRespForPoweredDown ();	// Check response for powered down message

	void sendIt (String atCommand);		// Send an AT command to BG96 module
	String receiveIt ();				// Receive data sended by BG96 module
	void printIt (String text);			// Print a string by debug serial port
	String stringToHexString(String str);	// Conversion from string to hexadecimal string
};


#endif