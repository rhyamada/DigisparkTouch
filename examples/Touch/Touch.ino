// DigiTouch test and usage documentation
// CAUTION!!!! This does click things!!!!!!!!

#include <DigiTouch.h>

void setup() {
  DigiTouch.begin(); //start or reenumerate USB
}

void loop() {
  // If not using plentiful DigiTouch.delay(), make sure to call
  // DigiTouch.update() at least every 50ms  
  // clicks across whole the screen
  // these are signed chars
  int i = 0;
  for(; i < 32767; i+=100){
    DigiTouch.moveTo(i,i, 1);
    DigiTouch.delay(100);
    DigiTouch.moveTo(i,i, 3);
    DigiTouch.delay(100);
  }
  for(; i > 0; i-=100){
    DigiTouch.moveTo(i,i, 1);
    DigiTouch.delay(100);
    DigiTouch.moveTo(i,i, 3);
    DigiTouch.delay(100);
  }
}
