#include "main.h"
#include "chassis.h"
#include "sonar.h"

//sonar = ultrasonicInit(7, 6);

int getDistance()
{
   int distance = ultrasonicGet(sonar);
   return distance;
}
