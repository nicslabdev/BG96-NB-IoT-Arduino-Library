/*********************************************************************************************/
/*
 * BG96 NB-IoT Arduino library V 2.1
 * Created by Manuel Montenegro, April 22, 2019.
 * Developed for MOTAM proyect. 
 * 
 *  This library manages communication with BG96 NB-IoT Module.
 *
 *	Compatible boards with this library: Arduino/Genuino 101 with AVNET BG96 Shield.
*/
/*********************************************************************************************/

#include <Bg96NbIot.h>

Bg96NbIot::Bg96NbIot ()
{ 
	ip = "";
	imei = "";
	socket = 0;
}


// Start BG96 module
bool Bg96NbIot::begin ()
{
	BG96.begin(9600);					// Start serial port with BG96. AT+IPR selects the baudrate of module
	DEBUG.begin(115200);				// Start serial port for debugging

	while(!BG96);						// Wait until BG96's serial connection is stablished

	return setUpBg96 ( );				// Start the setting up of BG96 module
}

// Return the IP that network assigns to the NB-IoT module
String Bg96NbIot::getIP() 
{
	return ip;
}

// Return the IP that network assigns to the NB-IoT module
String Bg96NbIot::getIMEI() 
{
	return imei;
}


// Open a TCP socket. Return true if correct.
bool Bg96NbIot::openSocket (String sAddress, int sPort)
{
	bool flag = true;					// Flag for checking correct working of AT commands

	String atCommand;

	if (flag) 
	{
		atCommand = "AT+QIACT=1";
		sendIt(atCommand);
		flag = checkRespForOk(150000);
	}

	if (flag) {
		atCommand = "AT+QIOPEN=1,"+String(socket)+",\"TCP\",\""+sAddress+"\","+String(sPort)+","+LOCAL_PORT+",0";
		sendIt(atCommand);
		flag = checkRespForSocket ( );
	}

	return flag;
}

// Open a SSL socket. Return true if correct.
bool Bg96NbIot::openSslSocket (String sAddress, int sPort)
{
	bool flag = true;

	String atCommand;

	if (flag)
	{
		atCommand = "AT+QSSLCFG=\"sslversion\","+String(socket)+",3";
		sendIt(atCommand);
		flag = checkRespForOk ( 300 );
	}

	if (flag)
	{
		atCommand = "AT+QSSLCFG=\"ciphersuite\","+String(socket)+",0XFFFF";
		sendIt(atCommand);
		flag = checkRespForOk ( 300 );
	}

	if (flag)
	{
		atCommand = "AT+QSSLCFG=\"seclevel\","+String(socket)+",2";
		sendIt(atCommand);
		flag = checkRespForOk ( 300 );
	}

	if (flag)
	{
		atCommand = "AT+QSSLCFG=\"cacert\","+String(socket)+",\"ca.crt\"";
		sendIt(atCommand);
		flag = checkRespForOk ( 300 );
	}

	if (flag)
	{
		atCommand = "AT+QSSLCFG=\"clientcert\","+String(socket)+",\"trafficLight.crt\"";
		sendIt(atCommand);
		flag = checkRespForOk ( 300 );
	}

	if (flag)
	{
		atCommand = "AT+QSSLCFG=\"clientkey\","+String(socket)+",\"trafficLight.key\"";
		sendIt(atCommand);
		flag = checkRespForOk ( 300 );
	}

	if (flag)
	{
		atCommand = "AT+QSSLOPEN=1,"+String(socket)+","+String(socket)+",\""+sAddress+"\","+sPort+",0";
		sendIt(atCommand);
		flag = checkRespForSslSocket ( );
	}

	return flag;
}

// Close SSL socket. Return true if correct.
bool Bg96NbIot::closeSslSocket ()
{
	bool flag = true;

	String atCommand;

	if (flag)
	{
		atCommand = "AT+QSSLCLOSE="+String(socket);
		sendIt(atCommand);
		flag = checkRespForOk ( 30000 );
	}

	return flag;
}

