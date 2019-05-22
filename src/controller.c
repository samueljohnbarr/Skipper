#include "chassis.h"
#include "lidar.h"
#include "lineTrack.h"
#include "main.h"
#include "memory.h"
#include "movement.h"
#include "print.h"
#include "tools.h"
#include "wireless.h"
#include "controller.h"
#include "command.h"
#include "lidarMap.h"
#include "roam.h"
#include "cluster.h"


//int track = false;
int inRangeOnce = 1;
int lidarStatus = 1;
int speed1 = 200;
char scan[360 * SCAN_DATA_LEN + HEADER_LENGTH + 1];
unsigned int distances[FULL];
char read[FULL];
char head[HEADER_LENGTH];
int readIndex = 0;
int pCount = 0;
char SCAN_HEAD[] = {0xA5, 0x5A, 0x05, 0x00, 0x00, 0x40, 0x81};
int send[3];
extern int currState;

void leftBumpUpButton() {
  int * distances = lidarGetDistances();
  setMap(distances);
  initClusters(300, 120);
  printClusters();
}

void leftBumpDownButton() {
  int * distances = lidarGetDistances();
  patchDistances(distances);
  setMap(distances);
  printMap();
}

void rightBumpUpButton() {
  printf("Right: %d\n", encoderGet(rightEncoder));
  printf("Left: %d\n\n", encoderGet(leftEncoder));
  delay(500);
}

void rightBumpDownButton() {
  int angle = findHeading();
  printf("Path Angle: %d\n", angle);
}

void leftPadLeftButton() {
  float x = atan(50/2.0) * (180.0/3.14159);
  printf("degrees: %f\n", x);
  printf("%f\n", round(2 * (x/10)));

}




void leftPadUpButton() {
  //lidarScan(NULL);
  printf("Scanned\n");
  int * dist = lidarGetDistances();
  for (int i = 0; i < ANGLES; i++) {
    printf("%d\n", dist[i]);
    delay(20);
  }
  //lidarPrintDistances();
}




void leftPadRightButton() {
  int * distances = lidarGetDistances();
  //patchDistances(distances);
  //Create a map of x & y points
  setMap(distances);
  printMap();
}





//WHATI TODO:
void leftPadDownButton() {
  encoderReset(rightEncoder);
  encoderReset(leftEncoder);
  printf("Reset\n");
}


void rightPadLeftButton() {
  lidarPrintDistances();
}





void rightPadUpButton() {

}





void rightPadRightButton() {

}





void rightPadDownButton() {
  delay(500);
  int power, turn;
  while (!(joystickGetDigital(CONTROLLER, RIGHT_BUTT_SET, RIGHT_BUTT))) {
    power = joystickGetAnalog(CONTROLLER, R_JOY_V);
    if (power > -25 && power < 25)
      power = 0;
    turn = joystickGetAnalog(CONTROLLER, R_JOY_H);
    if (turn > -25 && turn < 25)
      turn = 0;

    chassisSet(power + turn, power - turn);
    //chassisSet(power, power);

    while (joystickGetDigital(CONTROLLER, RIGHT_BUTT_SET, DOWN_BUTT)) {
      //print("here\n");
      roam();
    }

    if (joystickGetDigital(CONTROLLER, RIGHT_BUTT_SET, UP_BUTT)) {
      delay(500);
      printf("checksum: %x\n", getChecksum());
    }
    if (joystickGetDigital(CONTROLLER, LEFT_BUTT_SET, RIGHT_BUTT)) {
      int * distances = lidarGetDistances();
      printf("Desired Angle: %d\n",getPath(280, 160, distances) % 360);
      delay(500);
    }
  }
}
