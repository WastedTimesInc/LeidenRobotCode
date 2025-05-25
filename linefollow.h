#ifndef LINEFOLLOW_H
#define LINEFOLLOW_H

#include <Arduino.h>
#include "motorctrl.h"
#include "commonutils.h"
#include "globals.h"

// simpleFollow
// This is a simple line following algorithm based on the two central line sensors (CL, CR), available in two configs
//
// simpleFollow(int EXIT_CONDITION, uint8_t SPEED, uint8_t CORRECTION_SPEED, bool CORRECTION_FLIP, int LOOP_DELAY)
// This is the simplest form, it takes an exit condition EXIT_CONDITION (See commonutils.h)
// SPEED              =     Cruise speed (0-255)
// CORRECTION_SPEED   =     Correction speed, speed applied to the motor to correct trajectory (0-255)
// CORRECTION_FLIP    =     Used to flip the correction direction, if false, correction is just slowing down a motor, if true, correction is an inverse rotation of a motor
// LOOP_DELAY         =     The feedback loop delay must be >=0
//
// simpleFollow(int EXIT_CONDITION, uint8_t SPEED, uint8_t CORRECTION_SPEED, bool CORRECTION_FLIP, int LOOP_DELAY, uint8_t BLIP_SPEED, int BLIP_DURATION)
// This is the same as above, but has a blip function, this is to combat motor lock at slow speeds, it works the same as the first function with the addition of:
// BLIP_SPEED       =     Speed of the motor blip (0-255)
// BLIP_DURATION    =     Duration of the blip, must be greater that >=0
// Note: The blip adds blocking delay to the loop, therefore to get an equal loop duration when using blip, the overall loop delay = BLIP_DURATION+LOOP_DELAY
void simpleFollow(int EXIT_CONDITION, uint8_t SPEED, uint8_t CORRECTION_SPEED, bool CORRECTION_FLIP, int LOOP_DELAY);
void simpleFollow(int EXIT_CONDITION, uint8_t SPEED, uint8_t CORRECTION_SPEED, bool CORRECTION_FLIP, int LOOP_DELAY, uint8_t BLIP_SPEED, int BLIP_DURATION);

void dynamicFollow(int EXIT_CONDITION, uint8_t SPEED, int LOOP_DELAY, float DAMPING_FACTOR);
void dynamicFollow(int EXIT_CONDITION, uint8_t SPEED, int LOOP_DELAY, float DAMPING_FACTOR, uint8_t BLIP_SPEED, int BLIP_DURATION);

void highSpeedFollowLegacy(int EXIT_CONDITION, uint8_t SPEED, int LOOP_DELAY, float CORR_GAIN);
void highSpeedFollow(int EXIT_CONDITION, uint8_t SPEED, int LOOP_DELAY, float CORR_GAIN);

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
void leftTurn(uint8_t L_SPEED, uint8_t R_SPEED, int LOOP_DELAY, int BLIND_DELAY);

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
void rightTurn(uint8_t L_SPEED, uint8_t R_SPEED, int LOOP_DELAY, int BLIND_DELAY);


#endif