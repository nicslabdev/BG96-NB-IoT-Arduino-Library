# BG96 NB-IoT Arduino Library

## Get started

Just download the library and place it into Arduino/libraries folder.

You can use BG96_NB-IoT_test in order to see how it works.

## Compatible boards:

 - Arduino/Genuino 101
 - Arduino Leonardo

And probably another Arduino boards. You can test it and give us feedback.

We used **AVNET BG96 NB-IoT Arduino shield** in order to develop this library, but any BG96 module should work because the code only uses serial port.

## Debugging

If you can see AT commands sent to BG96 module and its responses, just uncomment from Bg96NbIot.h this line

    // #define DEBUGMODE

## Changelog
### Version 1.0

 - Added TCP stack: open socket, send and receive functions.
