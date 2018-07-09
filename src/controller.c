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

//int track = false;
int inRangeOnce = 1;
//int reps = 0;
int speed1 = 200;
char read[360 * SCAN_DATA_LEN];
char head[HEADER_LENGTH];
int readIndex = 0;
int pCount = 0;
char SCAN_HEAD[] = {0xA5, 0x5A, 0x05, 0x00, 0x00, 0x40, 0x81};


void leftBumpUpBotton() {

}

void leftBumpDownButton() {

}

void rightBumpUpButton() {

}

void rightBumpDownButton() {

}

void leftPadLeftButton(){
  int count = fcount(uart1);
  printf("Count: %d\n", count);
  delay(500);
  int numPoints = (count - 7) / 5;
  char *ans = CHAR_MAL(HEADER_LENGTH + SCAN_DATA_LEN * numPoints + 1);
  lidarScan(ans, &numPoints);

  for (int i = 0; i < SCAN_DATA_LEN * numPoints; i++) {
    if (!(i % 5) && i != 0)
      print("\n");
    printf("%x | ", ans[i]);
    delay(2);
  }
  print("\n");
  printf("Accepted Points: %d", numPoints);

  unsigned int *ians = UINT_MAL(SCAN_DATA_LEN * numPoints);

  decodeScanResponse(ans, ians, SCAN_DATA_LEN * numPoints);

  print("\n");
  delay(500);
  // free(ans);
}




void leftPadUpButton() {
  printf("Start\n");
  //lidarRequest();
  //fputc(SCAN, uart1);
  lidarExpressScan();
  delay(500);
}




void leftPadRightButton() {
  int printHead = true;
  // Grab Header if header is currently null
  if (head[1] != 0x5A) {
    // Wait for buffer to fill
    while (fcount(uart1) < 63)
      delay(1);

    // Grab header
    for (int i = 0; i < HEADER_LENGTH; i++) {
      head[i] = fgetc(uart1);
    }
  } else printHead = false;
  // Get response length
  int responseLen = (int)head[2];

  // Create response array (only one packet for now)
  char packet[responseLen];
  int packIndex = 0;
  while (!feof(uart1) && packIndex < responseLen) {
    // Wait for lidar buffer to fill
    while (fcount(uart1) < 63)
      delayMicroseconds(500);
    // Load packet
    packet[packIndex++] = fgetc(uart1);
  }

  // Print Header
  if (printHead) {
    print("Header: ");
    for (int i = 0; i < HEADER_LENGTH; i++) {
      printf("%x | ", head[i]);
    }
  }

  // Print Start info
  print("\nStart Info: \n");
  for (packIndex = 0; packIndex < 4; packIndex++) {
    printf("%x | ", packet[packIndex]);
  }

  // Print Cabins
  print("\n\nCabins:");
  for (int i = packIndex; i < responseLen; i++) {
    if (!((i - 4) % 5))
      printf("\n%d: ", (i - 4) / 5 + 1);
    printf("%x | ", packet[i]);
  }

  printf("\nCheckSum: %d\n",checkSum(packet));
  delay(500);
}






void leftPadDownButton() {
  int numPoints = 150 * SCAN_DATA_LEN;
  while (readIndex < numPoints && !feof(uart1)) {
    // Get number of bytes to read
    int count = fcount(uart1);
    printf("Count: %d\n", count);
    // If first index, take header into account
    if (!readIndex)
      count -= HEADER_LENGTH;
    // Calc how many data packets to read
    int numPoints = count / 5;
    int numBytes = numPoints * SCAN_DATA_LEN;
    char buf[numBytes]; //+1
    // Grab header
    if (!readIndex)
      fread(head, 1, HEADER_LENGTH, uart1);
    // Grab some packets
    int i = 0;
    while (i < numBytes) {
      buf[i] = fgetc(uart1);
      checkByteIntegrity(buf[i], &i);
      while (fcount(uart1) < 63)
        delayMicroseconds(500);
    }
    // Copy into data array
    if (readIndex < 0)
      readIndex = 0;
    for (int i = 0; i < numBytes; i++) {
      read[readIndex++] = buf[i];
    }
    if (feof(uart1))
      printf("End of File");

    // Wait for lidar buffer to fill
    while (fcount(uart1) < 63)
      delay(1);
  }

  // Print header
  print("Header: ");
  for (int i = 0; i < HEADER_LENGTH; i++)
    printf("%x | ", head[i]);

  // Print data packets
  print("\nData:\n");
  for (int i = 0; i < numPoints; i++) {
    if (!(i % 5) && i != 0)
      printf("\n%d: ", i / 5);
    printf("%x | ", read[i]);
    delay(2);
  }
  print("\n");

  // Check inegrity
  int p = numPoints / SCAN_DATA_LEN;
  checkHeader(SCAN_HEAD, head, SCAN_DATA_LEN, 0);
  checkScanIntegrity(read, &p);
  printf("Accepted Points: %d", p);

  unsigned int rArray[numPoints / SCAN_DATA_LEN];
  decodeScanResponse(read, rArray, numPoints);
  readIndex = -1;
  delay(500);
}





void rightPadLeftButton() {
  char test[] = {0xA5, 0x5A, 0x05, 0x00, 0x00, 0x40, 0x81, 0x01, 0x02,
                 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A};
  if (!checkHeader(SCAN_HEAD, test, SCAN_DATA_LEN, 2))
    printf("Head find failure :(");
  for (int i = 0; i < 10; i++) {
    printf("%x | ", test[i]);
  }
  delay(500);
}





void rightPadUpButton() {
      lidarReset();
}





void rightPadRightButton() {

}





void rightPadDownButton() {
  //track = true;
  // autonomous();
}
