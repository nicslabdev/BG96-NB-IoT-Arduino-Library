#include <Bg96NbIot.h>


String HOST = "hook.io";
String SERVER_PATH = "/examples/echo/testing";
int SERVER_PORT = 80;

Bg96NbIot nbiot;                                        // BG96 module instance

void setup() 
{
    Serial.begin(115200);
    
    bool flag = true;

    if (flag) 
    {
        flag = nbiot.begin();
    }
    
    if (flag) 
    {
        Serial.println("IP: "+nbiot.getIP());
        Serial.println("IMEI: "+nbiot.getIMEI());
        flag = nbiot.openSocket(HOST, SERVER_PORT);
    }

    if (flag)
    {
        flag = nbiot.sendData("GET "+SERVER_PATH+" HTTP/1.1\r\nHost: "+HOST+"\r\nConnection: keep-alive\r\n\r\n");
    }

    if (flag)
    {
        String response = nbiot.receiveData();
        Serial.println(response);
    }

    if (flag)
    {
        Serial.println("===== Application: Everything worked properly");
    }
    else
    {
        Serial.println("===== Application: Something went wrong");
    }

    nbiot.shutdown();
}


void loop()
{

}