#include "main.h"
#include "chassis.h"
#include "lidar.h"
#include "lidarMap.h"

#define LENGTH_OF_BOT 50
#define MINIMUM_CLEARANCE 30
#define MAX_RANGE 300

//Headings defined in main.h
#define NO_MVMT 8

void setClosestPoint();
int getPath(int leftBound, int span, int * distances);
double calcError(int angle);
void roamPD(int error);

int clearances[180];
int direction;
int refStart = 0;
int closeX;
int closeY;
int angle;
int lastTime = 0;
int error;
int lastError;

/**
 * Verifies a point
 * @param a -360 to 719
 * @return point from 0-360
 */
int verify(int a) {
  if (a < 0) {
    return 360 + a;
  }
  if (a >= 360) {
    return a - 360;
  }
  return a;
}

int isInRange(int heading) {
   switch (heading) {
     case NORTH:
       return getQuadrant(7) || getQuadrant(0);
     case SOUTH:
       return getQuadrant(3) || getQuadrant(4);
     case EAST:
       return getQuadrant(1) || getQuadrant(2);
     case WEST:
       return getQuadrant(5) || getQuadrant(6);
     case NORTHEAST:
       return getQuadrant(0) || getQuadrant(1);
     case NORTHWEST:
       return getQuadrant(6) || getQuadrant(7);
     case SOUTHWEST:
       return getQuadrant(5) || getQuadrant(4);
     case SOUTHEAST:
       return getQuadrant(2) || getQuadrant(3);
   }
   return false;
}

/**
 * Retrieves heading of robot given power & turn
 */
int getHeading(int power, int turn) {
   if (power == 0 && turn == 0) return NO_MVMT;
   if (power > 0 && !turn)      return NORTH;
   if (power < 0 && !turn)      return SOUTH;
   if (!power && turn > 0)      return EAST;
   if (!power && turn < 0)      return WEST;
   if (power > 0 && turn > 0)   return NORTHEAST;
   if (power > 0 && turn < 0)   return NORTHWEST;
   if (power < 0 && turn > 0)   return SOUTHWEST;
   if (-power && -turn)         return SOUTHEAST;

   return NO_MVMT;
}


int findHeading() {
  lidarScan(NULL);
  int * distances = lidarGetDistances();
  int curr[2] = {0,0};
  int max[2] = {0,0};
  float pathwidth;
  int clear;
  //Iterate through every possible angle
  for (int i = 0; i < ANGLES; i++) {
    //For each angle, iterate distance in 5cm increments
    for (int d = 10; d <= MAX_RANGE; d += 5) {
      float degrees = atan(LENGTH_OF_BOT/2.0) * (180/3.14159);
      pathwidth = round((2 * degrees)/d);
      //printf("Index %d | Distance: %d | Width: %f\n", i, d, pathwidth);
      int init = round(i - pathwidth/2);
      int end = round(i + pathwidth/2);
      //printf("Start Angle: %d | End Angle: %d\n", verify(init), verify(end));
      //Test angles of path to see if path is clear at distance d
      for (int j = init; j <= end; j++) {
        int dist = distances[verify(j)];
        //printf("Distance: %d\n", dist);
        //If dist is zero, disregard (bad point)
        if (dist != 0) {
          if (dist <= d) {
            //printf("Dist %d < range %d\n", dist, d);
            clear = false;
          }
        }
        if (!clear) break;
      }
      //If clear, update curr
      if (clear) {
        curr[0] = i;
        curr[1] = d;
      }
      //If not, path has reach max
      else {
        clear = true;
        break;
      }
    }
    //If current dist is greater than max dist, set max to curr
    if (curr[1] > max[1]) {
      //printf("Max Angle: %d\n", curr[0]);
      max[0] = curr[0];
      max[1] = curr[1];
    }
    //reset curr
    curr[0] = 0;
    curr[1] = 0;
  }
  //Return angle with max path distance
  return max[0];
}

/**
* TODO: Maybe add parameter for single angle i.e., calcClearance(int angle)
* Calculates the clearance from angle1 to angle 2.
* @return True if nothing in way
*/
int calcClearance(int angle1, int angle2) {
  /*
  (((1-85)/(275-359)))
  for (int i = 0; i < 86; i++) {
    int theta  = 90 - i;
    clearances[i] = getlidarDistance[i] * sin(theta) - (lengthOfBot/2);
  }
  */
  return 0;
}

