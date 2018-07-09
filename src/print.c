#include "print.h"
#include "sensors.h"

void printDiagnostics(int sensors, int threshold, int whiteTape,
                      int error, int lastError, int threshLow) {
	printf("\n");
	printf("______________________________\n");
	printf("******CONTROL VARIABLES******\n");
	printf("Threshold: %d\n", threshold);
	printf("ThreshLow: %d\n", threshLow);
	printf("Sensors: %x\n", sensors);
	printf("Whitetape: %d\n", whiteTape);
	printf("Error: %d\n", error);
	printf("lastError: %d\n", lastError);
	printf("********SENSOR VALUES********\n");
	printf("Far Right: %d\n", frontRight);
	printf("Mid Right: %d\n", middleRight);
	printf("Center Right: %d\n", centerRight);
	printf("Far Left: %d\n", frontLeft);
	printf("Mid Left: %d\n", middleLeft);
	printf("Center Left: %d\n", centerLeft);
	printf("************MOTORS***********\n");
	printf("Left: %d\n", getRightSpeed());
	printf("Right: %d\n", getLeftSpeed());
}

void printSensors() {
  printf("\n********SENSOR VALUES********\n");
  printf("Far Right: %d\n", analogRead(FR));
  printf("Mid Right: %d\n", analogRead(MR));
  printf("Center Right: %d\n", analogRead(CR));
  printf("Far Left: %d\n", analogRead(FL));
  printf("Mid Left: %d\n", analogRead(ML));
  printf("Center Left: %d\n", analogRead(CL));
}
