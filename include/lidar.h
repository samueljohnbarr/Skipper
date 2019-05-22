#ifndef _LIDAR_H_
#define _LIDAR_H_

#define BAUD 115200     //Lidar BAUD rate
#define LIDAR_PIN 9     //Port number
#define FULL_COUNT 1800 //360 points, 5 bytes per point

#define REQUEST 0xA5
#define STOP 0x25
#define RESET 0x40
#define SCAN 0x20
#define SCAN_DATA_LEN 5
#define EXPRESS_SCAN 0x82
#define EXPRESS_SCAN_DATA_LEN 84
#define FORCE_SCAN 0x21
#define FORCE_SCAN_DATA_LEN 5
#define GET_INFO 0x50
#define GET_INFO_LEN 20
#define GET_HEALTH 0x52
#define HEALTH_DATA_LEN 3
#define GET_SAMPLERATE 0x59
#define GET_SAMPLERATE_LEN 4

#define HEADER_LENGTH 7


//char HEALTH_HEADER[] = {0xA5, 0x5A, 0x03, 0x00, 0x00, 0x00, 0x06};

/**
* Initialize the lidar for operation.
*/
int initLidar();

/**
* Stops operation of the lidar.
*/
void lidarStop();

/**
* Resets the lidar to it's initial starting state
*/
void lidarReset();

/**
* Performs a continuous scan with the lidar. Fills in the
* distances[angle] array with the distances at each angle.
*/
void lidarScan(void * n);

/**
* Returns a copy of the distance array.
* Distance array updated every 0.5 seconds
* @param array[360] to copy into
*/
int * lidarGetDistances();

/**
* Query's the lidar for the sample rate
*/
void lidarGetSamplerate();

/**
* Checks the c bit in the incoming scan package
*/
int lidarCheckC(char r);

/**
* Checks the S bits in the incoming scan package
*/
int lidarCheckS(char r);

/**
* Prints off the distances and angles found in the distances[angles] array
*/
void lidarPrintDistances();

/**
* Makes a request to the lidar. Used before every lidar command.
*/
void lidarRequest();

/**
* Returns true if the quadrant has obstacle in the way.
*/
int getQuadrant(int index);

int getMax();

/**
 * Fills in the gaps (zeros) of the distance vector
 * by replacing them with the closest non-zero points.
 * If two local points are non-zero, it picks the smallest one.
 * @param patched array of length 360 to fill with patched data
 */
void patchDistances(int * patched);

int getChecksum();

#endif
