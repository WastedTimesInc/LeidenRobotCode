#ifndef LINEFOLLOW_H
#define LINEFOLLOW_H

#include <Arduino.h>
#include "motorctrl.h"
#include "commonutils.h"
#include "globals.h"

void simpleFollow(int EXIT_CONDITION, uint8_t SPEED, uint8_t CORRECTION_SPEED, bool CORRECTION_FLIP, int LOOP_DELAY);

void simpleFollow(int EXIT_CONDITION, uint8_t SPEED, uint8_t CORRECTION_SPEED, bool CORRECTION_FLIP, int LOOP_DELAY, uint8_t BLIP_SPEED, int BLIP_DURATION);

#endif