/* 
 *	Send data to remote server.
 *	Return true if send data is correct or false if error 
*/
bool Bg96NbIot::sendData (String data) 
{
	String atCommand = "AT+QISEND="+String(socket)+","+String(data.length());

	bool flag = true;					// Flag for checking correct working of AT commands

	if (flag) 
	{
		sendIt(atCommand);
		flag = checkRespIfInputDataChar ( );
	}

	if (flag)
	{
		sendIt(data);
		flag = checkRespIfDataSent();
	}

	return flag;
}


/* 
 *	Send data to remote server by SSL.
 *	Return true if send data is correct or false if error 
*/
bool Bg96NbIot::sendDataBySsl (String data)
{
	String atCommand = "AT+QSSLSEND="+String(socket)+","+String(data.length());

	bool flag = true;					// Flag for checking correct working of AT commands

	if (flag) 
	{
		sendIt(atCommand);
		flag = checkRespIfInputDataChar ( );
	}

	if (flag)
	{
		sendIt(data);
		flag = checkRespIfDataSent();
	}

	return flag;
}


// Receive data from BG96 socket and return it in string format.
String Bg96NbIot::receiveData () 
{
	bool flag = true;					// Flag for checking correct working of AT commands

	String receivedData = "";

	if (flag) 
	{
		flag = checkRespIfDataReceived ();
	}

	if (flag) 
	{
		String atCommand = "AT+QIRD="+String(socket);
		sendIt(atCommand);
		receivedData = checkRespForHostResponse();
	}

	return receivedData;
}


// Receive data from BG96 SSL socket and return it in string format.
String Bg96NbIot::receiveDatabySsl () 
{
	bool flag = true;					// Flag for checking correct working of AT commands

	String receivedData = "";

	if (flag)
	{
		flag = checkRespIfDataReceivedBySsl ();
	}

	if (flag) 
	{
		String atCommand = "AT+QSSLRECV="+String(socket);
		sendIt(atCommand);
		receivedData = checkRespForHostResponseBySsl();
	}

	// closeSslSocket();

	return receivedData;
}


// Shutdown the module
bool Bg96NbIot::shutdown ()
{
	bool flag = true;					// Flag for checking correct working of AT commands

	if (flag) {
		sendIt ("AT+CFUN=0");
		flag = checkRespForOk (15000);
	}

	if (flag) {
		sendIt ("AT+QPOWD=1");
		flag = checkRespForPoweredDown ();
	}

	ip = "";
	imei = "";

	return flag;	
}








/* Set the NB-IoT module up sending AT commands. Register in the network and saves the IMEI 
	and IP of the device.
	Return true if connection was successful or false otherwise
*/
bool Bg96NbIot::setUpBg96 ( ) 
{
	bool flag;							// Flag for checking the connection

	flag = isAlive();					// Check if there are connection with nbiot module

	while (!flag) 
	{
		flag = isAlive();
	}

	if (flag) 
	{
		flag = confNbiotConnection ();
	}
	if (flag) 
	{
		flag = networkRegistration ();
	}
	if (flag) {
		flag = askForIp ();
	}
	if (flag) {
		flag = askForImei ();
	}

	return flag;
}



// Return true if BG96 module responds to AT commands or false otherwise
bool Bg96NbIot::isAlive() 
{
	sendIt ("AT");						// Send AT command
	return checkRespForOk ( 1000 );		// Check the response during time out
}



// Configure parameters on the module for NB-IoT Connection
bool Bg96NbIot::confNbiotConnection () 
{
	bool flag = true;					// Flag for checking correct working of AT commands

	if (flag) {
		sendIt("AT+CGDCONT=1,\"IP\",\"\"");
		flag = checkRespForOk (500);
	}

	if (flag) {
		sendIt("AT+CFUN=1");
		flag = checkRespForOk (15000);
	}

	return flag;
}


