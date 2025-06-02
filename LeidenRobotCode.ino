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
  NewPing(US_PINS[0],US_PINS[0],200),
  NewPing(US_PINS[1],US_PINS[1],200)
};

const uint8_t STEPPER_PINS[4] = {8,9,10,11};

uint16_t ANGLE_TARGET = 600; 

void setup() {
  //GLOBAL_VERBOSE = true;
  initSystems();
}

void loop() {
  //<---------------INIT AND PATHDIR SELECTION--------------->
  readSensors();                                                                                    //Read line sensors
  readUltrasonic();                                                                                 //Read ultrasonic sensors
  if (US_STATE[0] != 0 && US_STATE[1] == 0) {                                                       //PATHDIR 1 if left reads a value and right reads 0(out of range);
    PATHDIR = 1;                                                                                    //Set PATHDIR to 1
  } else if (US_STATE[1] != 0 && US_STATE[0] == 0) {                                                //PATHDIR 2 if right reads a value and left reads 0(out of range);
    PATHDIR = 2;                                                                                    //Set PATHDIR to 2
  }



  //<------------------------------LINE FOLLOW 1------------------------------>
  //This first line follow is done in two parts to avoid false triggers or 
  //trigger misses at the T junction.
  //It works as follows:
  //  Follow the line normally until a trigger condition is met on a front
  //  sensor (depending on PATHDIR set earlier). Once this condition is met
  //  a second line follow takes over, using only one sensor, and using a
  //  biased cruise to make sure to hook back onto the line until a rear
  //  sensor triggers (again depending on PATHDIR) which concludes this
  //  initial line follow
  //
  int correct = 0;                                                                                  //Init correction counter                          
  bool straight = true;                                                                             //Init straight boolean
  LOCATION = 1;                                                                                     //LOCATION 1 = Initial path before T
  switch (PATHDIR) {                                                                                //First half line follow, switch on PATHDIR
    case 1 : {                                                                                      //For PATHDIR 1, follow the line until the front left triggers
      while (!SENSOR_STATE[0]) {                                                                    //Verify trigger condition
        readSensors();                                                                              //Read line sensors
        straight = true;                                                                            //Reset straight to true
        if ((!SENSOR_STATE[2] && !SENSOR_STATE[3]) || (SENSOR_STATE[2] && SENSOR_STATE[3])) {       //Cruise speed if both CL and CR see white, or both see black
          writeMotor(1,180,1,180);                                                                  //Write cruise speed
        }else if (SENSOR_STATE[2]) {                                                                //If left sensors sees black (and by definition right sees white)
          writeMotor(0,2,180+correct*2);                                                            //Inverse direction correction on the inner wheel
          writeMotor(1,1,130);                                                                      //Reduced fixed speed on outer wheel
          straight = false;                                                                         //Set straight to false in order to increment counter
        } else if (SENSOR_STATE[3]) {                                                               //If right sensors sees black (and by definition left sees white)
          writeMotor(1,2,180+correct*2);                                                            //Inverse direction correction on the inner wheel
          writeMotor(0,1,130);                                                                      //Reduced fixed speed on outer wheel
          straight= false;                                                                          //Set straight to false in order to increment counter
        }

        if (straight) {                                                                             //If straight is true (ie normal cruise)
          correct = 0;                                                                              //Reset correction counter
        } else {                                                                                    //Else (ie straight is false)
          correct+= 1-(correct>37);                                                                 //Increment correction counter (oflow safe)
          delay(10);                                                                                //Small loop delay to allow correction to properly happen
        }
      }
      correct = 0;                                                                                  //Once exited, reset correct
      straight = true;                                                                              //And reset straight
      break;                                                                                        //Break switch
    }
    case 2 : {                                                                                      //For PATHDIR 2, follow the line until the front left triggers
      while (!SENSOR_STATE[1]) {                                                                    //Verify trigger condition
        readSensors();                                                                              //Read line sensors
        straight = true;                                                                            //Reset straight to true
        if ((!SENSOR_STATE[2] && !SENSOR_STATE[3]) || (SENSOR_STATE[2] && SENSOR_STATE[3])) {       //Cruise speed if both CL and CR see white, or both see black
          writeMotor(1,180,1,180);                                                                  //Write cruise speed
        }else if (SENSOR_STATE[2]) {                                                                //If left sensors sees black (and by definition right sees white)
          writeMotor(0,2,180+correct*2);                                                            //Inverse direction correction on the inner wheel
          writeMotor(1,1,130);                                                                      //Reduced fixed speed on outer wheel
          straight = false;                                                                         //Set straight to false in order to increment counter
        } else if (SENSOR_STATE[3]) {                                                               //If right sensors sees black (and by definition left sees white)
          writeMotor(1,2,180+correct*2);                                                            //Inverse direction correction on the inner wheel
          writeMotor(0,1,130);                                                                      //Reduced fixed speed on outer wheel
          straight= false;                                                                          //Set straight to false in order to increment counter
        }

        if (straight) {                                                                             //If straight is true (ie normal cruise)
          correct = 0;                                                                              //Reset correction counter
        } else {                                                                                    //Else (ie straight is false)
          correct+= 1-(correct>37);                                                                 //Increment correction counter (oflow safe)
          delay(10);                                                                                //Small loop delay to allow correction to properly happen
        }
      }
      correct = 0;                                                                                  //Once exited, reset correct
      straight = true;                                                                              //And reset straight
      break;                                                                                        //Break switch
    }
  }

  switch (PATHDIR) {                                                                                //Second half line follow, switch on PATHDIR
    case 1: {                                                                                       //For PATHDIR 1, follow the line until the rear left triggers
      while (!SENSOR_STATE[4]) {                                                                    //Verify trigger condition
        readSensors();                                                                              //Read line sensors
        straight = true;                                                                            //Reset straight to true
        if (!SENSOR_STATE[3]) {                                                                     //If center right sees white
          writeMotor(1,150,1,180);                                                                  //Write cruise biased left to make sure to turn back onto the line
        } else if (SENSOR_STATE[3]) {                                                               //If center right sees black
          writeMotor(1,2,180+correct*2);                                                            //Write inversed correction to inner motor
          writeMotor(0,1,130);                                                                      //Write fixed lower speed to outer motor
          straight= false;                                                                          //Set straight to false
        }
        if (straight) {                                                                             //If straight is true (ie normal cruise)
          correct = 0;                                                                              //Reset correction counter
        } else {                                                                                    //Else (ie straight is false)
          correct+= 1-(correct>37);                                                                 //Increment correction counter (oflow safe)
          delay(10);                                                                                //Small loop delay to allow correction to properly happen
        }
      }
      correct = 0;                                                                                  //Once exited, reset correct
      straight = true;                                                                              //And reset straight
      break;                                                                                        //Break switch
    }
    case 2: {                                                                                       //For PATHDIR 2, follow the line until the rear right triggers
      while (!SENSOR_STATE[5]) {                                                                    //Verify trigger conditino
        readSensors();                                                                              //Read line sensors
        straight = true;                                                                            //Reset straight to true
        if (!SENSOR_STATE[2]) {                                                                     //If center left sees white
          writeMotor(1,180,1,150);                                                                  //Write cruise biased right to make sure to turn back onto the line
        }else if (SENSOR_STATE[2]) {                                                                //If center right sees black
          writeMotor(0,2,180+correct*2);                                                            //Write inversed correction to inner motor
          writeMotor(1,1,130);                                                                      //Write fixed lower speed to outer motor
          straight = false;                                                                         //Set straight to false
        }
        if (straight) {                                                                             //If straight is true (ie normal cruise)
          correct = 0;                                                                              //Reset correction counter
        } else {                                                                                    //Else (ie straight is false)
          correct+= 1-(correct>37);                                                                 //Increment correction counter (oflow safe)
          delay(10);                                                                                //Small loop delay to allow correction to properly happen
        }
      }
      correct = 0;                                                                                  //Once exited, reset correct
      straight = true;                                                                              //And reset straight
      break;                                                                                        //Break switch
    }
  }



  //<------------------------------TURN 1------------------------------>
  //This is the standard turn procedure, firstly set LOCATION 2 to
  //indicate that the robot is at the T junction, move the robot
  //backwards to avoid having to overturn, the finally turn with 
  //a blind turn duration to avoid false trigger
  //
  LOCATION = 2;                                                                                     //Set LOCATION 2 to indicate T juction
  blindMove(2,255,250);                                                                             //Move backwards to reduce turning radius
  if (PATHDIR = 1) {                                                                                //If PATHDIR 1
    leftTurn(220, 220, 10, 300, 0.8, 0.4);                                                          //Turn left with 220 differential, 10ms loop delay, 300ms blind delay
  } else if (PATHDIR = 2) {                                                                         //If PATHDIR 2
    rightTurn(220, 220, 10, 300, 0.8, 0.4);                                                         //Turn right with 220 differential, 10ms loop delay, 200ms blind delay
  }





  //<------------------------------LINE FOLLOW 2------------------------------>
  //This second line follow consists of a traditional line following algorithm
  //followed by a tweak adjustment and a blind move to correctly line up
  //for the X junc turn.
  //
  LOCATION = 3;                                                                                     //Set LOCATION 3 to indicate line follow before X junc
  correct = 0;                                                                                      //Reset correct to 0
  straight = true;                                                                                  //Reset straight boolean to true
  
  //<TEMPORARY REMOVAL, MAY BE PERMENANT if no false trig therefore no blind follow needed>
  /*for (int i = 0; i < 100; i++) { //TODO? Blind line follow to avoid early trig on sensors (Maybe not needed with new trig conditions??)
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
      correct+= 1-(correct>75);
      //delay(15);
    }
    delay(10);
  }*/
  correct = 0;
  straight = true;                                                                                  //Reset straight boolean to true
  while (!SENSOR_STATE[0] && !SENSOR_STATE[1]) {                                                    //Verify trigger condition
    readSensors();                                                                                  //Read line sensors
    straight = true;                                                                                //Reset straight to true
    if ((!SENSOR_STATE[2] && !SENSOR_STATE[3]) || (SENSOR_STATE[2] && SENSOR_STATE[3])) {           //If both CL and CR see black, or both see white
      writeMotor(1,180,1,180);                                                                      //Write cruise speed
    }else if (SENSOR_STATE[2]) {                                                                    //If CL sensor sees black                                              
      writeMotor(0,2,180+correct);                                                                  //Write inverse correction to inner motor
      writeMotor(1,1,180+correct);                                                                  //Write correction to outer motor
      straight = false;                                                                             //Set straight to false
    } else if (SENSOR_STATE[3]) {                                                                   //If CR sensor sees black
      writeMotor(1,2,180+correct);                                                                  //Write inverse correction to inner motor
      writeMotor(0,1,180+correct);                                                                  //Write correction to outer motor
      straight= false;                                                                              //Set straight to false
    }

    if (straight) {                                                                                 //If straight is true (ie normal cruise)
      correct = 0;                                                                                  //Reset correction counter
    } else {                                                                                        //Else (ie straight is false)
      correct+= 1-(correct>37);                                                                     //Increment correction counter (oflow safe)
      delay(10);                                                                                    //Small loop delay to allow correction to properly happen
    }
  }
  correct = 0;                                                                                      //Reset correct to 0
  straight = true;                                                                                  //Reset straight boolean to true

  blindMove(2,255,250);                                                                             //Reverse blip to avoid innertial line miss after trigger
  delay(300);                                                                                       //Delay to kill any residual innertia

  for(int i =0;i<200;i++){                                                                          //Tweak to straighten at intersection
    readSensors();                                                                                  //Read line sensors
    if(SENSOR_STATE[0]){                                                                            //If front left sensor sees black
      writeMotor(0,2,255);                                                                          //Full reverse left motor
    }else{                                                                                          //Else (ie front left sensor sees white)
      writeMotor(0,1,255);                                                                          //Full forward left motor
    }
    if(SENSOR_STATE[1]){                                                                            //If front right sensors sees black
      writeMotor(1,2,255);                                                                          //Full reverse right motor
    }else{                                                                                          //Else (ie front right sensors sees white)
      writeMotor(1,1,255);                                                                          //Full forward right motor
    }
    delay(1);                                                                                       //Tiny loop delay to avoid CPU hogging
  }

  writeMotor(0,0,0,0);                                                                              //Stop both motors
  delay(300);                                                                                       //Delay to kill any residual innertia

   correct = 0;
  straight = true;                                                                                  //Reset straight boolean to true
  while (!SENSOR_STATE[4] && !SENSOR_STATE[5]) {                                                    //Verify trigger condition
    readSensors();                                                                                  //Read line sensors
    straight = true;                                                                                //Reset straight to true
    if ((!SENSOR_STATE[2] && !SENSOR_STATE[3]) || (SENSOR_STATE[2] && SENSOR_STATE[3])) {           //If both CL and CR see black, or both see white
      writeMotor(1,180,1,180);                                                                      //Write cruise speed
    }else if (SENSOR_STATE[2]) {                                                                    //If CL sensor sees black                                              
      writeMotor(0,2,180+correct);                                                                  //Write inverse correction to inner motor
      writeMotor(1,1,180+correct);                                                                  //Write correction to outer motor
      straight = false;                                                                             //Set straight to false
    } else if (SENSOR_STATE[3]) {                                                                   //If CR sensor sees black
      writeMotor(1,2,180+correct);                                                                  //Write inverse correction to inner motor
      writeMotor(0,1,180+correct);                                                                  //Write correction to outer motor
      straight= false;                                                                              //Set straight to false
    }

    if (straight) {                                                                                 //If straight is true (ie normal cruise)
      correct = 0;                                                                                  //Reset correction counter
    } else {                                                                                        //Else (ie straight is false)
      correct+= 1-(correct>37);                                                                     //Increment correction counter (oflow safe)
      delay(10);                                                                                    //Small loop delay to allow correction to properly happen
    }
  }
  correct = 0;                                                                                      //Reset correct to 0
  straight = true;                                                                                  //Reset straight boolean to true

  blindMove(2,255,250);                                                                             //Reverse blip to avoid innertial line miss after trigger
  delay(300);                                                                                       //Delay to kill any residual innertia

  for(int i =0;i<500;i++){                                                                          //Tweak to straighten at intersection
    readSensors();                                                                                  //Read line sensors
    if(SENSOR_STATE[4]){                                                                            //If front left sensor sees black
      writeMotor(0,2,255);                                                                          //Full reverse left motor
    }else{                                                                                          //Else (ie front left sensor sees white)
      writeMotor(0,1,255);                                                                          //Full forward left motor
    }
    if(SENSOR_STATE[5]){                                                                            //If front right sensors sees black
      writeMotor(1,2,255);                                                                          //Full reverse right motor
    }else{                                                                                          //Else (ie front right sensors sees white)
      writeMotor(1,1,255);                                                                          //Full forward right motor
    }
    delay(1);                                                                                       //Tiny loop delay to avoid CPU hogging
  }

  writeMotor(0,0,0,0);                                                                              //Stop both motors
  delay(300);                                                                                       //Delay to kill any residual innertia




  //<------------------------------TURN 2------------------------------>
  //This is the standard turn procedure, firstly set LOCATION 4 to
  //indicate that the robot is at the X junction, move the robot
  //backwards to avoid having to overturn, the finally turn with 
  //a blind turn duration to avoid false exit trigger
  //
  LOCATION = 4;                                                                                     //Set LOCATION 4 to indicate X juction
  blindMove(2,255,250);                                                                             //Move backwards to reduce turning radius
  if (PATHDIR = 1) {                                                                                //If PATHDIR 1
    rightTurn(220, 220, 10, 300, 0.8, 0.4);                                                         //Turn right with 220 differential, 10ms loop delay, 300ms blind delay
  } else if (PATHDIR = 2) {                                                                         //If PATHDIR 2
    leftTurn(220, 220, 10, 300, 0.8, 0.4);                                                          //Turn left with 220 differential, 10ms loop delay, 200ms blind delay
  }
  




  //<------------------------------LINE FOLLOW 3------------------------------>
  //This line follow is a standard line follow until trig on the outer front
  //sensor to avoid crossing and false triggering the inner front sensor on
  //the sharp bend
  LOCATION = 5;                                                                                     //Set LOCATION 5 to indicate line follow after X junc
  correct = 0;                                                                                      //Reset correct to 0
  straight = true;                                                                                  //Reset straight boolean to true

  //<TEMPORARY REMOVAL, MAY BE PERMENANT if no false trig therefore no blind follow needed>
  /*for (int i = 0; i < 100; i++) { //No trig follow (might not be necessary)
    readSensors();
    straight = true;
    if ((!SENSOR_STATE[2] && !SENSOR_STATE[3]) || (SENSOR_STATE[2] && SENSOR_STATE[3])) {
      writeMotor(1,160,1,160);
    }else if (SENSOR_STATE[2]) {
      //writeMotor(0,2,255);
      //writeMotor(1,1,255);
      //delay(5);
      writeMotor(0,2,180+correct*2);
      writeMotor(1,1,140);
      straight = false;
    } else if (SENSOR_STATE[3]) {
      //writeMotor(0,1,255);
      //writeMotor(1,2,255);
      //delay(5);
      writeMotor(1,2,180+correct*2);
      writeMotor(0,1,140);
      straight= false;
    }

    if (straight) {
      correct = 0;
    } else {
      correct+= 1-(correct>37);
      //delay(15);
    }
    delay(5);

  } */

  while (!SENSOR_STATE[PATHDIR%2]) {                                                                //Verify trigger condition
    readSensors();                                                                                  //Read line sensors
    straight = true;                                                                                //Reset straight to true
    if ((!SENSOR_STATE[2] && !SENSOR_STATE[3]) || (SENSOR_STATE[2] && SENSOR_STATE[3])) {           //If both CL and CR see black, or both see white
      writeMotor(1,180,1,180);                                                                      //Write cruise speed
    }else if (SENSOR_STATE[2]) {                                                                    //If CL sensor sees black                                              
      writeMotor(0,2,180+correct*2);                                                                //Write inverse correction to inner motor
      writeMotor(1,1,130);                                                                          //Write fixed correction to outer motor
      straight = false;                                                                             //Set straight to false
    } else if (SENSOR_STATE[3]) {                                                                   //If CR sensor sees black
      writeMotor(1,2,180+correct*2);                                                                //Write inverse correction to inner motor
      writeMotor(0,1,130);                                                                          //Write fixed correction to outer motor
      straight= false;                                                                              //Set straight to false
    }

    if (straight) {                                                                                 //If straight is true (ie normal cruise)
      correct = 0;                                                                                  //Reset correction counter
    } else {                                                                                        //Else (ie straight is false)
      correct+= 1-(correct>37);                                                                     //Increment correction counter (oflow safe)
      delay(10);                                                                                    //Small loop delay to allow correction to properly happen
    }
  }
  correct = 0;                                                                                      //Reset correct to 0
  straight = true;                                                                                  //Reset straight boolean to true
  blindMove(2,255,250);                                                                             //Reverse blip to avoid innertial line miss which would fake tweak
  writeMotor(0,0,0,0);                                                                              //Stop all motors
  delay(300);                                                                                       //Allow robot to settle
  for(int i =0;i<500;i++){                                                                          //Tweak to straighten end
    readSensors();                                                                                  //Read line sensors
    if(SENSOR_STATE[0]){                                                                            //If front left sensor sees black
      writeMotor(0,2,255);                                                                          //Full reverse left motor
    }else{                                                                                          //Else (ie front left sensor sees white)
      writeMotor(0,1,255);                                                                          //Full forward left motor
    }
    if(SENSOR_STATE[1]){                                                                            //If front right sensors sees black
      writeMotor(1,2,255);                                                                          //Full reverse right motor
    }else{                                                                                          //Else (ie front right sensors sees white)
      writeMotor(1,1,255);                                                                          //Full forward right motor
    }
    delay(1);                                                                                       //Tiny loop delay to avoid CPU hogging
  }
  writeMotor(0,0,0,0);                                                                              //Stop motors on exit
  delay(500);                                                                                       //Allow robot to settle

  //ANGLE
  LOCATION = 6;                                                                                     //LOCATION 6 to indicate positioned at end line
  readSensors();                                                                                    //Read line sensors
  blindMove(2,220,300);                                                                             //Reverse robot to allow front sensors to sweep angle
  writeMotor(0,0,0,0);                                                                              //Stop robot after reverse
  readSensors();                                                                                    //Read line sensors
  switch (PATHDIR) {                                                                                //Switch on PATHDIR for sensor condition and angle direction
    case 1 : {                                                                                      //PATHDIR 1, turn right and sensor trig on front left
      while (!SENSOR_STATE[0]) {                                                                    //Trig condition
        writeMotor(1,255,2,255);                                                                    //Write full differential right
        delay(12);                                                                                  //Right blip delay
        writeMotor(0,0,0,0);                                                                        //Stop motors
        delay(45);                                                                                  //Settle delay to avoid excessive innertia
      }
    }
    case 2 : {                                                                                      //PATHDIR 2, turn left and sensor trig on front right
      while (!SENSOR_STATE[1]) {                                                                    //Trig condition
        writeMotor(2,255,1,255);                                                                    //Write full differential left
        delay(12);                                                                                  //Left blip delay
        writeMotor(0,0,0,0);                                                                        //Stop motors
        delay(45);                                                                                  //Settle delay to avoid excessive innertia
      }
    }
  }
  blindMove(1,140,600);                                                                             //Move robot slowly forward to allow for arm deploy
  writeMotor(0,0,0,0);                                                                              //Stop motors

  //EXTEND ARM
  LOCATION = 7;                                                                                     //LOCATION 7 to indicate ready for arm deploy
  writeStepper(30,3500,false);                                                                      //Slow deploy for start
  writeStepper(255,2000,false);                                                                     //Fast deploy for first section
  writeStepper(30,3500,false);                                                                      //Slow deploy for middle interference 
  writeStepper(320,2000,false);                                                                     //Fast deploy until end
  delay(500);                                                                                       //Delay to make sure deploy complete

  //RETRACT ARM
  LOCATION = 8;                                                                                     //LOCATION 8 to indicate ready for arm retract
  writeStepper(320,2000,true);                                                                      //Fast retract for last section
  writeStepper(30,3500,true);                                                                       //Slow retract for middle interference
  writeStepper(255,2000,true);                                                                      //Fast retract for first section
  writeStepper(30,3500,true);                                                                       //Slow retract to complete deploy
  LOCATION = 9;                                                                                     //LOCATION 9 to indicate deploy and retract complete

  //EXIT
  LOCATION = 10;                                                                                    //LOCATION 10 to indicate lap complete
  while (true);                                                                                     //Code hang to avoid loop
}