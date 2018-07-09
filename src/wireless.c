#include "main.h"
#include "wireless.h"
//Working frequency: 433.4MHz to 473.0MHz
//Communication distance: 1,000m (open space)
#define BAUD 9600 //1.2Kbps up to 115.2Kbps (default 9.6Kbps)

//Request for transmission
char REQUEST = 0xA5;
//Confirm transmission
char CONFIRM = 0x5A;
//Deny transmission
char DENY = 0x66;
//Transmission End
char OVER = 0x75;
//Start of transmission
char START_BYTE = 0xC3;

char CONTROLLER_CMD = 0x15;
char LIDAR_INFO = 0x25;

char * currPayload;

enum wirelessState{SEND, RECIEVE, FAILED};
volatile enum wirelessState currState = SEND;
void wirelessSend(int command, int * payload, int payloadSize);
/*
 *___________________________________________________________
 *| Start Byte | Command | PayloadSize | Payload | Checksum |
 *************************************************************
 */

int initTransmitter() {
  usartInit(uart2, BAUD, SERIAL_8N1);
  return 1;
}

/**
* Attempting to initiate first contact.
*/
int handshakeTransmit() {
  if (currState == SEND) {
    fputc(REQUEST, uart2);
    delay(2);
    char ans = fgetc(uart2);
    if (ans == CONFIRM) return true;
    else if (ans == DENY) return false;
    else {
      currState = FAILED;
      return false;
    }
  } else {
    printf("Current State: %d\n", currState);
    return false;
  }
}

void handshakeReceive(void * n) {
  while (true) {
    char ans = fgetc(uart2);
    if (ans == REQUEST) {
      if (currState == RECIEVE) {
        fputc(CONFIRM, uart2);
        //return true;
      } else
        fputc(DENY, uart2);
    }
  }
}

/**
* Sends the specified payload through the wireless transmitter
* in order for that transmission to be received on the other end.
**/
void wirelessSend(int command, int * payload, int payloadSize) {
  fputc(START_BYTE,uart2);
  fputc(command,uart2);
  fputc(payloadSize,uart2);
  int checkSum = START_BYTE ^ command ^ payloadSize;
  for (int i = 0; i < payloadSize; i++) {
      fputc(payload[i],uart2);
      checkSum ^= payload[i];
  }
  //Checksum
  fputc(checkSum,uart2);
}

/*
* Sends the buttons being pressed on the controller through the
* the wireless transmitter by calling upon wirelessSend(). Used
* in order to control the other robot with only one remote.
*/
void controllerSend(int * payload) {
  int controllerPayloadSize = 0x02;
  wirelessSend(CONTROLLER_CMD, payload, controllerPayloadSize);
}

/*
* Receives the character array from the uart2 port that
* is being picked up by the transmitter. Saves the character
* array in the int pointer that is passed as a parameter.
*/
void receiveTransmission(int * payload) {
  char startByte = fgetc(uart2);
  if (startByte == START_BYTE) {
    char command = fgetc(uart2);
    payload[0] = command;
    char payloadSize = fgetc(uart2);
    for (int i = 1; i <= payloadSize; i++) {
       payload[i] = fgetc(uart2);
    }
  }
}
