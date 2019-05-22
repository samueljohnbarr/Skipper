#include "main.h"
#include "tools.h"

#define SHIFT 7

/**
* Prints the binary of a given int, printed backwards.
*/
void printBinary(int n) {
//  n = flipByte(n);
  while(n) {
    if (n & 1) printf("1");
    else printf("0");
    n >>= 1;
  }
  printf("\n");
}

/**
* Flips individual bits. 7 with 0, 6 with 1, 5 with 2, 4 with 3
* i.e. 0110 1001 --> 1001 0110
*/
char flipByte(char b) {
  char r = 0;
  for (int i = 0; i < SHIFT; i++) {
    char temp = b & 0x01;
    r += temp;
    b >>= 1;
    r <<= 1;
  }
  return r;
}

/**
* Adds to char into the corresponding int. result = x bits followed by y bits
*/
int shiftBytes(char x, char y){
  int temp = x;
  for (int i = 0; i < 8; i++) {
    temp <<= 1;
    y >>= 7 - i;
    temp += y & 0x01;
  }
  return temp;
}

/**
* Performs same operation as shiftBytes.
*/
unsigned int shiftAdd(char i, char j) {
  unsigned int k = (unsigned int) i;
  unsigned int l = (unsigned int) j;
  k <<= 8;
  k &= 0xFF00;
  return k+l;
}
