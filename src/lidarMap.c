#include <math.h>
#include "main.h"
#include "lidar.h"
#include "tools.h"

/**
 * Maps points to x and y values in cooresponding arrays
 * Indexed based on angle
 */
int mapX[ANGLES];
int mapY[ANGLES];

int calcXPoint(int angle, int distance);
int calcYPoint(int angle, int distance);


void setMap(int * lidarDistances) {
    for (int angle = 0; angle < ANGLES; angle++) {
      int distance = lidarDistances[angle];
      //Set map points
      if (distance != 0) {
        mapX[angle] = calcXPoint(angle, distance);
        mapY[angle] = calcYPoint(angle, distance);
      }
      else {
        mapX[angle] = 0;
        mapY[angle] = 0;
      }
    }
}

int calcXPoint(int angle, int distance) {
  if (angle < 90)  return  ceil(distance * sin(angle * D_IN_RADS));
  if (angle < 180) return  ceil(distance * cos((angle - 90)* D_IN_RADS));
  if (angle < 270) return  ceil(-distance * sin((angle - 180)* D_IN_RADS));

  return ceil(-distance * cos((angle - 270)* D_IN_RADS));
}

int calcYPoint(int angle, int distance) {
  if (angle < 90)  return  ceil(distance * cos(angle* D_IN_RADS));
  if (angle < 180) return  ceil(-distance * sin((angle - 90)* D_IN_RADS));
  if (angle < 270) return  ceil(-distance * cos((angle - 180)* D_IN_RADS));

  return ceil(distance * sin((angle - 270)* D_IN_RADS));
}

int getXPoint(int angle) {
  if (angle < 360)
    return mapX[angle];
  else
    return -1;
}

int getYPoint(int angle) {
  if (angle < 360)
    return mapY[angle];
  else
    return -1;
}

void printMap() {
  for (int angle = 0; angle < ANGLES; angle++) {
    printf("Angle: %d | Point: ( %d , %d )\n", angle, mapX[angle], mapY[angle]);
    delay(10);
  }
}

void testPrint() {
  int distance = 43;
  int angle = 65;
  int dx = ceil(-distance * sin(angle* D_IN_RADS));
  int dy = ceil(distance * cos(angle* D_IN_RADS));
  printf("dx : %d  dy : %d \n",dx,dy);
  delay(500);
}


void printVisualMap() {
  int sortedX[ANGLES];
  int sortedY[ANGLES];
  //Copy and normalize mapY and mapX
  for (int i = 0; i < ANGLES; i++) {
    if (sortedY[i] >= -300 && sortedY[i] <= 300)
    sortedY[i] = (mapY[i] + 300)/6;
    sortedX[i] = (mapY[i] + 300)/6;
  }

  //Insertion Sort based on Y
  for (int i = 1; i < ANGLES; i++) {
    int indexY = sortedY[i];
    int indexX = sortedX[i];
    int j = i;
    while (j > 0 && sortedY[j-1] < indexY) {
      sortedY[j] = sortedY[j-1];
      sortedX[j] = sortedX[j-1];
      j--;
    }
    sortedY[j] = indexY;
    sortedX[j] = indexX;
  }

  int level;
  int lastY = -1;
  for (int angle = 0; angle < ANGLES; angle++) {
    //Set the top level
    if (angle == 0) level = sortedY[angle];
    //Stay on level until change
    int lastX = -1;
    while (sortedY[angle] == level && sortedY[angle] != lastY) {
      char formatter[3] = {'%', (char)(sortedX[angle]), 's'};
      if (sortedX[angle] != lastX) printf(formatter, "*");
      lastX = sortedX[angle];
      angle--;
    }
    level--;
    lastY = sortedY[angle];
    print("\n");
  }
}
