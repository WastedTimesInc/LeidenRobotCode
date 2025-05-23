#ifndef MOTORCTRL_H
#define MOTORCTRL_H

#include <Arduino.h>
#include "commonutils.h"
#include "globals.h"

void writeMotor(uint8_t MOTOR, uint8_t DIR, uint8_t SPEED);
void writeMotor(uint8_t DIR0, uint8_t SPEED0, uint8_t DIR1, uint8_t SPEED1);



#endif