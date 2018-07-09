#ifndef SONAR_H_
#define SONAR_H_

#define CL 1
#define ML 6
#define FL 3
#define CR 2
#define MR 4
#define FR 5

int frontRight, middleRight, centerRight, frontLeft, middleLeft, centerLeft;

int getSensorsAverage(); //returns the average value of sensors
int* getCT(int cushion);
int sensorsDetection(int threshold, int cushion);

#endif //SONAR_H_
