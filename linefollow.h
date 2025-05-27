#ifndef LINEFOLLOW_H
#define LINEFOLLOW_H

#include <Arduino.h>
#include "motorctrl.h"
#include "commonutils.h"
#include "globals.h"

// leftTurn
// Pivots the robot left on itself using the CL sensor to detect the new line
// Available in two configurations
//
// leftTurn(int LOOP_DELAY)
// Turns at full differential with a loop delay LOOP_DELAY, BLIND_DELAY sets a blind timeout to avoid false wrong line triggers
//
// leftTurn(uint8_t L_SPEED, uint8_t R_SPEED, int LOOP_DELAY)
// Turns with given motor speeds L_SPEED and R_SPEED, L motor in rev, R motor in fwd
// Loop delay and blind delay same as above
void leftTurn(int LOOP_DELAY, int BLIND_DELAY);
void leftTurn(uint8_t L_SPEED, uint8_t R_SPEED, int LOOP_DELAY, int BLIND_DELAY, float MULT_1, float MULT_2);

// rightTurn
// Pivots the robot left on itself using the CL sensor to detect the new line
// Available in two configurations
//
// rightTurn(int LOOP_DELAY)
// Turns at full differential with a loop delay LOOP_DELAY, BLIND_DELAY sets a blind timeout to avoid false wrong line triggers
//
// rightTurn(uint8_t L_SPEED, uint8_t R_SPEED, int LOOP_DELAY)
// Turns with given motor speeds L_SPEED and R_SPEED, L motor in fwd, R motor in rev
// Loop delay and blind delay same as above
void rightTurn(int LOOP_DELAY, int BLIND_DELAY);
void rightTurn(uint8_t L_SPEED, uint8_t R_SPEED, int LOOP_DELAY, int BLIND_DELAY, float MULT_1, float MULT_2);


void blindMove(int DIR, uint8_t SPEED, int DURATION);

#endif