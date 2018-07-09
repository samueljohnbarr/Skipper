#include "main.h"
#include "lineTrack.h"
#include "chassis.h"
#include "arm.h"
#include "sensors.h"
#include "transfer.h"

static float KP = 0.15;
static float KD = 0.6;

//Can probably remove?
int whiteTape = true;

int baseSpeed = 45; //60
int lastError;
int lastTime;
int degree [5] = {75, 150, 225, 300, 375};//{175, 250, 325, 400, 475};
int error;
int sensors;
int printCount = 0;
int cushion = 400;// = 600;
int cushionCount = 0;
int threshold = 0; // = 500; //800
int threshLow;
int threshAvg[40];
int currentThreshold;
int threshAvgCount = 2;
int *ct;
int track = true;
int reps = 0;
//int inRangeOnce = 1;

void autonomous() {
  // Wait for debounce
  delay(500);
  // errorCorrection();
  chassisSet(40, 40);
  // goStraight();
  // setThreshold();
  while (track && (reps < 100)) {
    setThreshold();
    reps++;
  }
  reps = 0;
  while (track) {
    // Line track
    errorCorrection();
    if (isInRange()) {
      chassisSet(0, 0);
      //if (inRangeOnce) {
        //printf("In Range.");
        //inRangeOnce--;
      //}
      delay(300);
      if (joystickGetDigital(CONTROLLER, RIGHT_BUTT_SET, JOY_DOWN))
        // backUpProcedure();
        // operatorControl();
        track = false;
    }
    if (joystickGetDigital(CONTROLLER, RIGHT_BUTT_SET, JOY_DOWN)) {
      track = false;
      delay(500);
    }
    delay(20);
  }
}

void setThreshold() {
	chassisSet(baseSpeed, baseSpeed);
	cushion = 0;
	for (int i = 0; i < 500; i++) {
		ct = getCT(cushion);
		 cushion = *(ct+1);
		threshold += *ct;
	}
	threshold /= 500;
	printf("Cushion: %d\n", cushion);
	printf("Threshold: %d\n", threshold);
}

void errorCorrection()
{
	error = 0;
	sensors = sensorsDetection(threshold, cushion);
	//Context switch
	if (checkContext(sensors)) {
		//Algorithm below checks to see if context switch or line encountered
		int swapped = 0;
		//set error in case contextSwitch does not occur
		//error is based on second value to encounter line
		error = (abs(analogRead(FR) - threshold) <= abs(analogRead(FL) - threshold))
		  			? -degree[4] : degree[4];
		for (int i = 0; i < 2000 && !swapped; i++) {
			if (!checkContext(sensorsDetection(threshold, cushion)))
				swapped = 1;
		}
//This occurs when checkContext encountered a line
		if (swapped)
				PD(error);
//checkContext encountered a context switch, need new cushion and threshold
		else
		    setThreshold();
	}

  switch (sensors) {
    //Right angle detection
		case RIGHT_FAR+RIGHT_MID+RIGHT_CENTER:
		  rightAngle(true);
			break;
		case LEFT_FAR+LEFT_MID+LEFT_CENTER:
		  rightAngle(false);
			break;

    //Double sensor detection
		case RIGHT_FAR+RIGHT_MID:
		  error = degree[3];
		  break;
		case RIGHT_MID+RIGHT_CENTER:
		  error = degree[1];
			break;
		case LEFT_CENTER+LEFT_MID:
		  error = -degree[1];
			break;
		case LEFT_MID+LEFT_FAR:
		  error = -degree[3];
			break;

		//Single sensor detection
    case RIGHT_FAR:
		  error = degree[4];
			break;
		case RIGHT_MID:
		  error = degree[2];
			break;
		case RIGHT_CENTER:
		  error = degree[0];
			break;
		case LEFT_CENTER:
		  error = -degree[0];
			break;
		case LEFT_MID:
		  error = -degree[2];
			break;
		case LEFT_FAR:
		  error = -degree[4];
			break;
	}

	//Off-line check
	if (error == 0 && lastError == degree[4])
		error = degree[4];

  if (error == 0 && lastError == -degree[4])
		error = -degree[4];

	PD(error);

}

//check LineTrack.h for values
int checkContext(int sensors) {
	if (sensors == RFMCLFM || sensors == RFMLFMC || sensors == RFMCLFMC) {
		   printf("Context Switch\n");
			 return true;
	}
	return false;
}

void PD(int error) {
	float derivative = 0;
	int highBound = 127; //110
	int lowBound = -40;
	int motorSpeed = 0;

	//Derviative calculation
	if (error != lastError) {
    int currentTime = millis();
		float seconds = ((currentTime - lastTime) / 1000);
    lastTime = currentTime;

		if (seconds != 0)
			derivative = error / seconds;
		else
			derivative = 0;
		if (abs(lastError) > abs(error))
      derivative = -derivative;
		lastError = error;
	}

	motorSpeed = (KP * error) + (KD * derivative);

	rightMotorSet((baseSpeed - motorSpeed));
	leftMotorSet((baseSpeed + motorSpeed));

	//Motor Bound Check
	if (getRightSpeed() < lowBound)  rightMotorSet(lowBound);
	if (getLeftSpeed()  < lowBound)  leftMotorSet(lowBound);
	if (getRightSpeed() > highBound) rightMotorSet(highBound);
	if (getLeftSpeed()  > highBound) leftMotorSet(highBound);


}

void rightAngle(int right)
{
	int sensor;
	sensor = sensorsDetection(threshold,cushion);
	printf("Right Angle\n");

	//Turn with one side till mid sensor detects tape
	if (right) {
    while (sensor != LEFT_MID && sensor != LEFT_CENTER &&
		       sensor != LEFT_MID+LEFT_CENTER && !checkContext(sensor)) {
			sensor = sensorsDetection(threshold,cushion);
			//sensor = (threshold < 500) ? sensorsHighDetection(threshold, whiteTape, cushion) : sensorsLowDetection(threshold, whiteTape, cushion);
			chassisSet(baseSpeed + 10, 0);
			if (joystickGetDigital(CONTROLLER, RIGHT_BUTT_SET, JOY_DOWN)) break;
		}
    } else {
    while (sensor != RIGHT_MID && sensor != RIGHT_CENTER &&
		       sensor != RIGHT_MID+RIGHT_CENTER && !checkContext(sensor)) {
			sensor = sensorsDetection(threshold,cushion);
			//sensor = (threshold < 500) ? sensorsHighDetection(threshold, whiteTape, cushion) : sensorsLowDetection(threshold, whiteTape, cushion);
      chassisSet(0, baseSpeed + 10);
			if (joystickGetDigital(CONTROLLER, RIGHT_BUTT_SET, JOY_DOWN)) break;
		}
   }

  lastTime = millis();
}
