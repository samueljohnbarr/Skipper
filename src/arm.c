#include "main.h"

#define SHOULDER1 3
#define SHOULDER2 4
#define SHOULDER_POT 7
#define ELBOW 5
#define ELBOW_POT 8
#define CLAW 6

void clawSet(int speed) { motorSet(CLAW, speed); }
void elbowSet(int speed) { motorSet(ELBOW, -speed); }


void clawGrab() {
  motorSet(CLAW, -30);
  wait(1000);
  motorSet(CLAW, 0);
}

void clawRelease() {
  motorSet(CLAW, 60);
  wait(1000);
  motorSet(CLAW, 0);
}

void shoulderSet(int speed) {
  motorSet(SHOULDER1, -speed);
  motorSet(SHOULDER2, speed);
}

void elbowPotSet(int pot, int speed) {
  if (getElbowPot() < pot) {
    elbowSet(-speed);
    while (getElbowPot() < pot)
      ;
  } else {
    elbowSet(speed);
    while (getElbowPot() > pot)
      ;
  }
  elbowSet(10);
}

void shoulderPotSet(int pot, int speed) {
  if (getShoulderPot() < pot) {
    shoulderSet(speed);
    while (getShoulderPot() < pot)
      ;
  } else {
    shoulderSet(-speed);
    while (getShoulderPot() > pot)
      ;
  }
  shoulderSet(10);
}

void armSet(int speed) {
  elbowSet(speed);
  shoulderSet(speed);
}

void raiseArm() {
  shoulderPotSet(2100, 60);
  elbowPotSet(745, 60);
}

void extendArm() {
  shoulderPotSet(1200, 50);
  elbowPotSet(1600, 40);
}

void foldArm() {
  elbowPotSet(10, 60);
  shoulderPotSet(1200, 20);
}

int getShoulderPot() { return analogRead(SHOULDER_POT); }

int getElbowPot() { return analogRead(ELBOW_POT); }
