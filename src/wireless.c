#include "main.h"
#include "wireless.h"
#include "executeCmd.h"
//Working frequency: 433.4MHz to 473.0MHz
//Communication distance: 1,000m (open space)
#define BAUD 9600 //1.2Kbps up to 115.2Kbps (default 9.6Kbps)

//Request for single transmission
char SINGLE_REQUEST = 0xA5;
//Request for multiple transmissions
char MULTI_REQUEST = 0xA6;
//Confirm transmission
char CONFIRM = 0x5A;
//Deny transmission
char DENY = 0x66;
//Start of transmission
char START_BYTE = 0xC3;
//Used to measure distance
char PING = 0x15;

char * currPayload;

int wirelessChecksum(int command, int * payload, char checkSum, int payloadSize);
char getSentChar();
char receiveStart();
char receiveTransmission(int * payload, int payloadSize);

int currState = SEND;
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
int transmitRequest(int multi) {
  if (currState == SEND) {
    //Suspend the recieve task
    taskSuspend(wireless);
    if (!multi)
      fputc(SINGLE_REQUEST, uart2);
    else
      fputc(MULTI_REQUEST, uart2);
    char ans = getSentChar();
    printf("Response: %x\n", ans);
    if (ans == CONFIRM) {
      printf("CONFIRM\n");
      return true;
    }
    else if (ans == DENY) {
      printf("DENY\n");
      return false;
    }
    else {
      //currState = FAILED;
      return false;
    }
  } else {
    printf("Current State: %d\n", currState);
    return false;
  }
}

void wirelessReceive(void * n) {
  while (true) {
    char ans = getSentChar();
    //Handshake Recieved
    if (ans == SINGLE_REQUEST || ans == MULTI_REQUEST) {
      if (currState == IDLE || currState == RECEIVE) {
        fputc(CONFIRM, uart2);
        printf("Confirmed Request\n");
        currState = RECEIVE;
        while (true) {
          char cmd = receiveStart();
          printf("cmd: %x\n",cmd);
          int payloadSize = getSentChar();
          int data[payloadSize + 1];
          data [0] = cmd;
          int chckSum = receiveTransmission(data, payloadSize);
          if (data == NULL) break;
          if (!wirelessChecksum(cmd, data, chckSum, payloadSize)) break;
          decodeCommand(data);

          if (ans == SINGLE_REQUEST) break;
          delay(2);
        }
        fputc(OVER, uart2);
        currState = IDLE;
      } else
        fputc(DENY, uart2);
    } else if (ans == PING)
        fputc(CONFIRM, uart2);
    delay(5);
  }
}

char receiveStart() {
  char startByte = getSentChar();
  printf("Start Byte: %x\n",startByte);
  if (startByte == START_BYTE) {
    char command = getSentChar();
    return command;
  }
  return 0;
}

/**
* Receives the incoming signal from the uart2 communication port.
* Returns true if the transmission was received successfully,
* returns false otherwise.
*/
char receiveTransmission(int * payload, int payloadSize) {
  for (int i = 1; i <= payloadSize; i++) {
     payload[i] = getSentChar();
     printf("Payload: %d\n", payload[i]);
  }
  char chckSum = getSentChar();
  return chckSum;
}

/**
* Sends the specified payload through the wireless transmitter
* in order for that transmission to be received on the other end.
**/
void wirelessSend(int command, int * payload, int payloadSize) {
  fputc(START_BYTE,uart2);
  fputc(command,uart2);
  fputc(payloadSize,uart2);
  char checkSum = START_BYTE ^ command ^ payloadSize;
  printf("Initial checksum: %x\n",checkSum);
  for (int i = 0; i < payloadSize; i++) {
      fputc(payload[i],uart2);
      checkSum ^= payload[i];
      printf("modified checksum: %x\n",checkSum);
  }
  //Checksum
  printf("Checksum Sent: %x\n",checkSum);
  fputc(checkSum,uart2);
  while (fcount(uart2))
    printf("char received: %x\n",fgetc(uart2));
}



/**
* Returns true if the calculated check sum is the same as
* the check sum that is passed in.
*/
int wirelessChecksum(int command, int * payload, char checkSum, int payloadSize) {
  char ckSum = START_BYTE ^ command ^ 2;
  for (int i = 0; i < payloadSize; i++) {
    ckSum ^= payload[i];
  }
  return (checkSum == ckSum);
}

/**
* Waits for an available character to be read from the
* specified serial port. Returns the character after it is read.
*/
char getSentChar() {
  unsigned long int timeout = 5000 + millis();
  //Wait for something on the buffer
  while(!fcount(uart2))
    //Return if this action takes more than 5 seconds
    if (millis() > timeout) return 0x00;
  return fgetc(uart2);
}

/**
 * Sends an over byte and loops until it recieves one back.
 * Ensures that the buffer is empty before a new transmission
 */
void terminateTransmission() {
  fputc(OVER, uart2);
  int ans;
  //If an OVER is not immediately sent back, flush until found
  do {
    ans = getSentChar();
  } while (ans != OVER);
  taskResume(wireless);
}

/*
* Pings the other robot. Returns the time between wireless
* transmissions.
*/
double getPing() {
  printf("Pinging...\n");
  //Stop recieve process
  taskSuspend(wireless);
  fputc(PING, uart2);
  double tStart = micros();
  while(!fcount(uart2));
  double tEnd = micros();
  double ping = tEnd - tStart;
  printf("Ping: %f\n", ping);
  delay(2);
  taskResume(wireless);
  return ping;
}

/**
* Used to find the distance between the robots using
* wireless transceivers. Does not currently work; may not
* be able to work?
*/
void getWirelessDistance() {
  printf("Entering wireless distance.\n");
  taskSuspend(wireless);
  double tstart = micros();
  printf("initial time: %f us(microseconds)\n",tstart);
  for (int i = 0; i < 10; i++) {
      //if (!(i % 10)) printf("%d\n",i);
      fputc(PING,uart2);
      while(!fcount(uart2));
      fgetc(uart2);
  }
  double tfinal = micros();
  printf("final time: %f us(microseconds)\n",tfinal);
  double time = tfinal - tstart;
  printf("total time: %f us(microseconds)\n",time);
  time /= 10; //Average time per transmission
  time /= 1000000; //Convert micros to seconds
  printf("Average time per transmission: %f s\n",time);
  double distance = 300000000*time;
  printf("Expected time: 300 nanoseconds.\n");
  printf("Experimental time: %1.9f\n",time);
  printf("Expected distance: 1 meter.\n");
  printf("Experimental distance: %2.3f\n",distance);
  taskResume(wireless);
}
