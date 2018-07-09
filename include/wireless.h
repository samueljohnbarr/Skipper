#ifndef _WIRELESS_H_
#define _WIRELESS_H_

int initTransmitter();

void controllerSend(int * payload);

void handshakeReceive(void * n);

#endif
