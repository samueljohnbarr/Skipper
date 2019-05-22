#ifndef _ARM_H_
#define _ARM_H_

/**
 * Sets the claw motor speed
 * @param speed to set
 */
 void clawSet(int speed);

/**
 * Closes an open claw
 */
 void clawGrab();

 /**
  * Opens a closed claw
  */
  void clawRelease();

/**
 * Sets the shoulder motor to a certain speed
 * @param speed to set
 */
void shoulderSet(int speed);

/**
 * Sets the shoulder to specified potenometer setting
 */
void shoulderPotSet(int pot, int speed);

/**
 * Sets the elbow motor to a certain speed
 * @param speed to set
 */
void elbowSet(int speed);

/**
 * Sets the elbow to specified potentometer setting
 */
 void elbowPotSet(int pot, int speed);

/**
 * Sets all arm motors (shoulder & elbow) to a certain speed
 * @param speed to set
 */
void armSet(int speed);

/**
 * Returns value from the shoulder potentiometer
 */
int getShoulderPot();

/**
 * Returns value from the elbow potentiometer
 */
int getElbowPot();

/**
 * Raises arm to high position
 */
 void raiseArm();

 /**
  * Extends the arm at optimum grabbing position
  */
 void extendArm();

 /**
  * Folds the arm to resting point
  */
  void foldArm();


#endif //_ARM_H_
