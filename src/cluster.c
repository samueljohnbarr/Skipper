#include "main.h"
#include "lidar.h"
#include "lidarMap.h"

struct Cluster *nullClust;

struct Cluster {
  int startX;
  int startY;
  int endX;
  int endY;
  float midY;
  float slope;
  float intercept;

  struct Cluster *next;
};
/**
 * Compares point at current index & index+1
 * If both points are close, calls create cluster on both points
 * If not, calls create cluster on single point at index
 * @param index to check
 */
int findCluster(int index);

/**
 * Creates a cluster object and appends to end of cluster list
 */
void createCluster(int x1, int x2, int y1, int y2);

/**
 * @param index to check
 * @return if point at index fits into current cluster
 */
int matchCluster(int index);


int thresh = 30; //???
struct Cluster  * headCluster = NULL;
struct Cluster * currentCluster = NULL; //Last Cluster

void initClusters(int startAngle, int span) {
  int index = 0;
  int currAngle = 0;
  while (index < span) {
    currAngle = (index+startAngle) % 360;
    printf("Finding clusters at point %d\n", currAngle);
    //On match, increment index (a point is consumed)
    if (matchCluster(currAngle)) {
      index++;
      currAngle = (index+startAngle) % 360;
    }
    else {
      //If no match, find two points that can create a cluster
      while(!findCluster(currAngle)) {
        index++;
        currAngle = (index+startAngle) % 360;
      }
      //Consume two points when cluster is found and created
      index += 2;
      delay(10);
    }
  }
}

int findCluster(int index) {
  printf("index: %d\n", index);
  //Probably should patch points before doing this
  int x1 = getXPoint(index);
  int y1 = getYPoint(index);

  //Reject if point is invalid
  if (x1 == 0  && y1 == 0)
    return false;

  int x2 = getXPoint(index+1);
  int y2 = getYPoint(index+1);

  //Calculate the distance between the two points
  float distance = sqrt(pow(x2-x1,2) + pow(y2-y1,2));
  printf("Two point distance: %f\n", distance);
  //Create a new cluster if distance is small
  if (distance <= thresh) {
    printf("Acceptable Distance!\n");
    createCluster(x1, x2, y1, y2);
    return true;
  }
  else {
    //Create cluster with a single point if distance is large
    printf("Distance too far\n");
    createCluster(x1, x1, y1, y1);
    return false;
  }
}

void createCluster(int x1, int x2, int y1, int y2) {
  printf("New Cluster: %d, %d, %d, %d\n\n", x1, x2, y1, y2);
  struct Cluster *clust = (struct Cluster *)malloc(sizeof(struct Cluster));
  //Calculate slope of cluster
  if ((x2-x1) == 0)
    clust->slope = INFINITY;
  else
    clust->slope = (float)(y2-y1)/(float)(x2-x1);

  //Find y-intercept
  clust->intercept = y1 - clust->slope * x1;
  clust-> next = NULL;

  //Insert Start & End Points
  clust -> startX = x1;
  clust -> startY = y1;
  clust -> endX = x2;
  clust -> endY = y2;
  clust -> midY = (y1+y2)/2;
  clust -> next = NULL;

  //Insert
  if (headCluster == NULL)
    headCluster = currentCluster = clust;
  else {
    //printf("Inserting Cluster, midY: %f\n", clust -> midY);
    currentCluster -> next = clust;
    currentCluster = clust;
  }
}

int matchCluster(int index) {
  if (headCluster == NULL)
    return false;

  int x = getXPoint(index);
  int y = getYPoint(index);

  //Reject if point is invalid
  if (x == 0 && y == 0)
    return false;

  int output = (currentCluster -> slope) * x + (currentCluster -> intercept);
  if (abs(output-y) < thresh) {
    currentCluster -> endX = x;
    currentCluster -> endY = y;
    printf("Point added to cluster\n\n");
    return true;
  }
  else {
     printf("Point rejected from cluster\n");
     return false;
   }
}

void printClusters() {
  if (headCluster == NULL) {
    print("Clusters not initialized!\n");
    return;
  }

  struct Cluster * pointer;
  print("***Clusters***\n");
  int acc = 0;
  for (pointer = headCluster; pointer != NULL; pointer = pointer -> next) {
    if (pointer -> slope == INFINITY)
      printf("x = %d\n", pointer -> startX);
    else
      printf("f(x) = %fx + %f\n", pointer -> slope, pointer -> intercept);

    printf("Start Point: (%d, %d)  |  End Point: (%d, %d) | MidY: %f\n\n", pointer -> startX,
            pointer -> startY, pointer -> endX, pointer -> endY, pointer -> midY);
    delay(200);
    acc++;
  }
  printf("Num Clusters: %d\n", acc);

}
