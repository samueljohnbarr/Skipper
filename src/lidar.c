#include "lidar.h"
#include "main.h"
#include "tools.h"
#include <math.h>

// char * ans;
char *header;
char *data;
int lastDistance;

char SCAN_HEADER[]   = {0xA5, 0x5A, 0x05, 0x00, 0x00, 0x40, 0x81};

int scanning;

int initLidar() {
  usartInit(uart1, BAUD, SERIAL_8N1);
  header = CHAR_MAL(HEADER_LENGTH);
  motorSet(6, 127);
  scanning = false;
  return 1;
}

void lidarStop() {
  lidarRequest();
  fputc(STOP, uart1);
  scanning = false;
  delay(1);
}

void lidarReset() {
  lidarRequest();
  fputc(RESET, uart1);
  delay(2);
}

void lidarScan(char *r_Array, int * numPoints) {
  if (!scanning) {
    lidarRequest();
    fputc(SCAN, uart1);
  }
  delay(50);
  responseWait(SCAN_DATA_LEN, r_Array, *numPoints);
  delay(50);

  if (!scanning) {
    if (checkHeader(SCAN_HEADER, r_Array, SCAN_DATA_LEN, *numPoints) == -1) {
      print("Scan Header Read Failure");
    }
    scanning = true;
  }
  checkScanIntegrity(r_Array, numPoints);

  lidarStop();
}

void lidarExpressScan() {
  lidarRequest();
  fputc(EXPRESS_SCAN, uart1);
  fputc(0x05, uart1);
  fputc(0x00, uart1);
  fputc(0x00, uart1);
  fputc(0x00, uart1);
  fputc(0x00, uart1);
  fputc(0x00, uart1);
  fputc(0x22, uart1);
}
/*
char * lidarGetHealth() {
  lidarRequest();
  delay(2);
  fputc(GET_HEALTH, uart1);
  delay(2);
  return responseWait(HEALTH_DATA_LEN);
}

char * lidarGetInfo() {
  lidarRequest();
  fputc(GET_INFO, uart1);
  delay(1);
  return responseWait(GET_INFO_LEN);
}
*/
void lidarGetSamplerate(char *r_Array) {
  lidarRequest();
  fputc(GET_SAMPLERATE, uart1);
  responseWait(GET_SAMPLERATE, r_Array, GET_SAMPLERATE_LEN);
  delay(1);
}

void lidarRequest() { fputc(REQUEST, uart1); }

void responseWait(int responseLen, char *r_Array, int numPoints) {
  // Get Data
  fgets(r_Array, HEADER_LENGTH + numPoints * responseLen + 1, uart1);
}

void decodeExpressResponse(char *dataArray, unsigned int *rArray, int size) {}

int checkHeader(char * descripter, char * r_Array,
                                       int responseLen, int numPoints) {
  int size = HEADER_LENGTH + numPoints * responseLen +1;
  int hIndex;
  //Go through raw data
  for (int i = 0; i < size && hIndex != HEADER_LENGTH; i++) {
    //Find start of header
    if (r_Array[i] == descripter[0]) {
       //Consecutive header check
       hIndex = 1;
       for (int j = i+1; j < size && hIndex != HEADER_LENGTH; j++) {
         if (r_Array[j] == descripter[hIndex]) {
           hIndex++;
         }
         //False check : break
         else {
           hIndex = -1;
           break;
         }
       }
         if (hIndex == HEADER_LENGTH) hIndex = i;
     }
  }
  if (hIndex == -1) return false;
  //Strip off header off of raw
  header = descripter;
  for (int i = hIndex; i < size - HEADER_LENGTH; i++) {
    r_Array[i] = r_Array[i+HEADER_LENGTH];
    if (i + HEADER_LENGTH >= size - HEADER_LENGTH)
      r_Array[i+HEADER_LENGTH] = 0;
  }

  return true;
}

