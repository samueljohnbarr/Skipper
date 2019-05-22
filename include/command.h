#ifndef _COMMAND_H_
#define _COMMAND_H_

/**
* Sends the chassis command to the specified uart port. Follow
* boolean is used to determine if the robot should perform the
* command or just pass it along.
*/
void chassisSend(int follow);

/**
* Sends the buttons pressed on the controller to the specified
* uart port.
*/
void controllerSend(int * payload);

/**
* Sends the number of centimeters to be moved by the receiving robot.
*/
void moveCmSend(int leftCm, int rightCm);

#endif
