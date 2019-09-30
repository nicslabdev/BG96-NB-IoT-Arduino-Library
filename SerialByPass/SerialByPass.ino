/*********************************************************************************************/
/*
 * SerialByPass
 * Created by Manuel Montenegro, March 21, 2019.
 * Developed for MOTAM project. 
 * 
 *  This sketch stablishes a bypass connection between USB UART and module UART by serial pins,
 *  i.e. D0 (RX) and D1 (TX) pins. 
 * 
 *  This is used in order to manage modules with AT commands support.
 *  
 *  Compatible boards: Arduino Leonardo & Arduino/Genuino 101.
*/
/*********************************************************************************************/

#include "Arduino.h"

#define USB Serial 
#define MODULE Serial1

unsigned long baud = 115200;  //start at 9600 allow the USB port to change the Baudrate


void setup() 
{
  delay(100);

  // Start communication
  USB.begin(baud);
  MODULE.begin(9600);

}

// Forward every message to the other serial
void loop() 
{
  while (USB.available())
  {
    uint8_t c = USB.read();
    MODULE.write(c);
  }

  while (MODULE.available())
  {     
    USB.write(MODULE.read());
  }

}
