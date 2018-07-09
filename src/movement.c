#include "main.h"
#include "chassis.h"
#include "arm.h"
#include "print.h"

int speed = 120;
int currentThreshold = 0;
//int threshAvgCount = 2;
int thresho = 0;
//int threshAvg[40];
int printCount;
int* tc;
int cush;

void dance() {
  unsigned long t = 5000;
  chassisSet(speed, -speed);
  raiseArm();
  unsigned long hold = millis() + t;
  while (millis() < hold) {
    clawGrab();
    clawRelease();
  }
  chassisSet(0,0);
  elbowPotSet(10, 20);
  shoulderPotSet(1200, 30);
  clawGrab();
}

void backUpProcedure() {
  chassisSet(90,-90);
  delay(2000);
}

//Used for testing purposes to figure out threshold averaging
//This shit is tough
void goStraight(){
  thresho = getSensorsAverage();
  cush = 200;
  for (int i = 0; i < 1000; i++) {
    tc = getCT(cush);
    thresho += *tc;
    cush = *(tc+1);
  }
  thresho /= 1000;
  printCount++;
  if (printCount % 100 == 0) {
    printf("Threshold: %d\n", thresho);
    printf("Cushion: %d\n", cush);
  }
}
