#ifndef COMMONUTILS_H
#define COMMONUTILS_H

#include <Arduino.h>
#include <Wire.h>
#include "globals.h"

void writeI2C(uint8_t ADDR, uint8_t VAL);
void initSystems(bool VERBOSE);
void initSystems();
void readSensors();

#endif