/**
* Use the encoders to determine (roughly) what angle we are
* off from our initial position. Gives us an idea of where we
* started looking
*
*/
void setReference() {
  int leftLast;
  int rightLast;
  int leftCurrent;
  int rightCurrent;

  if (!refStart) {
    leftLast = encoderGet(leftEncoder);
    rightLast = encoderGet(rightEncoder);
  }

  leftCurrent = encoderGet(leftEncoder) - leftLast;
  rightCurrent = encoderGet(rightEncoder) - rightLast;

  leftCurrent = encToCm(leftCurrent);
  rightCurrent = encToCm(rightCurrent);


}

/**
* Returns the angle of displacement from our initial starting position.
* Return either 0..90 & 0..-90 (preferable?)  or just 0..360
*
*/
int getReference() {
  return 0;
}

/**
* More general direction of where we are facing N,E,S,W
*/
void setDirection(int heading) {

}

/**
* Get the general direction of where we are facing
*/
int getDirection() {
  return 0;
}

/**
 * Calculates average distance over a span
 * from startAngle to (startAngle + bot) length
 *
int getLengthAverage(int startAngle) {
  int * dist = lidarGetDistances();
  int avg;
  int numPoints = 0;
  for (int i = startAngle; i < (startAngle + LENGTH_OF_BOT); i++) {
      if (dist[i % 360] != -1) {
        avg += dist[i % 360];
        numPoints++;
      }
  }
  return avg /= numPoints;
}

int ensureClearance(int startAngle) {
    int count = 0;
    int * dist= lidarGetDistances();
    for (int i = startAngle; i < startAngle + 50; i++) {
      if (dist[i % 360] < MINIMUM_CLEARANCE && dist[i % 360] != -1) count++;
    }
    return count;
}
*/
/**
* Traffic control method used when robot is roaming.
*
*/
void roam() {
  //Create, fill, and patch distance vector
  int * distances = lidarGetDistances();
  patchDistances(distances);
  //Create a map of x & y points
  setMap(distances);

  int desiredAngle = getPath(280, 160, distances) % 360;
  //printf("Desired Angle: %d\n\n", desiredAngle);
  double error = calcError(desiredAngle);
  //printf("Error: %f\n", error);
  roamPD(error);
  delay(100);
}

/**
 * Finds objects in direct path of robot.
 * Returns array of obsticals.
 * obsticals[angleOfObstical][0] = XPointOfObstical
 * obsticals[angleOfObstical][1] = YPointOfObstical
 */
void detectObstacles(int start, int end, int obstacles[180][3], int * numObstacles) {

  for (int i = start; i <= end; i++) {
    int angle = i % 360;
    //If point is in the way of the robot
    int x = getXPoint(angle);
    int y = getYPoint(angle);
    if (x > -LENGTH_OF_BOT/2 && x < LENGTH_OF_BOT/2 && x && y) {
        obstacles[*numObstacles][0] = getXPoint(angle);
        obstacles[*numObstacles][1] = getYPoint(angle);
        obstacles[*numObstacles][2] = angle;
        *numObstacles++;
    }
  }
}

void printObstacles() {

}

void setClosestPoint() {
  int leftBound = 270;
  int rightBound = 40;

  //Detect
  int obstacles[180][3];
  int numObstacles = 0;
  detectObstacles(leftBound, (ANGLES+rightBound),
           obstacles, &numObstacles);

  //Get closest point
  int minimum = -1;
  int minIndex = -1;
  for (int i = 0; i < numObstacles; i++) {
    if (obstacles[i][1] < minimum) {
      minimum = obstacles[i][1];
      minIndex = i;
    }
  }
  closeX = obstacles[minIndex][0];
  closeY = obstacles[minIndex][1];
  angle  = obstacles[minIndex][2];
}



/**
 * Determines the angles at which the most clearance exists
 * @param leftBound  - lower portion of chunk we wish to scan
 * @param rightBound - upper portion of chunk we wish to scan
 * @param distances  - array of angles and distances from lidar

int getPath(int leftBound, int span, int * distances) {
  //int span = rightBound - leftBound;
  printf("leftbound: %d, span: %d\n",leftBound,span);
  int accumulator1 = 0, accumulator2 = 0;
  int a1Count = 0, a2Count = 0;

  //base case: span is narrowed down to 10 degrees or less
  if (span <= 10)
    return (leftBound + span/2);

  //get left side distance averages
  for (int i = 0; i < span/2; i++) {
    if (distances[(leftBound + i) % 360] != 0) {
      accumulator1 += distances[(leftBound + i) % 360];
      a1Count++;
    }
  }
  //get right side distance averages
  for (int i = span/2; i < span; i++) {
    if (distances[(leftBound + i) % 360] != 0) {
      accumulator2 += distances[(leftBound + i) % 360];
      a2Count++;
    }
  }

  //Average distances at both halves of our span
  accumulator1 /= a1Count;
  accumulator2 /= a2Count;
  printf("leftAverage: %d    rightAverage: %d\n\n",accumulator1,accumulator2);
  //If there is more room on the left side, recurse on that
  if (accumulator1 > accumulator2)
    return getPath(leftBound, span/2, distances);
  //otherwise recurse on right side
  else
    return getPath((leftBound + span/2), span/2, distances);
}
*/

