# BG96 NB-IoT Arduino Library

## Get started

Just download the library and place it into Arduino/libraries folder.

You can use BG96_NB-IoT_test in order to see how it works.

TLS functions will work only if you previously load CA certificate and client certificate (with its private key) on FILE memory of BG96 module. See QUECTEL BG96 FILE AT COMMANDS MANUAL for more information.

TLS connection requires client and server authentication by default.

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

### Version 2.0

 - Added TLS stack: open socket, send and receive funcions.
