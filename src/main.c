#include "chassis.h"
#include "lidar.h"
#include "lineTrack.h"
#include "main.h"
#include "memory.h"
#include "movement.h"
#include "print.h"
#include "tools.h"
#include "controller.h"
#include "wireless.h"
#include "command.h"
#include "roam.h"

char read[360 * SCAN_DATA_LEN];
char head[HEADER_LENGTH];

//extern char SCAN_HEADER;
extern int currState;
int tmit = 0;
int send[] = {0,0};
int blockedHeading;

void initialize() {
  digitalWrite(LED, 1);
  pinMode(LED_RED,OUTPUT);

  int portTop = 1;
  int portBottom = 2;
  leftEncoder = encoderInit(portTop, portBottom, false);
  portTop = 3;
  portBottom = 4;
  rightEncoder = encoderInit(portTop, portBottom, false);

  initLidar();
  wireless = taskCreate(wirelessReceive, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);
  lidar = taskCreate(lidarScan, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);

}

void initializeIO() {
  if (!initLidar()) printf("Lidar not properly initialized.");
  if (!initTransmitter()) printf("Transmitter not properly initialized.");
}

void operatorControl() {
  int power;
  int turn;
  while (1) {
    //Turn on LED indicator to signal that transmit is on or off.
    /*
    digitalWrite(LED,state == IDLE);
    */

    /*______________________JOYSTICKS_________________________*/

    // RIGHT: Drive Control
    power = joystickGetAnalog(CONTROLLER, R_JOY_V);
    if (power > -25 && power < 25)
      power = 0;
    turn = joystickGetAnalog(CONTROLLER, R_JOY_H);
    if (turn > -25 && turn < 25)
      turn = 0;

    int heading = getHeading(power, turn);

    if (isInRange(heading)) {
      printf("In range!\n\n");
      blockedHeading = heading;
      chassisSet(0,0);
    }
    else
      chassisSet(power + turn, power - turn);
     /*
    if (heading != blockedHeading)
       chassisSet(power + turn, power - turn);
    else
       chassisSet(0,0);
    //chassisSet(power, power);
    */

   if (joystickGetDigital(CONTROLLER, LEFT_BUMP, JOY_DOWN)) {
     leftBumpDownButton();
     delay(500);
   }

    /*____________________LED Indication______________________*/

    //digitalWrite(LED_RED,currState == IDLE);

    /*________________________BUMPERS_________________________*/
    // LEFT: Shoulder Control
    if (joystickGetDigital(CONTROLLER, RIGHT_BUMP, UP_BUTT)) {
      //Pass chassis command
      rightBumpUpButton();
    }

    if (joystickGetDigital(CONTROLLER, RIGHT_BUMP, DOWN_BUTT)) {
      rightBumpDownButton();
    }

    // RIGHT: Elbow Control
    if (joystickGetDigital(CONTROLLER, LEFT_BUMP, JOY_UP))
      leftBumpUpButton();

    if (joystickGetDigital(CONTROLLER, LEFT_BUMP, JOY_DOWN))
      leftBumpDownButton();

    /*______________________RIGHT_BUTTONS_____________________*/
    // UP: *UNBOUND*
    if (joystickGetDigital(CONTROLLER, RIGHT_BUTT_SET, UP_BUTT)) {
      if (!tmit)
        rightPadUpButton();
      else {
        send[0] = RIGHT_BUTT_SET;
        send[1] = UP_BUTT;
        controllerSend(send);
      }
    }



    // DOWN: Autonomous (Hold)
    if (joystickGetDigital(CONTROLLER, RIGHT_BUTT_SET, DOWN_BUTT)) {
        rightPadDownButton();
    }

    // LEFT: *UNBOUND*
    if (joystickGetDigital(CONTROLLER, RIGHT_BUTT_SET, LEFT_BUTT)) {
      if (!tmit)
        rightPadLeftButton();
        else {
          send[0] = RIGHT_BUTT_SET;
          send[1] = LEFT_BUTT;
          controllerSend(send);
        }
    }
    // RIGHT: Program termination
    if (joystickGetDigital(CONTROLLER, RIGHT_BUTT_SET, RIGHT_BUTT)) {
      rightPadRightButton();
    }

    /*______________________LEFT_BUTTONS_____________________*/
    // UP: Fold arm assembly
    if (joystickGetDigital(CONTROLLER, LEFT_BUTT_SET, UP_BUTT)) {
      if (!tmit)
        leftPadUpButton();
        else {
          send[0] = LEFT_BUTT_SET;
          send[1] = UP_BUTT;
          controllerSend(send);
        }
    }

    // DOWN: Extend arm assembly
    if (joystickGetDigital(CONTROLLER, LEFT_BUTT_SET, DOWN_BUTT)) {
      if (!tmit)
        leftPadDownButton();
        else {
          send[0] = LEFT_BUTT_SET;
          send[1] = DOWN_BUTT;
          controllerSend(send);
        }
    }

    // RIGHT + LEFT: Delete Recorded Path
    if (joystickGetDigital(CONTROLLER, LEFT_BUTT_SET, RIGHT_BUTT) &&
        joystickGetDigital(CONTROLLER, LEFT_BUTT_SET, LEFT_BUTT)) {
      lidarStop();
      print("Stopped\n");
    }

    // LEFT: Record Path (Hold
    if (joystickGetDigital(CONTROLLER, LEFT_BUTT_SET, LEFT_BUTT)) {
      if (!tmit)
        leftPadLeftButton();
        else {
          send[0] = LEFT_BUTT_SET;
          send[1] = LEFT_BUTT;
          controllerSend(send);
        }
    }

    /* ________________________________________________
     *               EXPRESS SCAN PRINT                *
     * ________________________________________________*/
    if (joystickGetDigital(CONTROLLER, LEFT_BUTT_SET, RIGHT_BUTT)) {
      if (!tmit)
        leftPadRightButton();
        else {
          send[0] = LEFT_BUTT_SET;
          send[1] = RIGHT_BUTT;
          controllerSend(send);
        }
    }

    /*_______________________________________________________*/

    delay(20);
  }
}
