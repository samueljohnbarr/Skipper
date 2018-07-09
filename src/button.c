#include "main.h"

static int pin;
int state;
int preset;
long int time;
long int lastPress;

void initButton(int p) {
  pin = p;
  state = 0;
  preset = 0;
}

void pressed() {
  state = 1;
  //If no press for >3 secs, reset preset
  if ((lastPress / 1000) >= 3)
    preset = 0;
  time = lastPress = millis();

}

int depressed() {
  state = 0;
  //Record time
  long int secs = (millis() - time) / 1000;
  //Preset 10 for long press
  if (secs >= 2)
    return 10;
  //Rack up consecutive presses
  if (secs <= 1)
    preset++;

  return preset;
}
