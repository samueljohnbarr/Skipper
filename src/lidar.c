#include "lidar.h"
#include "main.h"
#include "tools.h"

#define F_RANGE 35
#define R_RANGE 55

char *header;
char *data;
int lastDistance;
char sampleRate[11];
int lidarDistances[ANGLES];
int scanning;
int maxDistance = 0;
int maxAngle;
int tempDistances[ANGLES];
int quadrant[8] = {0};
int localQuad[8] = {0};
char SCAN_HEADER[] = {0xA5, 0x5A, 0x05, 0x00, 0x00, 0x40, 0x81};


void clearQuads();
void setQuads();
void setQuadrant(int currAngle, int currDist);
/**
 * Gets raw information from lidar
 * and passes information to decodePacket
 */
int lidarGetScanResponse();

/**
 * Decodes a packet and stores value into
 * lidarDistances array
 */
void decodePacket(int quality, int angle, int angle1, int dist, int dist1);

/**
* Initialize lidar with constraints specified.
*/
int initLidar() {
  usartInit(uart1, BAUD, SERIAL_8N1);
  header = CHAR_MAL(HEADER_LENGTH);
  pinMode(LIDAR_PIN,OUTPUT);
  digitalWrite(LIDAR_PIN,HIGH);
  scanning = false;
  return 1;
}

int * lidarGetDistances() {
  //taskSuspend(lidar);
  for (int i = 0; i < ANGLES; i++)
    tempDistances[i] = lidarDistances[i];
  //taskResume(lidar);
  return tempDistances;
}

/**
* Sends a request to the lidar. Done before a specific request.
*/
void lidarRequest() { fputc(REQUEST, uart1); }

/**
* Stop the spinning of the lidar.
*/
void lidarStop() {
  lidarRequest();
  fputc(STOP, uart1);
  scanning = false;
  delay(1);
}

/**
* Reset lidar back to base settings. (?)
*/
void lidarReset() {
  lidarRequest();
  fputc(RESET, uart1);
  delay(2);
}

/**
* Perform a simple, multi-response scan.
*/
void lidarScan(void * n) {
  while (true) {
    //Dump anything in buffer
    while (fcount(uart1)) {
       //printf("Clearing Buffer...\n");
       fgetc(uart1);
     }
    //Request lidar for scanning
    //printf("Scanning\n");
    lidarRequest();
    fputc(SCAN, uart1);

    if (!lidarGetScanResponse())
      printf("Header scan failure\n");

    lidarStop();
  }
}

int lidarGetScanResponse() {
    clearQuads();
    int arrayLength = ANGLES * SCAN_DATA_LEN;
    int i = 0;
    //Verify scan header
    if (!scanning) {
      while (i < HEADER_LENGTH) {
        //printf("getting header...\n");
        char current = fgetc(uart1);
        if (current != SCAN_HEADER[i])
          return false;
        i++;
      }
      scanning = true;
    }

    //printf("getting packets...\n");
    int quality = 0;
    int angle = 0;
    int angle1 = 0;
    int dist = 0;
    int dist1 = 0;
    //Get packet
    for (i = 0; i < arrayLength; i++) {
      if (!(i % 5)) quality = fgetc(uart1);
      if ((i % 5) == 1) angle = fgetc(uart1);
      if ((i % 5) == 2) angle1 = fgetc(uart1);
      if ((i % 5) == 3) dist = fgetc(uart1);
      if ((i % 5) == 4) {
        dist1 = fgetc(uart1);
        decodePacket(quality, angle, angle1, dist, dist1);
        //if (localQuad[0]) quadrant[0] = localQuad[0]; //Only set when 1 (something in range on new scan)
      }
    }
    setQuads(); //Set the global quads value after full round of packet decoding
    return true;
}

void decodePacket(int quality, int angle, int angle1, int dist, int dist1) {

  int currQuality = quality >> 2;
  //Shift angle2 over to make room for angle1
  int currAngle = angle1 << 8;
  //Bitwise OR angle2 with angle 1, clearing off upper bits of angle1 first
  currAngle = currAngle | (0xFF & angle);
  //Shift resultant angle over 1 to clear of C bit
  currAngle = (currAngle >> 1) & 0x7FFF;
  //Shift distance2 over to make room for distance1
  int currDist = dist1 << 8;
  //Bitwise OR to get the distance together
  currDist = (currDist | ( 0xFF & dist));
  //Clear off upper bits
  currDist &= 0xFFFF;

  //Calculate actual angle and distance
  currAngle = currAngle/64;
  currDist = currDist/40;
  //currAngle = currAngle >> 32;
  //currDist = currDist >> 20;

  if (currAngle < FULL && currAngle >= 0) {
    //Only keep quality data
    if (currQuality == 15) {
      if (currDist < F_RANGE && currDist && (currAngle <= 90 || currAngle >= 270)) setQuadrant(currAngle, currDist); //&&
      if (currDist < R_RANGE && currDist && (currAngle > 90 && currAngle < 270)) setQuadrant(currAngle, currDist);
         //((currAngle >= 0 && currAngle <= 90) || (currAngle >= 270 && currAngle <= 360)))
         //localQuad[0] = 1;
      lidarDistances[currAngle] = currDist;
      if (currDist > maxDistance) {
        maxDistance = currDist;
        maxAngle = currAngle;
      }
    }
    else
      //If quality is shite, set distance to zero
      lidarDistances[currAngle] = 0;
  }
}

