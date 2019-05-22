#ifndef _LIDARMAP_H_
#define _LIDARMAP_H_

/**
* Method used to fill in the mapX and mapY points.
* @param distance vector to map
*/
void setMap(int * lidarDistances);

/**
* Returns the distance in the x direction
*/
int getXPoint(int angle);

/**
* Returns the distance in the y direction
*/
int getYPoint(int angle);

/**
* Prints the X and Y points
*/
void printMap();

void testPrint();

void printVisualMap();

#endif
