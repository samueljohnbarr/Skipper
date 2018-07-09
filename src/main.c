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

char read[360 * SCAN_DATA_LEN];
char head[HEADER_LENGTH];
extern char SCAN_HEADER;
int transmit = 0;
int send[2];

void initialize() {
  // Initialize ultrasonic sensor
  int inPort = 6;
  int outPort = 7;
  sonar = ultrasonicInit(outPort, inPort);
  // Initialize shaft encoders
  int portTop = 12;
  int portBottom = 11;
  rightEncoder = encoderInit(portTop, portBottom, false);
  portTop = 2;
  portBottom = 3;
  leftEncoder = encoderInit(portTop, portBottom, true);
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
    digitalWrite(LED,transmit);

    /*______________________JOYSTICKS_________________________*/

    // LEFT: Unbound
    if (joystickGetAnalog(CONTROLLER, L_JOY_H)) {}

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
    chassisSet(power + turn, power - turn);

    /*____________________LED Indication______________________*/

    digitalWrite(LED_RED,transmit); //Works, but you can hardly see them
    //in the day time.

    /*________________________BUMPERS_________________________*/
    // LEFT UP: Unbound
    if (joystickGetDigital(CONTROLLER, LEFT_BUMP, UP_BUTT)) {}
    // LEFT DOWN: Unbound
    if (joystickGetDigital(CONTROLLER, LEFT_BUMP, DOWN_BUTT)) {}
    // RIGHT UP: Unbound
    if (joystickGetDigital(CONTROLLER, RIGHT_BUMP, JOY_UP)) {}
    // RIGHT DOWN: Unbound
    if (joystickGetDigital(CONTROLLER, RIGHT_BUMP, JOY_DOWN)) {}

    /*______________________RIGHT_BUTTONS_____________________*/
    // UP: Run recorded path
    if (joystickGetDigital(CONTROLLER, RIGHT_BUTT_SET, UP_BUTT)) {
      runPath();
    }
    // DOWN: Autonomous
    if (joystickGetDigital(CONTROLLER, RIGHT_BUTT_SET, DOWN_BUTT)) {
      autonomous();
    }
    // LEFT: Record a path
    if (joystickGetDigital(CONTROLLER, RIGHT_BUTT_SET, LEFT_BUTT)) {
      recordPath();
    }
    // RIGHT: Program termination
    if (joystickGetDigital(CONTROLLER, RIGHT_BUTT_SET, RIGHT_BUTT)) {
      printf("Program terminate.\n");
      break;
    }

    /*______________________LEFT_BUTTONS_____________________*/
    // UP: Unbound
    if (joystickGetDigital(CONTROLLER, LEFT_BUTT_SET, UP_BUTT)) {}
    // DOWN: Unbound
    if (joystickGetDigital(CONTROLLER, LEFT_BUTT_SET, DOWN_BUTT)) {}
    // LEFT: Unbound
    if (joystickGetDigital(CONTROLLER, LEFT_BUTT_SET, LEFT_BUTT)) {}
    // RIGHT: Delete saved path
    if (joystickGetDigital(CONTROLLER, LEFT_BUTT_SET, RIGHT_BUTT)) {
      deletePath();
    }


    /*_______________________________________________________*/

    delay(20);
  }
}

void autonomous() {
  // Wait for debounce
  delay(500);
  lineTrack();
}
