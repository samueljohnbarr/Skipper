#include "main.h"
#include "chassis.h"

#define LEFTDRIVE 10
#define RIGHTDRIVE 3

int currentSpeed;
int lastSpeed;

void chassisSet(int leftSpeed, int rightSpeed) {
  motorSet(LEFTDRIVE, -leftSpeed);
  motorSet(RIGHTDRIVE, -rightSpeed);
}

void axisTurn(int speed, int right)
{
  int limit = 330;

  encoderReset(leftEncoder);
  encoderReset(rightEncoder);

  if (!right)
  {
    limit = 340;
    chassisSet(-speed, speed);
  }
  else
  {
    limit = 370;
    chassisSet(speed, -speed);
  }

  while(abs(encoderGet(leftEncoder)) <= limit ||
        abs(encoderGet(rightEncoder)) <= limit);

  chassisSet(0, 0);
}

void leftMotorSet(int speed)
{
  motorSet(LEFTDRIVE, -speed);
  lastSpeed = currentSpeed;
  currentSpeed = speed;
}

void rightMotorSet(int speed)
{
  motorSet(RIGHTDRIVE, speed);
  lastSpeed = currentSpeed;
  currentSpeed = speed;
}

int getRightSpeed()
{
  return motorGet(RIGHTDRIVE);
}

int getLeftSpeed()
{
  return -motorGet(LEFTDRIVE);
}

void encChassisSet(int leftEnc, int rightEnc) {
  encoderReset(rightEncoder);
  encoderReset(leftEncoder);
  int speed = 100;

  int isLeftSet = false;
  int isRightSet = false;

  //set initial speed
  if (leftEnc < 0)  leftMotorSet(-speed);
  else              leftMotorSet(speed);
  if (rightEnc < 0) rightMotorSet(-speed);
  else              rightMotorSet(speed);

  while (!isLeftSet || !isRightSet) {
    if (abs(encoderGet(rightEncoder)) >= abs(rightEnc)) {
      isRightSet = true;
      rightMotorSet(0);
    }
    if (abs(encoderGet(leftEncoder)) >= abs(leftEnc)) {
      isLeftSet= true;
      leftMotorSet(0);
    }
  }
}

int degreeToEnc(int d) {
  return round(d*6.13);
}

int encToCm(int encVal) {
  return (int)(encVal/18.78);
}

int cmToEnc(int cm) {
  return (int)(cm * 18.78);
}
