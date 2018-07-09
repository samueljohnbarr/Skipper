#include "arm.h"
#include "chassis.h"
#include "lidar.h"
#include "lineTrack.h"
#include "main.h"
#include "memory.h"
#include "movement.h"
#include "print.h"
#include "tools.h"
#include "transfer.h"
#include "controller.h"
#include "wireless.h"

//int track = false;
//int inRangeOnce = 1;
//int reps = 0;
//int speed1 = 200;
char read[360 * SCAN_DATA_LEN];
char head[HEADER_LENGTH];
//int readIndex = 0;
//int pCount = 0;
extern char SCAN_HEADER;
int transmit = 0;
int send[2];

void initialize() {
  // Initialize ultrasonic sensor
  int inPort = 6;
  int outPort = 7;
  sonar = ultrasonicInit(outPort, inPort);
  digitalWrite(LED, 1);
  // read = CHAR_MAL(16);
  // Initialize shaft encoders
  // int portTop = 12;
  // int portBottom = 11;
  // rightEncoder = encoderInit(portTop, portBottom, false);
  // portTop = 2;
  // portBottom = 3;
  // leftEncoder = encoderInit(portTop, portBottom, true);
}

void initializeIO() {
  if (!initLidar()) printf("Lidar not properly initialized.");
  if (!initTransmitter()) printf("Transmitter not properly initialized.");
  //TaskHandle wireless = taskCreate(handshakeReceive,
  //  TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);
}

void operatorControl() {
  int power;
  int turn;
  while (1) {

    //Turn on LED indicator to signal that transmit is on or off.
    /*
    digitalWrite(LED,transmit);
    */

    /*______________________JOYSTICKS_________________________*/
    // LEFT: Claw Control (left axis)
    if (joystickGetAnalog(CONTROLLER, L_JOY_H)) {
      clawSet(joystickGetAnalog(CONTROLLER, L_JOY_H));
    }

    // RIGHT: Drive Control
    power = joystickGetAnalog(CONTROLLER, R_JOY_V);
    if (power > -25 && power < 25)
      power = 0;
    turn = joystickGetAnalog(CONTROLLER, R_JOY_H);
    if (turn > -25 && turn < 25)
      turn = 0;
    if (turn == 0 && power == 0) {
      elbowSet(0);
      shoulderSet(0);
    }
    // chassisSet(power + turn, power - turn);

    /*____________________LED Indication______________________*/

    digitalWrite(LED_RED,transmit); //Works, but you can hardly see them
    //in the day time.

    /*________________________BUMPERS_________________________*/
    // LEFT: Shoulder Control
    if (joystickGetDigital(CONTROLLER, LEFT_BUMP, UP_BUTT)) {
      transmit = !transmit;
      delay(500);
    }

    //if (joystickGetDigital(CONTROLLER, LEFT_BUMP, JOY_DOWN))



    // RIGHT: Elbow Control
    //if (joystickGetDigital(CONTROLLER, RIGHT_BUMP, JOY_UP))

    //if (joystickGetDigital(CONTROLLER, RIGHT_BUMP, JOY_DOWN))

    /*______________________RIGHT_BUTTONS_____________________*/
    // UP: *UNBOUND*
    if (joystickGetDigital(CONTROLLER, RIGHT_BUTT_SET, UP_BUTT)) {
      if (!transmit)
        rightPadUpButton();
      else {
        send[0] = RIGHT_BUTT_SET;
        send[1] = UP_BUTT;
        controllerSend(send);
      }
    }



    // DOWN: Autonomous (Hold)
    if (joystickGetDigital(CONTROLLER, RIGHT_BUTT_SET, DOWN_BUTT)) {
      if (!transmit)
        rightPadDownButton();
        else {
          send[0] = RIGHT_BUTT_SET;
          send[1] = DOWN_BUTT;
          controllerSend(send);
        }
    }

    // LEFT: *UNBOUND*
    if (joystickGetDigital(CONTROLLER, RIGHT_BUTT_SET, LEFT_BUTT)) {
      if (!transmit)
        rightPadLeftButton();
        else {
          send[0] = RIGHT_BUTT_SET;
          send[1] = LEFT_BUTT;
          controllerSend(send);
        }
    }
    // RIGHT: Program termination
    if (joystickGetDigital(CONTROLLER, RIGHT_BUTT_SET, RIGHT_BUTT)) {
      printf("Program terminate.\n");
      break;
    }

    /*______________________LEFT_BUTTONS_____________________*/
    // UP: Fold arm assembly
    if (joystickGetDigital(CONTROLLER, LEFT_BUTT_SET, UP_BUTT)) {
      if (!transmit)
        leftPadUpButton();
        else {
          send[0] = LEFT_BUTT_SET;
          send[1] = UP_BUTT;
          controllerSend(send);
        }
    }

    // DOWN: Extend arm assembly
    if (joystickGetDigital(CONTROLLER, LEFT_BUTT_SET, DOWN_BUTT)) {
      if (!transmit)
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
      if (!transmit)
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
      if (!transmit)
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
