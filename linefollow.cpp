#include "linefollow.h"

void simpleFollow(int EXIT_CONDITION, uint8_t SPEED, uint8_t CORRECTION_SPEED, bool CORRECTION_FLIP, int LOOP_DELAY) {
  uint8_t CORRECTION_DIR = 1; // Set correction direction by default FWD
  if (CORRECTION_FLIP) { // If correction direction is flipped, set to REV
    CORRECTION_DIR = 2;
  }
  while(!exitCondition(EXIT_CONDITION)){ // Check exit condition, see commonutils.h for details
    if (!SENSOR_STATE[2] && !SENSOR_STATE[3]) { // If both sensors clear, cruise both motors
      writeMotor(1, SPEED, 1, SPEED);
    } else if (!SENSOR_STATE[2] && SENSOR_STATE[3]) { // If right sensor detects line, apply correction to right side
      writeMotor(1, SPEED, CORRECTION_DIR, CORRECTION_SPEED);
    } else if (SENSOR_STATE[2] && !SENSOR_STATE[3]) { // If left sensor detects line, apply correction to left side
      writeMotor(CORRECTION_DIR, CORRECTION_SPEED, 1, SPEED);
    }

    delay(LOOP_DELAY); // Loop delay
  }
  writeMotor(0,0,0,0); // Stop motors on exit
}

void simpleFollow(int EXIT_CONDITION, uint8_t SPEED, uint8_t CORRECTION_SPEED, bool CORRECTION_FLIP, int LOOP_DELAY, uint8_t BLIP_SPEED, int BLIP_DURATION) {
  uint8_t CORRECTION_DIR = 1; // Set correction direction by default FWD
  if (CORRECTION_FLIP) { // If correction direction is flipped, set to REV
    CORRECTION_DIR = 2;
  }
  while(!exitCondition(EXIT_CONDITION)){ // Check exit condition, see commonutils.h for details
    if (!SENSOR_STATE[2] && !SENSOR_STATE[3]) { // If both sensors clear, cruise both motors
      writeMotor(1, BLIP_SPEED, 1, BLIP_SPEED); // Apply blip speed
      delay(BLIP_DURATION); // Blip duration
      writeMotor(1, SPEED, 1, SPEED); // Write normal speed
    } else if (!SENSOR_STATE[2] && SENSOR_STATE[3]) { // If right sensor detects line, apply correction to right side
      writeMotor(1, BLIP_SPEED, CORRECTION_DIR, BLIP_SPEED); // Apply blip speed with correction direction
      delay(BLIP_DURATION); // Blip duration
      writeMotor(1, SPEED, CORRECTION_DIR, CORRECTION_SPEED); // Apply normal correction
    } else if (SENSOR_STATE[2] && !SENSOR_STATE[3]) { // If left sensor detects line, apply correction to left side
      writeMotor(CORRECTION_DIR, BLIP_SPEED, 1, BLIP_SPEED); // Apply blip speed with correction direction
      delay(BLIP_DURATION); // Blip duration
      writeMotor(CORRECTION_DIR, CORRECTION_SPEED, 1, SPEED); // Apply normal correction
    }

    delay(LOOP_DELAY); // Loop delay
  }
  writeMotor(0,0,0,0); // Stop motors on exit
}

void leftTurn(int LOOP_DELAY) {
  while (!SENSOR_STATE[2]) { // While CL sensor reads white, turn full differential left
    writeMotor(2, 255, 1, 255); // Full L differential
    readSensors(); // Update sensors
    delay(LOOP_DELAY); // Loop delay
  }
  while (SENSOR_STATE[2]) { // CL sensor then detects line, keep turning while it detects the line
    writeMotor(2, 255, 1, 255); // Full L differential
    readSensors(); // Update sensors
    delay(LOOP_DELAY); // Loop delay
  }
  writeMotor(0,0,0,0); // Stop motors on exit, turn finished
}

void leftTurn(uint8_t L_SPEED, uint8_t R_SPEED, int LOOP_DELAY) {
  while (!SENSOR_STATE[2]) { // While CL sensor reads white, turn custom differential left
    writeMotor(2, L_SPEED, 1, R_SPEED); // Custom L differential
    readSensors(); // Update sensors
    delay(LOOP_DELAY); // Loop delay
  }
  while (SENSOR_STATE[2]) { // CL sensor then detects line, keep turning while it detects the line
    writeMotor(2, L_SPEED, 1, R_SPEED); // Custom L differential
    readSensors(); // Update sensors
    delay(LOOP_DELAY); // Loop delay
  }
  writeMotor(0,0,0,0); // Stop motors on exit, turn finished
}

void rightTurn(int LOOP_DELAY) {
  while (!SENSOR_STATE[3]) { // While CR sensor reads white, turn full differential right
    writeMotor(1, 255, 2, 255); // Full R differential
    readSensors(); // Update sensors
    delay(LOOP_DELAY); // Loop delay
  }
  while (SENSOR_STATE[3]) { // CR sensor then detects line, keep turning while it detects the line
    writeMotor(1, 255, 2, 255); // Full R differential
    readSensors(); // Update sensors
    delay(LOOP_DELAY); // Loop delay
  }
  writeMotor(0,0,0,0);
}

void rightTurn(uint8_t L_SPEED, uint8_t R_SPEED, int LOOP_DELAY) {
  while (!SENSOR_STATE[3]) { // While CR sensor reads white, turn custom differential right
    writeMotor(1, L_SPEED, 2, R_SPEED); // Custom R differential
    readSensors(); // Update sensors
    delay(LOOP_DELAY); // Loop delay
  }
  while (SENSOR_STATE[3]) { // CR sensor then detects line, keep turning while it detects the line
    writeMotor(1, L_SPEED, 2, R_SPEED); // Custom R differential
    readSensors(); // Update sensors
    delay(LOOP_DELAY); // Loop delay
  }
  writeMotor(0,0,0,0);
}


