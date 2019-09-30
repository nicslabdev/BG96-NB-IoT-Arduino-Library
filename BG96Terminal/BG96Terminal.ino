#include "Arduino.h"

#define USB Serial 
#define BG96 Serial1

#define BG96_STATUS_PIN 9
#define BG96_PWRKEY_PIN 10

unsigned long baud = 115200;


void setup() 
{

  pinMode(BG96_STATUS_PIN, INPUT);
  pinMode(BG96_PWRKEY_PIN, OUTPUT);
  
  delay(100);

  // Start communication
  USB.begin(baud);
  BG96.begin(baud);

//  if (digitalRead(BG96_STATUS_PIN) == LOW) {
//    digitalWrite(BG96_PWRKEY_PIN, HIGH);
//  }

}

// Forward every message to the other serial
void loop() 
{
  while (USB.available())
  {
    uint8_t c = USB.read();
    BG96.write(c);
  }

  while (BG96.available())
  {     
    USB.write(BG96.read());
  }
}
