#ifndef _WIRELESS_H_
#define _WIRELESS_H_

#define CONTROLLER_CMD 0x15
#define LIDAR_INFO 0x25
#define CHASSIS_CMD 0x35
#define MOVECM_CMD 0x45

//Transmission End
#define OVER 0x75

//enum wirelessState{IDLE, SEND, RECEIVE, FAILED};
//volatile enum wirelessState currState = SEND;

#define IDLE 0
#define SEND 1
#define RECEIVE 2
#define FAILED 3

int initTransmitter();

void wirelessReceive(void * n);

void wirelessSend(int command, int * payload, int payloadSize);

/**
 * Requests the other party to recieve this party's
 * transmission
 * @param multi true if multi transmit request, else single
 */
int transmitRequest(int multi);

void handlePayload(int * payload);

void getWirelessDistance();

double getPing();

/**
* Terminates the transmission between the receiving robot and
* this robot.
*/
void terminateTransmission();

#endif
