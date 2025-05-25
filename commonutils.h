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

// junctionDetect
// Detects the type of junction with a set of sensors, POS: 1=front_sensors, 2=rear_sensors
// Returns:
//    -1  : ERROR
//     0  : X Junction
//     1  : T Junction (left)
//     2  : T Junction (right)
int junctionDetect(int POS);

// exitCondition
// Checks a condition and returns true when validated
// CHECK :
//  1 - Detect both front sensors
//  2 - Detect both rear sensors
//  3 - Detect A front sensors
//  4 - Detect A rear sensor
//  5 - Detect both central sensors
//  6 - Detect A central sensor
//  7 - Detect TL Sensor
//  8 - Detect TR Sensor
//  9 - Detect CL Sensor
//  10 - Detect CR Sensor
bool exitCondition(int CHECK);

// initSystems
// initializes all pins, wire etc
// when VERBOSE is true, start serial at baud 115200
void initSystems(bool VERBOSE);
void initSystems();

#endif