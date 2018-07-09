#include "main.h"
#include "sonar.h"
#include "chassis.h"
#include "arm.h"
#include "transfer.h"

int range = 20;

void objectTransfer()
{
    //Retrieve Object
    clawRelease();
    extendArm();
    clawGrab();
    foldArm();
    //Move Object
    axisTurn(50, 1);
    //Place Object
    extendArm();
    clawRelease();
    //Return to initial state
    foldArm();
    clawGrab();
    axisTurn(50, 0);
}

int isInRange()
{
    if (sonar == NULL) printf("Sensor not initialized!\n");
    int val = ultrasonicGet(sonar);
    return (val <= 26 && val != -1);
}
