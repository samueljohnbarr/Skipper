#include "sensors.h"
#include "lineTrack.h"
#include "main.h"

int ct[2];

int getSensorsAverage() {
  frontRight = analogRead(FR);
  middleRight = analogRead(MR);
  centerRight = analogRead(CR);
  frontLeft = analogRead(FL);
  middleLeft = analogRead(ML);
  centerLeft = analogRead(CL);
  int average = frontRight + middleRight + centerRight + frontLeft + middleLeft + centerLeft;
  average = average / 6;
  return average;
}

int* getCT(int cushion) {
  frontRight = analogRead(FR);
  middleRight = analogRead(MR);
  frontLeft = analogRead(FL);
  middleLeft = analogRead(ML);
  ct[0] = frontRight + middleRight + frontLeft + middleLeft;
  ct[0] /= 4;

  switch(FR) {
    case FR: if (abs(frontRight - ct[0]) > cushion) cushion = abs(frontRight - ct[0]);
    case MR: if (abs(middleRight - ct[0]) > cushion) cushion = abs(middleRight - ct[0]);
    case FL: if (abs(frontLeft - ct[0]) > cushion) cushion = abs(frontLeft - ct[0]);
    case ML: if (abs(middleLeft - ct[0]) > cushion) cushion = abs(middleLeft - ct[0]);
    default: ct[1] = cushion;
  }
  return ct;
}

int sensorsDetection(int threshold, int cushion) {
	int val = 0;
	if (abs(analogRead(FR) - threshold) > abs(cushion - threshold))
	  val += RIGHT_FAR;
	if (abs(analogRead(MR) - threshold) > abs(cushion - threshold))
		val += RIGHT_MID;
	if (abs(analogRead(CR) - threshold) > abs(cushion - threshold))
		val += RIGHT_CENTER;
	if (abs(analogRead(CL) - threshold) > abs(cushion - threshold))
		val += LEFT_CENTER;
	if (abs(analogRead(ML) - threshold) > abs(cushion - threshold))
	  val += LEFT_MID;
	if (abs(analogRead(FL) - threshold) > abs(cushion - threshold))
		val += LEFT_FAR;
	return val;
}
