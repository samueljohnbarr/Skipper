#include "wireless.h"
#include "main.h"

/*
* Sends the chassis commands to the robot that is receiving the commands.
* If the follow conditional is met then this robot will perform the
* same commands, on top of sending the commands. Performs these actions
* as long as the back button (right button set, right button) has not
* been pressed.
*/
void chassisSend(int follow) {
 if (transmitRequest(true)) {
  int power, turn;
  int payloadSize = 0x02;
  int payload[2];
    //Continue until button is pressed
    while (!joystickGetDigital(CONTROLLER, RIGHT_BUTT_SET, RIGHT_BUTT)) {
      //Package payload
      power = joystickGetAnalog(CONTROLLER, R_JOY_V);
      if (power > -25 && power < 25) power = 0;
      turn = joystickGetAnalog(CONTROLLER, R_JOY_H);
      if (turn > -25 && turn < 25) turn = 0;
      payload[0] = power+turn;
      payload[1] = power-turn;

      //Transmit payload
      wirelessSend(CHASSIS_CMD, payload, payloadSize);

      //Copy movement if needed
      if (follow) chassisSet(power+turn,power-turn);

      //Check premature termination
      int ans = 0;
      if (fcount(uart2))
        ans = fgetc(uart2);
      if (ans == OVER) {
        printf("Premature Termination\n");
        break;
      }
    }
    terminateTransmission();
  }
}

void moveCmSend(int leftCm, int rightCm) {
  int payloadSize = 0x02;
  int payload[2];
  if (transmitRequest(false)) {
    //Package payload
    payload[0] = leftCm;
    payload[1] = rightCm;

    //Transmit payload
    wirelessSend(MOVECM_CMD, payload, payloadSize);
  }
  terminateTransmission();
}

/*
* Sends the buttons being pressed on the controller through the
* the wireless transmitter by calling upon wirelessSend(). Used
* in order to control the other robot with only one remote.
*/
void controllerSend(int * payload) {
  int controllerPayloadSize = 0x02;
  wirelessSend(CONTROLLER_CMD, payload, controllerPayloadSize);
}
