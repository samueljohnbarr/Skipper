#ifndef _LINETRACK_H_
#define _LINETRACK_H_

#define RIGHT_FAR 0x01
#define RIGHT_MID 0x02
#define RIGHT_CENTER 0x04
#define LEFT_CENTER 0x08
#define LEFT_MID 0x10
#define LEFT_FAR 0x20

#define RFMCLFM  0x37
#define RFMLFMC  0x3B
#define RFMCLFMC 0x3F

/**
 * Detects where the line is in relation to the robot and
 * returns a number that reflects that error.
 * If it detects a right angle, it calls the rightAngle function,
 * else it calls the Proportional-Derivative (PD) function
 */
void errorCorrection();

/**
 * Checks values of all sensors and returns an integer
 * representing the sensors above threshold.
 * Handles context
 */
int sensorDetection();

/**
 * Returns true if conditions are met to change context
 */
int checkContext(int sensors);

/**
 * Prints sensor values and control variables
 * @param sensorDetection variable
 */

/**
 * Proportional-Derivative Control
 * Corrects motor speeds on both motors depending on
 * how large the error is, and the time it took for errors to change.
 */
void PD(int error);

/**
 * Corrects the robot in the event of a right angle
 */
void rightAngle(int right);

void setThreshold();

/* 4-24-18
static float KP = 0.15;
static float KD = 0.6;

//Flag for white/black switching
int whiteTape = true;

int baseSpeed = 50; //60
int lastError;
int lastTime;
int degree [5] = {175, 250, 325, 400, 475}; //75, 150, 225, 300, 375
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
*/

#endif //_LINETRACK_H_