// Select the operator and register SIM in the network
bool Bg96NbIot::networkRegistration () 
{
	bool flag = true;					// Flag for checking correct working of AT commands

	if (flag) 
	{
		String atCommand = "AT+COPS=1,2,\"";// Building AT Command
		atCommand += networkOperator;	// Network operator is defined in .h
		atCommand += "\",9";

		sendIt (atCommand);				// Send AT command
		flag = checkRespForOpSelection ();
	}

	if (flag)
	{
		sendIt ("AT+CEREG?");			// Send AT command
		flag = checkRespForReg ();
	}
	

	return flag;
}


// Saves the IP that NB-IoT network gives to BG96 module
bool Bg96NbIot::askForIp ( ) 
{
	String receivedIp;

	sendIt("AT+CGPADDR");				// Send AT command

	receivedIp = checkRespForIp ();		// Receive and parse the IP

	if ( receivedIp.length() > 0 ) {
		ip = receivedIp;
		return true;
	} else {
		return false;
	}
}

// Saves the IMEI of SIM card
bool Bg96NbIot::askForImei ( ) 
{
	String receivedImei;

	sendIt("AT+CGSN");					// Send AT command

	receivedImei = checkRespForImei ();	// Receive and parse the IMEI

	if ( receivedImei.length() > 0 ) {
		imei = receivedImei;
		return true;
	} else {
		return false;
	}
}









/* Check for OK in the response from BG96 module
	return true if OK is in the response or false if time out 
*/
bool Bg96NbIot::checkRespForOk( int timeOut ) 
{
	String response;
	bool ok = false;
	bool error = false;
	unsigned long startTime = millis();	// Take time at start for time out

	// Check every char string received for "OK" response
	while ( !error && !ok && ((millis()-startTime) <= (unsigned long) timeOut) ) {
		response += receiveIt();
		ok = (response.indexOf("\r\nOK\r\n") >= 0);	// Check if "OK" is in the response
		error = (response.indexOf("\r\nERROR\r\n") >= 0);	// Check if "ERROR" is in response
	}

	return ok;
}


/* Check for network registration in the response from BG96 module
	return true if registration is done or false if time out 
*/
bool Bg96NbIot::checkRespForReg () 
{
	int timeOut = 300;					// From BG96 Quectel AT Commands Manual
	String response;
	bool ok = false;
	unsigned long startTime = millis();	// Take time at start for time out

	// Check every char string received for CEREG message
	while (!ok && ((millis()-startTime) <= (unsigned long) timeOut) ) {
		response += receiveIt();
		ok = (response.indexOf("+CEREG: 0,5") >= 0);
	}

	return ok;
}


/* Check for OK in the response from BG96 module on operator selection
	return true if OK is in the response or false if time out or CME ERROR
*/
bool Bg96NbIot::checkRespForOpSelection ( ) 
{
	int timeOut = 180000;
	String response;
	bool ok = false;
	bool error = false;
	unsigned long startTime = millis();	// Take time at start for time out

	// Check every char string received for "OK" response
	while ( !error && !ok && ((millis()-startTime) <= (unsigned long) timeOut) ) {
		response += receiveIt();
		ok = (response.indexOf("\r\nOK\r\n") >= 0);	// Check if "OK" is in the response
		error = (response.indexOf("\r\n+CME ERROR:") >= 0);	// Check if "ERROR" is in response
	}

	return ok;
}



/* Check for IP given by network in the response from BG96 module
	return the IP in string format 
*/
String Bg96NbIot::checkRespForIp () 
{
	int timeOut = 300;					// From BG96 Quectel AT Commands Manual
	String response;
	String receivedIp = "";
	bool ok = false;
	unsigned long startTime = millis();	// Take time at start for time out

	while ( !ok && ((millis()-startTime) <= (unsigned long) timeOut) ) {
		response += receiveIt();
		if (response.indexOf("\r\nOK\r\n") >= 0) {	// Check all the response is received
			if (response.indexOf("\r\n+CGPADDR:") >= 0) {
				receivedIp = response.substring((1+response.indexOf(",")));
				receivedIp = receivedIp.substring(0,receivedIp.indexOf("\r\n"));
			}
		}
	}

	return receivedIp;
}


