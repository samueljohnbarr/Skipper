#include "main.h"
#include "wireless.h"
#include "executeCmd.h"

void handleLidarInfo(int * payload);
void handleChassisCmd(int * payload);

void decodeCommand(int * payload) {
  int command = payload[0];
  switch(command) {
    case LIDAR_INFO: handleLidarInfo(payload); break;
    case CHASSIS_CMD: handleChassisCmd(payload); break;
  }
}

void handleLidarInfo(int * payload) {
   //lol
}

void handleChassisCmd(int * payload) {
  printf("Setting Chassis...\n");
  chassisSet(payload[1], payload[2]);
}

/**
* Probably doesn't do the right thing since we've changed some things.
* Could probably scrap.
*/
void handleControllerCmd(int * payload) {
  int size = payload[1];
  int button,lValue,rValue;
  for (int i = 1; i <= size; i++){
    button = payload[i];
    if (button == CHASSIS_SET) {
      lValue = payload[i+1];
      rValue = payload[i+2];
      chassisSet(lValue,rValue);
      i += 2;
    } else i++;
  }
}
