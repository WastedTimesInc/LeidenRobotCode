//<--------------------INCLUDES-------------------->
// Arduino.h      -   Standard arduino library
// linefollow.h   -   Line following functions
// commonutils.h  -   Common utilities 
// globals.h      -   extern call for all global vars
#include <Arduino.h>
#include "linefollow.h"
#include "commonutils.h"
#include "globals.h"
#include "unittests.h"
#include "NewPing.h"

bool GLOBAL_VERBOSE = true;


//<--------------------MOTOR CTRL-------------------->
// Motor parameters section, containes control addresses,
// pins, stata variables, adresses
//
// CTRL_ADDR          -   I2C Motor control address
// CURRENT            -   Stores data currently written to I2C chip
// MOTOR_A(B)_STATE   -   State variables for motors, 0=off, 1=fwd, 2=rev
// MOTOR_A(B)_SPEED   -   Speed stat for motors, 0-255
// SPD_PIN_A(B)       -   PWM pins for motor control
// MASKS              -   All the necessary masks for motor control
const int CTRL_ADDR = 0x20;

uint8_t CURRENT = 0;

uint8_t MOTOR_A_STATE = 0;
uint8_t MOTOR_B_STATE = 0;

uint8_t MOTOR_A_SPEED = 0;
uint8_t MOTOR_B_SPEED = 0;

const uint8_t SPD_PIN_A = 5;
const uint8_t SPD_PIN_B = 6;

const uint8_t M_NULL = 0;
const uint8_t MOTOR_A_FWD = 0b00000001;
const uint8_t MOTOR_A_REV = 0b00000010;
const uint8_t MOTOR_A_MASK = 0b00000011;
const uint8_t MOTOR_A_INV_MASK = 255 - MOTOR_A_MASK;
const uint8_t MOTOR_B_FWD = 0b00000100;
const uint8_t MOTOR_B_REV = 0b00001000;
const uint8_t MOTOR_B_MASK = 0b00001100;
const uint8_t MOTOR_B_INV_MASK = 255 - MOTOR_B_MASK;


//<--------------------LOCATION AND PATH-------------------->
// These two variables locate us both along the path, and on the correct path
//
// LOCATION INDEX
// 0 = HOME
// 1 = INITIAL PATH BEFORE T JUNC
// 2 = T JUNC
// 3 = SECOND PATH BEFORE X JUNC
// 4 = X JUNC
// 5 = PATH TO END LINE
// 6 = END LINE
// 7 = ARM DEPLOY
// 8 = ARM RETRACT
// 9 = EXIT
//
// PATHDIR INDEX
// 0 = UNKNOWN;
// 1 = PATH A, INITIAL LEFT, THEN RIGHT
// 2 = PATH B, INITIAL RIGHT, THEN LEFT
int LOCATION = 0;
int PATHDIR = 0;


//<--------------------SENSOR ARRAYS-------------------->
// Two arrays that store sensor pins and sensor data
// 
// SENSOR_PINS    -   Stores sensor pins for line sensors
// SENSOR_STATE   -   Stores sensor state for line sensors
// US_PINS        -   Stores sensor pins for ultrasonics
// US_STATE       -   Stores sensor state for usltrasonics
const uint8_t SENSOR_PINS[6] = {A0, A1, A2, A3, 7, 4};
bool SENSOR_STATE[6] = {false, false, false, false, false, false};
const uint16_t SENSOR_THRESHOLD = 500;

const uint8_t US_PINS[2] = {12, 13};
uint16_t US_STATE[2] = {0, 0};
NewPing US_SENSORS[2] = {
  NewPing(US_PINS[0],US_PINS[0],200),
  NewPing(US_PINS[1],US_PINS[1],200)
};


void setup() {
  initSystems();
  testMotors();
}

void loop() {
  LOCATION = 1;
  dynamicFollow(4, 255, 50, 0.8);
  LOCATION = 2;
  PATHDIR = detectJunction(2);
  switch (PATHDIR) {
    case 1 : {
      leftTurn(50, 100);
      break;
    }
    case 2 : {
      rightTurn(50, 100);
      break;
    }
  }
  LOCATION = 3;
  highSpeedFollow(4, 255, 50, 0.4);
  LOCATION = 4;
  switch (PATHDIR) {
    case 1 : {
      rightTurn(50, 100);
      break;
    }
    case 2 : {
      leftTurn(50, 100);
      break;
    }
  }
  LOCATION = 5;
  dynamicFollow(3, 255, 50, 0.8);
}