int checkByteIntegrity(char b, int * i) {
  int byteOrder = *i % 5;
  switch (byteOrder) {
    //Quality & S
    case 0:
      if (checkS(b)) //&& b != 0x02)
        *i += 1;
      else return false;
      break;
    //Angle1 & C
    case 1:
      if (checkC(b))
        *i += 1;
      else {
        *i -= 1;
        return false;
      }
      break;
    case 2:
      *i += 1;
      break;
    //Distance1
    case 3:
      lastDistance = b;
      *i += 1;
      break;
    //Distance2
    case 4:
      if(b == 0 && lastDistance == 0) {
        *i -= 4;
        return false;
      } else *i += 1;
      break;
  }
  return true;
}

void checkScanIntegrity(char * r_Array, int * numPoints) {
  int size = *numPoints * 5;
  int goodData = 0;
  int rIndex = 0;

  //Only keep packets with with valid S & C bits
  for (int i = 0; i < size-1; i++) {
    if (checkS(r_Array[i]) && checkC(r_Array[i+1])) {
      for (int j = rIndex; j < rIndex+5; j++)
        r_Array[j] = r_Array[i++];
      rIndex += 5;
      goodData++;
      i--;
    }
  }
  *numPoints = goodData;
}

int checkSum(char * packet) {
  char check = ((packet[1] & 0x07) << 3) + (packet[0] & 0x07);

  int checksum = ((0 ^ 0xA5) ^ 0x5A) ^ 0x54;
  int cabinStart = 4;

  for (int i = cabinStart; i < EXPRESS_SCAN_DATA_LEN; i++)
    checksum ^= packet[i];
  printf("\nGiven checksum:  %x  | Calculated checksum:  %x\n",check,checksum);
  return checksum == check;
}

void decodeScanResponse(char *dataArray, unsigned int *rArray, int size) {
  int elements = ceil(size / 5.0);
  int *quality = INT_MAL(elements);
  char *angles1 = CHAR_MAL(elements);
  char *angles2 = CHAR_MAL(elements);
  char *distances1 = CHAR_MAL(elements);
  char *distances2 = CHAR_MAL(elements);
  int dSize = size + 7;
  // Decode data array into corresponding byte arrays
  for (int i = 0; i < dSize; i++) {
    if (i < 7)
      header[i] = dataArray[i];
    else {
      // Quality byte
      if ((i % 5) == 2) quality[i / 5] = (int) dataArray[i];

      // Angle bytes
      if ((i % 5) == 3) angles1[i / 5] = dataArray[i];
      if ((i % 5) == 4) angles2[i / 5] = dataArray[i];

      // Distance bytes
      if ((i % 5) == 0) distances1[i / 5] = dataArray[i];
      if ((i % 5) == 1) distances2[i / 5] = dataArray[i];
    }
  }

  // Conjoin angle and distance arrays
  unsigned int *angles = UINT_MAL(elements);
  unsigned int *distances = UINT_MAL(elements);

  for (int i = 0; i < elements; i++) {
    // Join char arrays into integer array
    angles[i] = ((shiftAdd(angles2[i], angles1[i])) >> 1) / 64;
    distances[i] = (shiftAdd(distances2[i], distances1[i])) / 40;
  }

//  if (elements == ceil(size - 7)/5.0)
  // Conjoin angle and distance arrays into return array
  for (int i = 0; i < elements; i++) {
    if (angles[i] > 359) {
      printf("Angle out of bounds\n");
      printf("Angle: %u | Distance: %u\n\n", angles[i], distances[i]);
    } else {
      printf("Accepted Angle :)\n");
      printf("Quality: %u | Angle: %u | Distance: %u\n\n",
               quality[i], angles[i], distances[i]);
      rArray[angles[i]] = distances[i];
      delay(5);
    }
  }
  free(quality);
  free(angles1);
  free(angles2);
  free(distances1);
  free(distances2);
}

int checkS(char r) {
  return (((r & 0x02) && !(r & 0x01)) || (!(r & 0x02) && (r & 0x01)));
}

int checkC(char r) { return (r & 0x01); }

void printHeader() {
  print("Header : ");
  // printf("%x\n", header);
  for (int i = 0; i < HEADER_LENGTH; i++) {
    printf("%x | ", header[i]);
  }
  print("\n");
}
