#include "main.h"
#include "chassis.h"

#define LEFTDRIVE 10
#define RIGHTDRIVE 1

int currentSpeed;
int lastSpeed;

void chassisSet(int leftSpeed, int rightSpeed) {
  motorSet(LEFTDRIVE, -leftSpeed);
  motorSet(RIGHTDRIVE, rightSpeed);
}

void axisTurn(int speed, int right) {
  int limit = 330;
  resetEncoders();

  if (!right) {
    limit = 340;
    chassisSet(-speed, speed);
  }
  else {
    limit = 370;
    chassisSet(speed, -speed);
  }

  while(abs(getLeftEnc()) <= limit ||
        abs(getRightEnc()) <= limit);

  chassisSet(0, 0);
}

void leftMotorSet(int speed) {
  motorSet(LEFTDRIVE, -speed);
  lastSpeed = currentSpeed;
  currentSpeed = speed;
}

void rightMotorSet(int speed) {
  motorSet(RIGHTDRIVE, speed);
  lastSpeed = currentSpeed;
  currentSpeed = speed;
}

int getRightSpeed() {
  return motorGet(RIGHTDRIVE);
}

int getLeftSpeed() {
  return -motorGet(LEFTDRIVE);
}

int getLeftEnc() {
  return encoderGet(leftEncoder);
}

int getRightEnc() {
  return encoderGet(rightEncoder);
}

void resetEncoders() {
  encoderReset(leftEncoder);
  encoderReset(rightEncoder);
}