/**
* Returns true if the calculated checkSum is equal to
* that of the checksum provided.
*/
int lidarCheckSum(char * packet) {
  char check = ((packet[1] & 0x07) << 3) + (packet[0] & 0x07);

  int checksum = ((0 ^ 0xA5) ^ 0x5A) ^ 0x54;
  int cabinStart = 4;

  for (int i = cabinStart; i < EXPRESS_SCAN_DATA_LEN; i++)
    checksum ^= packet[i];
  printf("\nGiven checksum:  %x  | Calculated checksum:  %x\n",check,checksum);
  return checksum == check;
}

/**
* Returns true if the last to bits in the parameter
* are not the same.
*/
int lidarCheckS(char r) {
  return (((r & 0x02) && !(r & 0x01))
      || (!(r & 0x02) && (r & 0x01)));
}

/**
* Returns true if the last bit in the parameter is a 1.
*/
int lidarCheckC(char r) {
  return (r & 0x01);
}

void clearQuads() {
  for(int i = 0; i < 8; i++) {
    localQuad[i] = 0;
  }
}

void setQuads() {
  for(int i = 0; i < 8; i++) {
      quadrant[i] = localQuad[i];
  }
}

void setQuadrant(int currAngle, int currDist) {
  if ((currAngle >= 0 && currAngle <= 45)) {
     localQuad[0] = 1;
     return;
  }
  if ((currAngle > 45 && currAngle <= 90)) {
     localQuad[1] = 1;
     return;
  }
  if ((currAngle > 90 && currAngle <= 135)) {
     localQuad[2] = 1;
     return;
  }
  if ((currAngle > 135 && currAngle <= 180)) {
     localQuad[3] = 1;
     return;
  }
  if ((currAngle > 180 && currAngle <= 225)) {
     localQuad[4] = 1;
     return;
  }
  if ((currAngle > 225 && currAngle <= 270)) {
     localQuad[5] = 1;
     return;
  }
  if ((currAngle > 270 && currAngle <= 315)) {
     localQuad[6] = 1;
     return;
  }
  if ((currAngle > 315 && currAngle < 360)) {
     localQuad[7] = 1;
     return;
  }
}

int getQuadrant(int index) {
  //printf("Quadrant: %d\n", quadrant[0]);
  if (index >= 0 && index < 8)
    return quadrant[index];
  return -1;
}

void lidarPrintDistances() {
  int zeros = 0;
  for (int i = 0; i < FULL; i++) {
    if (lidarDistances[i] == 0) zeros++;
    printf("Angle: %d  |  Dist: %d\n", i, lidarDistances[i]);
    delay(20);
  }
  printf("Bad Data Points: %d\n", zeros);
}

/**
 * Fills in the gaps (zeros) of the distance vector
 * by replacing them with the closest non-negative points.
 * If two local points are both zero, it picks the smallest one.
 * @param distance array of length 360 to fill with patched data
 */
void patchDistances(int * patched) {
  int zero = 0;
  for (int i = 0; i < ANGLES; i++) {
    if (patched[i] == zero) {
      int f = i+1;
      int b = i-1;
      //Find closest non-zero distance
      while (patched[f%360] == zero && patched[b%360] == zero) {
        f++;
        b--;
      }
      //Determine smallest non-zero distance to fill zero
      if (!(patched[b%360])) {
        patched[i] = patched[f%360];
      }
      else if (!(patched[f%360])) {
        patched[i] = patched[b%360];
      }
      else if (patched[f%360] < patched[b%360]) {
        patched[i] = patched[f%360];
      }
      else
        patched[i] = patched[b%360];
    }
    else
      patched[i] = patched[i];
  }
}

int getChecksum() {
  print("getting checksum\n");
  int cs = 0x0;
  for (int i = 0; i < ANGLES; i++) {
    cs ^= lidarDistances[i];
  }
  return cs;
}

int getMax() {
  return maxAngle;
}
