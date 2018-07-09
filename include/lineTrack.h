#ifndef _LINETRACK_H_
#define _LINETRACK_H_

#define RIGHT_FAR 0x01
#define RIGHT_MID 0x02
#define RIGHT_CENTER 0x04
#define LEFT_CENTER 0x08
#define LEFT_MID 0x10
#define LEFT_FAR 0x20

#define RFMCLFM  0x37
#define RFMLFMC  0x3B
#define RFMCLFMC 0x3F

/**
 * Loops through entire line track proceedure
 * Until down button on right button set is pressed
 */
void lineTrack();

#endif //_LINETRACK_H_
