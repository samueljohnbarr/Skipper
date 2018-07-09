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
