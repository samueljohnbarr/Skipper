#ifndef _MEMORY_H_
#define _MEMORY_H_

//typedef struct StampNode StampNode;
/**
 * Writes the path to a file
 */
void writeFile();

/**
 * Loads a file from memory that contains path information
 */
void loadFile();

/**
 * Adds a path 'stamp' to the tail of the stamp linked list
 * @param headRef reference to the head pointer
 * @param left speed
 * @param right speed
 * @param ti time (in milliseconds) of speed
 */
//void addStamp(struct StampNode** headRef, int left, int right, int ti);

/**
 * Inserts path 'stamps' if there is a change in speed
 */
void recordPath();

/**
 * Runs the recorded path if there is one
 */
void runPath();

/**
 * Resets the path and frees up space in memory
 */
void deletePath();

/**
 * Prints the linked list to terminal for debugging purposes
 */
 void printPath();


#endif //_MEMORY_H_
