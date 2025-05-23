#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>

// I2C address for motor controller
extern const int CTRL_ADDR;

// Shared state variables
extern uint8_t CURRENT;

extern uint8_t MOTOR_A_STATE;
extern uint8_t MOTOR_B_STATE;

extern uint8_t MOTOR_A_SPEED;
extern uint8_t MOTOR_B_SPEED;

// PWM output pins for motors
extern const uint8_t SPD_PIN_A;
extern const uint8_t SPD_PIN_B;

// Motor control bitmasks
extern const uint8_t M_NULL;

extern const uint8_t MOTOR_A_FWD;
extern const uint8_t MOTOR_A_REV;
extern const uint8_t MOTOR_A_MASK;
extern const uint8_t MOTOR_A_INV_MASK;

extern const uint8_t MOTOR_B_FWD;
extern const uint8_t MOTOR_B_REV;
extern const uint8_t MOTOR_B_MASK;
extern const uint8_t MOTOR_B_INV_MASK;

extern const uint8_t SENSOR_PINS[];
extern bool SENSOR_STATE[];

#endif // GLOBALS_H