/* Check for IMEI's card in the response from BG96 module
*	Return the IMEI in string format 
*/
String Bg96NbIot::checkRespForImei () 
{
	int timeOut=300;
	String response;
	String receivedImei = "";
	bool ok = false;
	unsigned long startTime = millis();	// Take time at start for time out

	while ( !ok && ((millis()-startTime) <= (unsigned long) timeOut) ) {
		response += receiveIt();
		if (response.indexOf("\r\nOK\r\n") >= 0) {	// Check all the response is received
			// The firsts chars received are \r\n
			receivedImei = response.substring(2);
			receivedImei = receivedImei.substring(0,receivedImei.indexOf("\r\n")); 
			ok = true;
		}
	}

	return receivedImei;
}



// Check if socket is opened in the response from BG96 module
bool Bg96NbIot::checkRespForSocket ( ) 
{
	int timeOut = 150000;
	String response;
	bool ok = false;
	unsigned long startTime = millis();	// Take time at start for time out

	while ( !ok && ((millis()-startTime) <= (unsigned long) timeOut) ) {
		response += receiveIt();
		// Check all the response is received
		ok = ((response.indexOf("\r\nOK\r\n") >= 0) && (response.indexOf("+QIOPEN: "+String(socket)+",0"))>=0);
	}

	return ok;
}


// Check if SSL socket is opened in the response from BG96 module
bool Bg96NbIot::checkRespForSslSocket ( ) 
{
	int timeOut = 150000;
	String response;
	bool ok = false;
	unsigned long startTime = millis();	// Take time at start for time out

	while ( !ok && ((millis()-startTime) <= (unsigned long) timeOut) ) {
		response += receiveIt();
		// Check all the response is received
		ok = ((response.indexOf("\r\nOK\r\n") >= 0) && (response.indexOf("+QSSLOPEN: "+String(socket)+",0"))>=0);
	}

	return ok;
}



/* Check for confirmation of correct sending in the response from BG96 module.
*	Return number of bytes BG96 sent or -1 if error 
*/
bool Bg96NbIot::checkRespIfDataSent () 
{
	int timeOut = 10000;
	String response;
	bool ok = false;
	bool error = false;
	unsigned long startTime = millis();	// Take time at start for time out

	while ( !error && !ok && ((millis()-startTime) <= (unsigned long) timeOut) ) 
	{
		response += receiveIt();
		ok = (response.indexOf("\r\nSEND OK\r\n") >= 0);
		error = ((response.indexOf("\r\nSEND FAIL\r\n") >= 0) || (response.indexOf("\r\nERROR\r\n") >= 0 ));
	}

	return ok;
}

// Check response for data received message from BG96 module
bool Bg96NbIot::checkRespIfDataReceived () 
{
	int timeOut = 5000;
	String response;
	bool ok = false;
	unsigned long startTime = millis();	// Take time at start for time out

	while ( !ok && ((millis()-startTime) <= (unsigned long) timeOut) )
	{
		response += receiveIt();
		ok = (response.indexOf("\r\n+QIURC: \"recv\","+String(socket)+"\r\n") >= 0);	// Check all the response is received
	}

	return ok;
}


/* 
* Check response for data received from SSL socket from BG96 module.
*	return true if data received 
*/
bool Bg96NbIot::checkRespIfDataReceivedBySsl () 
{
	int timeOut = 5000;
	String response;
	bool ok = false;
	unsigned long startTime = millis();	// Take time at start for time out

	while ( !ok && ((millis()-startTime) <= (unsigned long) timeOut) )
	{
		response += receiveIt();
		ok = (response.indexOf("\r\n+QSSLURC: \"recv\","+String(socket)+"\r\n") >= 0);	// Check all the response is received
	}

	return ok;
}


