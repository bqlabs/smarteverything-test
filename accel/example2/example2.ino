
#include <Arduino.h>
#include <Wire.h>
#include <LSM9DS1.h>

//#define DEBUG

#define A_THRESHOLD  1500
#define A_EPSILON    50
#define Z_THRESHOLD  800

int state = -1;
// 0: initial state
// 1: data acquisition
// 2: impact state
// 3: lying state

float x = 0;
float y = 0;
float z = 0;

void setup() {
    #ifdef DEBUG
        SerialUSB.begin(115200);
    #endif

    Wire.begin();
    smeAccelerometer.begin();

    ledBlueLight(LOW);
    ledRedLight(LOW);
}

void loop() {

  if (isButtonOnePressed()) {
      ledBlueLight(HIGH);
      ledRedLight(LOW);
      state = 0;
  }

  if (isButtonTwoPressed()) {
      ledBlueLight(LOW);
      ledRedLight(LOW);
      state = -1;
  }

  switch(state) {
    case 0:
      // Initial state
      #ifdef DEBUG
        SerialUSB.println("State 0");
      #endif
      state = 1;
    break;
    case 1:
      // Data acquisition
      //SerialUSB.println("State 1");
      x = smeAccelerometer.readX();
      y = smeAccelerometer.readY();
      z = smeAccelerometer.readZ();
      #ifdef DEBUG
        SerialUSB.println(sqrt(x*x + y*y + z*z));
      #endif
      if (sqrt(x*x + y*y + z*z) >= A_THRESHOLD) {
        state = 2;
      }
    break;
    case 2:
      // Impact state
      #ifdef DEBUG
        SerialUSB.println("State 2");
      #endif
      // Wait to verify fall
      delay(2000);
      x = smeAccelerometer.readX();
      y = smeAccelerometer.readY();
      z = smeAccelerometer.readZ();
      #ifdef DEBUG
        SerialUSB.println(sqrt(x*x + y*y + z*z) - 980);
      #endif
      if (abs(sqrt(x*x + y*y + z*z) - 980) <= A_EPSILON) {
        state = 3;
      }
      else {
        state = 1;
      }
    break;
    case 3:
      // Lying state
      #ifdef DEBUG
        SerialUSB.println("State 3");
      #endif
      if (abs(smeAccelerometer.readZ()) >= Z_THRESHOLD) {
        #ifdef DEBUG
          SerialUSB.println("FALL");
        #endif
        ledRedLight(HIGH);
        ledBlueLight(LOW);
        state = -1;
      }
      else {
        state = 1;
      }
    break;
  }

  delay(50);
}
