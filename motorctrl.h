#ifndef MOTORCTRL_H
#define MOTORCTRL_H

#include <Arduino.h>
#include "commonutils.h"
#include "globals.h"

// writeMotor
// This writes a(all) motor(s) to a given speed and direction, available in two configs
//
// writeMotor(uint8_t MOTOR, uint8_t DIR, uint8_t SPEED)
//    Write the speed and direction of just one motor
//    MOTOR : 0=left, 1=right
//    DIR   : 0=stopped, 1=fwd, 2=rev
//    SPEED : 0-255
//
// writeMotor(uint8_t DIR0, uint8_t SPEED0, uint8_t DIR1, uint8_t SPEED1)
//    Write the speed and direction for both motors
//    DIR0(1)   : 0=stopped, 1=fwd, 2=rev
//    SPEED0(1) : 0-255
void writeMotor(uint8_t MOTOR, uint8_t DIR, uint8_t SPEED);
void writeMotor(uint8_t DIR0, uint8_t SPEED0, uint8_t DIR1, uint8_t SPEED1);



#endif