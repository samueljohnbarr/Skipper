#include "chassis.h"
#include "lineTrack.h"
#include "main.h"

struct StampNode {
  int leftSpeed;
  int rightSpeed;
  long time;
  struct StampNode *next;
};

struct StampNode *head;
// Common-use iterator or temp node pointer
struct StampNode *itr;

int numStamps;

PROS_FILE *file;
char fileData[1024];

int leftSpeed;
int rightSpeed;

void writeFile() {
  file = fopen("memory.txt", "w");
  for (itr = head; itr->next != NULL; itr = itr->next) {
    fprintf(file, "%d %d %d\n", (int)itr->leftSpeed, (int)itr->rightSpeed,
            (int)itr->time);
  }

  fclose(file);
}

void loadFile() {
  file = fopen("memory.txt", "r");
  // fread(&loadedData, 2, fuck);
  fclose(file);
}

void addStamp(struct StampNode **headRef, int left, int right, long ti) {
  // Create new node and set values
  struct StampNode *last = (struct StampNode *)malloc(sizeof(struct StampNode));
  last->leftSpeed = left;
  last->rightSpeed = right;
  last->time = ti;
  last->next = NULL;

  //If head doesn't exist, set new node to it
  if (*headRef == NULL)
    *headRef = last;
  else {
    // If head exists, set itr to tail node
    itr = *headRef;
    while (itr->next != NULL)
      itr = itr->next;

    // Append to tail
    itr->next = last;
  }
  numStamps++;
}

void recordPath() {
  //Only record Stamp if speed changes or if head is null
  if (leftSpeed != getLeftSpeed() || rightSpeed != getRightSpeed() ||
      head == NULL) {
    leftSpeed = getLeftSpeed();
    rightSpeed = getRightSpeed();
    long ti = millis();
    addStamp(&head, leftSpeed, rightSpeed, ti);
  }
}

void runPath() {
  if (head != NULL) {
    for (itr = head; itr->next != NULL; itr = itr->next) {
      // Set speed from current stamp
      chassisSet(itr->leftSpeed, itr->rightSpeed);
      // Wait duration between next stamp
      wait(itr->next->time - itr->time);
    }
    // Stop that boy
    chassisSet(0, 0);
  } else
    printf("%s\n", "runPath::No recorded data to run.");
}

void deletePath() {
  while (head != NULL) {
    // Use itr as temp node
    itr = head;
    head = head->next;
    // itr trails behind head and frees the node
    free(itr);
  }
}

void printPath() {
  if (head != NULL) {
    printf("%s", "____________________________\n");
    for (itr = head; itr->next != NULL; itr = itr->next) {
      printf("LeftSpeed: %d\nRightSpeed: %d\nTime: %ld\n\n", itr->leftSpeed,
             itr->rightSpeed, itr->time);
    }
  } else
    printf("%s\n", "printPath::No data to show.");
}
