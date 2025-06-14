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
#include <math.h>

bool GLOBAL_VERBOSE = false;

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
// 7 = ANGLE REACHED
// 8 = ARM DEPLOY
// 9 = ARM RETRACT
// 10 = EXIT
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
const uint8_t SENSOR_PINS[6] = {7, 4, A0, A1, A2, A3};
bool SENSOR_STATE[6] = {false, false, false, false, false, false};
const uint16_t SENSOR_THRESHOLD = 500;

const uint8_t US_PINS[2] = {13, 12};
uint16_t US_STATE[2] = {0, 0};
NewPing US_SENSORS[2] = {
  NewPing(US_PINS[0],US_PINS[0],50),
  NewPing(US_PINS[1],US_PINS[1],50)
};

const uint8_t STEPPER_PINS[4] = {8,9,10,11};

uint16_t ANGLE_TARGET = 600; 

void setup() {
  //GLOBAL_VERBOSE = true;
  initSystems();
}

void loop() {
  readSensors();

  //LINE FOLLOW 1
  int correct = 0;
  bool straight = true;
  LOCATION = 1;
  while (!SENSOR_STATE[4] && !SENSOR_STATE[5]) {
    readSensors();
    straight = true;
    if ((!SENSOR_STATE[2] && !SENSOR_STATE[3]) || (SENSOR_STATE[2] && SENSOR_STATE[3])) {
      writeMotor(1,150,1,150);
    }else if (SENSOR_STATE[2]) {
      writeMotor(0,2,210+correct);
      writeMotor(1,1,180);
      straight = false;
    } else if (SENSOR_STATE[3]) {
      writeMotor(1,2,210+correct);
      writeMotor(0,1,180);
      straight= false;
    }

    if (straight) {
      correct = 0;
    } else {
      correct+= 1-(correct>37);
      delay(10);
    }
    readSensors();
    if (PATHDIR == 0) {
      if (SENSOR_STATE[1]) {
        PATHDIR = 1;
      }
      if (SENSOR_STATE[2]) {
        PATHDIR = 2;
      }
    }
  }
  correct = 0;
  straight = true;

  //TURN 1
  LOCATION = 2;
  if (PATHDIR == 1) {
    blindMove(2,255,200);
    leftTurn(230, 230, 10, 200, 0.8, 0.4);
  } else if (PATHDIR == 2){
    blindMove(2,255,200);
    rightTurn(230, 230, 10, 200, 0.8, 0.4);
  }

  //LINE FOLLOW 2
  correct = 0;
  straight = true;
  LOCATION = 3;
  
  for (int i = 0; i < 80; i++) {
    readSensors();
    straight = true;
    if ((!SENSOR_STATE[2] && !SENSOR_STATE[3]) || (SENSOR_STATE[2] && SENSOR_STATE[3])) {
      writeMotor(1,180,1,180);
    }else if (SENSOR_STATE[2]) {
      writeMotor(0,2,180);
      writeMotor(1,1,180+correct);
      straight = false;
    } else if (SENSOR_STATE[3]) {
      writeMotor(1,2,180);
      writeMotor(0,1,180+correct);
      straight= false;
    }

    if (straight) {
      correct = 0;
    } else {
      correct+= 1-(correct>20);
      //delay(15);
    }
    delay(10);
  }
  readSensors();
  while (!SENSOR_STATE[0] && !SENSOR_STATE[1]) {
    readSensors();
    straight = true;
    if ((!SENSOR_STATE[2] && !SENSOR_STATE[3]) || (SENSOR_STATE[2] && SENSOR_STATE[3])) {
      writeMotor(1,180,1,180);
    }else if (SENSOR_STATE[2]) {
      writeMotor(0,2,180+correct);
      writeMotor(1,1,180+correct);
      straight = false;
    } else if (SENSOR_STATE[3]) {
      writeMotor(1,2,180+correct);
      writeMotor(0,1,180+correct);
      straight= false;
    }

    if (straight) {
      correct = 0;
    } else {
      correct+= 1-(correct>40);
    }
    delay(10);
  }
  while (!SENSOR_STATE[4] && !SENSOR_STATE[5]) {
    readSensors();
    straight = true;
    if ((!SENSOR_STATE[2] && !SENSOR_STATE[3]) || (SENSOR_STATE[2] && SENSOR_STATE[3])) {
      writeMotor(1,130,1,130);
    }else if (SENSOR_STATE[2]) {
      writeMotor(0,2,120+correct);
      writeMotor(1,1,120+correct);
      straight = false;
    } else if (SENSOR_STATE[3]) {
      writeMotor(1,2,120+correct);
      writeMotor(0,1,120+correct);
      straight= false;
    }

    if (straight) {
      correct = 0;
    } else {
      correct+= 1-(correct>40);
    }
    delay(10);
  }
  correct = 0;
  straight = true;
  blindMove(2,255,150);
  writeMotor(0,0,0,0);

  /*if (!SENSOR_STATE[4] && SENSOR_STATE[5]) {
    writeMotor(1,160,0,0);
    while (!SENSOR_STATE[4]) {
      readSensors();
    }
  } else if (!SENSOR_STATE[5] && SENSOR_STATE[4]) {
    writeMotor(0,0,1,160);
    while (!SENSOR_STATE[5]) {
      readSensors();
    }
  }*/

  //TURN 2
  LOCATION == 4;
  /*if (PATHDIR == 1) {
    //blindMove(2,255,100);
    rightTurn(180, 180, 10, 250, 1, 1);
  } else if (PATHDIR == 2) {
    //blindMove(2,255,100);
    leftTurn(180, 180, 10, 250, 1, 1);
  }*/

  if (PATHDIR == 1) {
    writeMotor(1,180,2,220);
    while(!SENSOR_STATE[0]) {
      readSensors();
    }
    while(SENSOR_STATE[0]) {
      readSensors();
    }
    while(!SENSOR_STATE[0]) {
      readSensors();
    }
    writeMotor(0,0,1,180);
    delay(20);
    while(!SENSOR_STATE[3]){
      readSensors();
    }
  } else if (PATHDIR == 2) {
    writeMotor(2,220,1,180);
    while(!SENSOR_STATE[1]) {
      readSensors();
    }
    while(SENSOR_STATE[1]) {
      readSensors();
    }
    while(!SENSOR_STATE[1]) {
      readSensors();
    }
    writeMotor(1,180,0,0);
    delay(20);
    while(!SENSOR_STATE[2]){
      readSensors();
    }
  }
  //LINE FOLLOW 3
  correct = 0;
  straight = true;
  LOCATION = 5;

  for (int i = 0; i < 200; i++) {
    readSensors();
    straight = true;
    if ((!SENSOR_STATE[2] && !SENSOR_STATE[3]) || (SENSOR_STATE[2] && SENSOR_STATE[3])) {
      writeMotor(1,150,1,150);
    }else if (SENSOR_STATE[2]) {
      //writeMotor(0,2,255);
      //writeMotor(1,1,255);
      //delay(5);
      writeMotor(0,2,210+correct);
      writeMotor(1,1,210+correct);
      straight = false;
    } else if (SENSOR_STATE[3]) {
      //writeMotor(0,1,255);
      //writeMotor(1,2,255);
      //delay(5);
      writeMotor(1,2,210+correct);
      writeMotor(0,1,210+correct);
      straight= false;
    }

    if (straight) {
      correct = 0;
    } else {
      correct+= 1-(correct>37);
      //delay(15);
    }
    //delay(20);
  }
  readSensors();
  while (!SENSOR_STATE[0] || !SENSOR_STATE[1]) {
    readSensors();
    straight = true;
    if ((!SENSOR_STATE[2] && !SENSOR_STATE[3]) || (SENSOR_STATE[2] && SENSOR_STATE[3])) {
      writeMotor(1,150,1,150);
    }else if (SENSOR_STATE[2]) {
      //writeMotor(0,2,255);
      //writeMotor(1,1,255);
      //delay(5);
      writeMotor(0,2,210+correct);
      writeMotor(1,1,210+correct);
      straight = false;
    } else if (SENSOR_STATE[3]) {
      //writeMotor(0,1,255);
      //writeMotor(1,2,255);
      //delay(5);
      writeMotor(1,2,210+correct);
      writeMotor(0,1,210+correct);
      straight= false;
    }

    if (straight) {
      correct = 0;
    } else {
      correct+= 1-(correct>32);
      
    }
    delay(20);
  }
  writeMotor(0,0,0,0);
  /*correct = 0;
  straight = true;
  writeMotor(2,255,2,255);
  delay(300);
  writeMotor(0,0,0,0);
  delay(300);
  readSensors();
  if (!SENSOR_STATE[0] && SENSOR_STATE[1]) {
    writeMotor(1,160,0,0);
    while (!SENSOR_STATE[0]) {
      readSensors();
    }
  } else if (!SENSOR_STATE[1] && SENSOR_STATE[0]) {
    writeMotor(0,0,1,160);
    while (!SENSOR_STATE[1]) {
      readSensors();
    }
  }
  writeMotor(0,0,0,0);
  delay(500);
  //ANGLE
  LOCATION = 6;
  readSensors();
  writeMotor(1,130,1,130);
  while(!SENSOR_STATE[2] && !SENSOR_STATE[3]) {
    readSensors();
  }/*
  writeMotor(0,0,0,0);







  //readSensors();
  /*while(!SENSOR_STATE[4] && !SENSOR_STATE[5]) {
    readSensors();
    if (PATHDIR == 1) {
      writeMotor(0,0,2,255);
      delay(15);
      writeMotor(0,0,0,0);
      delay(45);
    } else if (PATHDIR == 2) {
      writeMotor(2,255,0,0);
      delay(15);
      writeMotor(0,0,0,0);
      delay(45);
    }
  }
  blindMove(1,130,200);
  writeMotor(2,220,2,220);
  delay(300);
  writeMotor(0,0,0,0);
  readSensors();
  while (!SENSOR_STATE[0] && !SENSOR_STATE[1]) {
    readSensors();
    if (PATHDIR == 1) {
      writeMotor(1,255,2,255);
      delay(12);
      writeMotor(0,0,0,0);
      delay(45);    
    } else if (PATHDIR == 2) {
      writeMotor(2,255,1,255);
      delay(12);
      writeMotor(0,0,0,0);
      delay(45);
    }
  }
  writeMotor(1,140,1,140);
  delay(600);
  writeMotor(0,0,0,0);*/

  //EXTEND ARM
  LOCATION = 7;
  writeStepper(30,3500,false);
  writeStepper(255,2000,false);
  writeStepper(30,3500,false);
  writeStepper(320,2000,false);
  delay(500);

  //RETRACT ARM
  LOCATION = 8;
  /*writeStepper(320,2000,true);
  writeStepper(30,3500,true);
  writeStepper(255,2000,true);
  writeStepper(30,3500,true);*/
  LOCATION = 9;

  //EXIT
  LOCATION = 10;
  while (true);



  /*readSensors();
  straight = true;
  if(SENSOR_STATE[2]){
    writeMotor(0,2,180+correct);
    writeMotor(1,1,180+correct);
    straight = false;
  }else{
    writeMotor(0,1,200);
  }
  if(SENSOR_STATE[3]){
    writeMotor(1,2,180+correct);
    writeMotor(0,1,180+correct);
    straight= false;
  }else{
    writeMotor(1,1,200);
  }
  if(SENSOR_STATE[2]&&SENSOR_STATE[3]){
    //writeMotor(1,200,1,200);
  }
  if(SENSOR_STATE[4]&&PATHDIR==0){
    PATHDIR=1;
    leftTurn(190,190,1,1);
  }else if(SENSOR_STATE[5]&&PATHDIR==0){
    PATHDIR=2;
    rightTurn(190,190,1, 1);
  }else if(LOCATION!=5&&SENSOR_STATE[4]&&PATHDIR==2){
    LOCATION = 5;
    leftTurn(190,190,1, 1);
  }else if(LOCATION!=5&&SENSOR_STATE[5]&&PATHDIR==1){
    LOCATION = 5;
    rightTurn(190,190,1, 1);
  }else{
    delay(1); 
  }
  if(straight){
    correct =0;
  }else{
    correct+= 1-(correct>75);
    delay(10);
  }*/
}