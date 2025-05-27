#ifndef COMMONUTILS_H
#define COMMONUTILS_H

#include <Arduino.h>
#include <Wire.h>
#include "globals.h"
#include "NewPing.h"

// writeI2C
// This is a generic function to write a value VAL at an address ADDR over I2C
// Called by multiple other fuctions
void writeI2C(uint8_t ADDR, uint8_t VAL);

// readSensors
// This is a function that reads line sensors and saves their result in the global SENSOR_STATE array
void readSensors();

void readUltrasonic();
void readUltrasonic(uint8_t PING_ITER);

// initSystems
// initializes all pins, wire etc
void initSystems();

void writeStepper(int STEPS, int PAUSE_DELAY, bool REV);

#endif