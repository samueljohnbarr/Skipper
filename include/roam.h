#ifndef _ROAM_H_
#define _ROAM_H_


int isInRange(int heading);
int getHeading(int power, int turn);

/**
* Method that performs the autonomous roam operation.
*/
void roam();


int findHeading();

/**
* Method to get the desired angle out of a range of angles.
* Desired angle is one with the largest clearance and the greatest distance.
* @params:
* leftBound: the leftMost angle
* span: the range of angles
* distances: reference to memory that holds the distance values from lidar
*/
int getPath(int leftBound, int span, int * distances);








#endif