int getPath(int leftBound, int span, int * distances) {
  //int span = rightBound - leftBound;
  //printf("leftbound: %d, span: %d\n",leftBound,span);
  int accumulator1 = 0, accumulator2 = 0;
  int a1Count = 0, a2Count = 0;
  int threshold = 30;

  //base case: span is narrowed down to 10 degrees or less
  if (span <= 10)
    return (leftBound + span/2);

  //get number of consecutive good values from left side
  for (int i = 0; i < span/2; i++) {
    accumulator1 += distances[(leftBound + i) % 360];
    a1Count++;

  }
  //get number of consecutive good values from right side
  for (int i = span/2; i < span; i++) {
    accumulator2 += distances[(leftBound + i) % 360];
    a2Count++;

  }

  //Average distances at both halves of our span
  accumulator1 /= a1Count;
  accumulator2 /= a2Count;

//  printf("leftAverage: %d    rightAverage: %d\n\n",accumulator1,accumulator2);
  //If there is more room on the left side, recurse on that
  if (accumulator1 > accumulator2) {
    //printf("left: %d  -  leftAngle: %d  -  rightAngle: %d\n",accumulator1,leftBound,leftBound+span);
    return getPath(leftBound, span/2, distances);
  }
  //otherwise recurse on right side
  else {
    //printf("left: %d  -  leftAngle: %d  -  rightAngle: %d\n",accumulator1,leftBound,leftBound+span);
    return getPath((leftBound + span/2), span/2, distances);
  }
}

/**
 * Calculates error value based on how far the objects is
 * and how much of it is in the way of the robot
 */
double calcError(int angle) {
  int error;
  if (angle > 0 && angle < 180)
    error = angle;
  else
    error = -(359 - angle);
  return error;
}


/**
* Performs the PD control for getting around obstacles.
* @param error - the difference between the angle that we want
* to be pointed at versus where we are currently pointed at
*/
void roamPD(int error) {
  float derivative = 0;
	int highBound = 127;
	int lowBound = -40;
	int motorSpeed = 30;
  float KP = 1;
  float KD = .5;
  int baseSpeed = 30;

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

  chassisSet(baseSpeed + motorSpeed,baseSpeed - motorSpeed);
	//rightMotorSet((baseSpeed - motorSpeed));
	//leftMotorSet((baseSpeed + motorSpeed));
//  printf("RightMotorSet: %d\n",baseSpeed - motorSpeed);
//  printf("LeftMotorSet: %d\n",baseSpeed + motorSpeed);

	//Motor Bound Check
	if (getRightSpeed() < lowBound)
  {
  //  printf("Right Speed less than lowBound. Being set to lowBound\n");
    rightMotorSet(lowBound);
  }
	if (getLeftSpeed()  < lowBound)
  {
  //  printf("Left Speed less than lowBound. Being set to lowBound\n");
    leftMotorSet(lowBound);
  }
	if (getRightSpeed() > highBound)
  {
  //  printf("Right Speed greater than highBound. Being set to highBound\n");
    rightMotorSet(highBound);
  }
	if (getLeftSpeed()  > highBound)
  {
  //  printf("Left Speed greater than highBound. Being set to highBound\n");
    leftMotorSet(highBound);
  }
}


/**
* TODO: Finish the method

void getClearances() {
  int window;
  int clearances[90];
  int winStart = 0;
  int * dist = lidarGetDistances();
  int leftBound = 270;
  int rightBound = 40;
  //Find clearances
  for (int i = leftBound; i < ANGLES+rightBound-LENGTH_OF_BOT; i++) {
    clearances[i] = getLengthAverage(i);
  }
  //Find largest clearance
  int maxClearance = 0;
  int clearIndex;
  for (int i = 0; i < 90; i++)
    if (clearances[i] > maxClearance) {
      maxClearance = clearances[i];
      clearIndex = i;
    }
  int clearanceStatus = ensureClearance(clearIndex);
  //Calc error from clearance
}
*/


int pathClear() {
  return (calcClearance(315,0) || calcClearance(0,45));
}

/**
* Angle the robot to get around the object
*/
void makeCorrection() {
  //int furthestDist = getMax();

}

void roomba() {
  while (!(joystickGetDigital(CONTROLLER, RIGHT_BUTT_SET, RIGHT_BUTT))) {

    while (pathClear()) {
      chassisSet(30,30);
    }
    makeCorrection();
  }
}