/* Check response looking for data received from host server
*  	Return data received from host
*/
String Bg96NbIot::checkRespForHostResponse()
{
	int timeOut = 1000;
	String response = "";
	bool ok = false;
	bool error = false;

	unsigned long startTime = millis();	// Take time at start for time out

	while ( !error && !ok && ((millis()-startTime) <= (unsigned long) timeOut) )
	{
		response += receiveIt();
		ok = (response.indexOf("\r\nOK\r\n") >= 0);
		error = response.indexOf("\r\nERROR\r\n") >= 0;
	}

	// Capture from BG96 response the number of bytes received from host
	String keyWord = "+QIRD: ";
	response = response.substring(response.indexOf(keyWord));	// Delete the first empty line (\r\n)

	// Discard AT commands information from response and keep only data received from host
	int startPos = response.indexOf("\r\n")+2;
	int endPos = response.indexOf("\r\n\r\n\r\nOK\r\n");

	response = response.substring(startPos, endPos);

	return response;
}



/* Check response looking for data received from host server by SSL
*  	Return data received from host
*/
String Bg96NbIot::checkRespForHostResponseBySsl()
{
	int timeOut = 10000;
	String response = "";
	bool ok = false;
	bool error = false;

	unsigned long startTime = millis();	// Take time at start for time out

	while ( !error && !ok && ((millis()-startTime) <= (unsigned long) timeOut) )
	{
		response += receiveIt();

		ok = (response.indexOf("\r\nOK\r\n") >= 0);
		error = response.indexOf("\r\nERROR\r\n") >= 0;
	}

	// Capture from BG96 response the number of bytes received from host
	String keyWord = "+QSSLRECV: ";
	response = response.substring(response.indexOf(keyWord));	// Delete the first empty line (\r\n)

	// Discard AT commands information from response and keep only data received from host
	int startPos = response.indexOf("\r\n")+2;
	int endPos = response.indexOf("\r\n\r\n\r\nOK\r\n");

	response = response.substring(startPos, endPos);

	return response;
}



// Check response for '>' character: module is waiting for data
bool Bg96NbIot::checkRespIfInputDataChar () 
{
	int timeOut = 300;
	String response;
	bool ok = false;
	unsigned long startTime = millis();	// Take time at start for time out

	while ( !ok && ((millis()-startTime) <= (unsigned long) timeOut) )
	{
		response += receiveIt();
		ok = (response.indexOf("\r\n> ") >= 0);
	}

	return ok;
}

/* Check for confirmation of correct sending in the response from BG96 module.
*	Return number of bytes BG96 sent or -1 if error 
*/
bool Bg96NbIot::checkRespForPoweredDown () 
{
	int timeOut = 300;
	String response;
	bool ok = false;

	unsigned long startTime = millis();	// Take time at start for time out

	while ( !ok && ((millis()-startTime) <= (unsigned long) timeOut) ) 
	{
		response += receiveIt();
		ok = (response.indexOf("\r\nOK\r\nPOWERED DOWN\r\n") >= 0);
	}

	delay(1000);						// Time neccesary for actually power down

	return ok;
}

	





// Send an AT command to BG96 module
void Bg96NbIot::sendIt ( String atCommand ) 
{
	atCommand += "\r\n";
#ifdef DEBUGMODE
	printIt("-- "+atCommand);			// Print AT command sended
#endif
	BG96.print (atCommand);				// Send AT command to BG96 module
}



// Receive the response from BG96 module. Return it and print it by debug serial port
String Bg96NbIot::receiveIt ( ) 
{
	String received;

	// Save data received from BG96 module
	while (BG96.available()) {
		received += (char)BG96.read ();
	}
#ifdef DEBUGMODE
	printIt (received);					// DEBUG: Print response from BG96 module
#endif

	return received;					// Return string received
}



// Print something by DEBUG serial port
void Bg96NbIot::printIt ( String text ) 
{
	if (text.length() > 0) {			// If string is not empty ...
		DEBUG.print(text);				// Send by debug serial port the string
	}
}