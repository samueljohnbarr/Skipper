#ifndef _CHASSIS_H_
#define _CHASSIS_H_

/**
 * Sets both drive wheels to certain speeds
 * @param left drive speed
 * @param right drive speed
 */
void chassisSet(int leftSpeed, int rightSpeed);

/**
 * Turns the robot on axis 90 degrees
 * @param speed to turn
 * @param right or left turn
 */
 void axisTurn(int speed, int right);

/**
 * Sets the left dtive motor to a certain speed
 * @param speed to set
 */
void leftMotorSet(int speed);

/**
 * Sets the right drive motor to a certain speed
 * @param speed to set
 */
void rightMotorSet(int speed);

/**
 * @return right motor speed
 */
int getRightSpeed();

/**
 * @return left motor speed
 */
int getLeftSpeed();


#endif // _CHASSIS_H_
