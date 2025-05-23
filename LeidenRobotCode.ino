#include <Arduino.h>
#include "commonutils.h"
#include "globals.h"

const int CTRL_ADDR = 0x20;
// Stores current I2C state
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

const uint8_t SENSOR_PINS[6] = {A0, A1, A2, A3, 7, 4};
bool SENSOR_STATE[6] = {false, false, false, false, false, false};


void setup() {
}

void loop() {
}