/*
    Main paper: http://www.ncbi.nlm.nih.gov/pmc/articles/PMC4346101/
 */

#include <Arduino.h>
#include <Wire.h>
#include <LSM9DS1.h>

#define A_THRESHOLD  1500
#define A_EPSILON    50
#define T_EPSILON    40
#define Z_THRESHOLD  800

int state = -1;
// 0: initial state
// 1: data acquisition
// 2: impact state
// 3: lying state

float x = 0;
float y = 0;
float z = 0;

float g0x = 0;
float g0y = 0;
float g0z = 0;
float g1x = 0;
float g1y = 0;
float g1z = 0;

void setup() {
    Wire.begin();
    smeAccelerometer.begin();
    SerialUSB.begin(115200);

    ledBlueLight(LOW);
    ledRedLight(LOW);
}

//
// Q = M3 * M2 * Q1
//
// M3 = M((a3, b3, c3,  0))
// M2 = M((a2,  0,  0, d2))
// Q1 = V((a1, b1, c1,  0))
//
float theta() {

  // Q1
  float m0 = sqrt(g0x*g0x + g0y*g0y);
  float theta1 = atan(g0z / m0) / 2.;
  float a1 = cos(theta1);
  float b1 = sin(theta1) * -g0y / m0;
  float c1 = sin(theta1) * g0x / m0;

  // Q2
  float theta2 = (atan2(g1y, g1x) - atan2(g0y, g0x)) / 2.;
  float a2 = cos(theta2);
  float d2 = sin(theta2);

  // Q3
  float m1 = sqrt(g1x*g1x + g1y*g1y);
  float theta3 = - atan(g1z / m1) / 2.;
  float a3 = cos(theta3);
  float b3 = sin(theta3) * -g1y / m1;
  float c3 = sin(theta3) * g1x / m1;

  // Q
  float q0 = a1*a2*a3 - b1*(a2*b3 + c3*d2) - c1*(a2*c3 - b3*d2);
  float q1 = a1*(a2*b3 + c3*d2) + a2*a3*b1 - a3*c1*d2;
  float q2 = a1*(a2*c3 - b3*d2) + a2*a3*c1 + a3*b1*d2;
  float q3 = a1*a3*d2 - b1*(a2*c3 - b3*d2) + c1*(a2*b3 + c3*d2);

  return 2 * atan(sqrt(q1*q1 + q2*q2 + q3*q3) / q0) * 180 / 3.141592;
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
      SerialUSB.println("State 0");
      g0x = smeAccelerometer.readX();
      g0y = smeAccelerometer.readY();
      g0z = smeAccelerometer.readZ();
      state = 1;
    break;
    case 1:
      // Data acquisition
      //SerialUSB.println("State 1");
      x = smeAccelerometer.readX();
      y = smeAccelerometer.readY();
      z = smeAccelerometer.readZ();
      SerialUSB.println(sqrt(x*x + y*y + z*z));
      if (sqrt(x*x + y*y + z*z) >= A_THRESHOLD) {
        state = 2;
      }
    break;
    case 2:
      // Impact state
      SerialUSB.println("State 2");
      // Wait to verify fall
      delay(2000);
      x = smeAccelerometer.readX();
      y = smeAccelerometer.readY();
      z = smeAccelerometer.readZ();
      SerialUSB.println(sqrt(x*x + y*y + z*z) - 980);
      if (abs(sqrt(x*x + y*y + z*z) - 980) <= A_EPSILON) {
        state = 3;
      }
      else {
        state = 1;
      }
    break;
    case 3:
      // Lying state
      SerialUSB.println("State 3");
      g1x = smeAccelerometer.readX();
      g1y = smeAccelerometer.readY();
      g1z = smeAccelerometer.readZ();
      SerialUSB.println(abs(g1z));
      //if (abs(abs(theta()) - 90) <= T_EPSILON) {
      if (abs(g1z) >= Z_THRESHOLD) {
        SerialUSB.println("FALL");
        ledRedLight(HIGH);
        ledBlueLight(LOW);
        state = -1;
      }
      else {
        state = 1;
      }
    break;
  }

  delay(100);
}
