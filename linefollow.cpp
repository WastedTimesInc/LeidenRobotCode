#include "linefollow.h"

void simpleFollow(int EXIT_CONDITION, uint8_t SPEED, uint8_t CORRECTION_SPEED, bool CORRECTION_FLIP, int LOOP_DELAY) {
  uint8_t CORRECTION_DIR = 1;
  if (CORRECTION_FLIP) {
    CORRECTION_DIR = 2;
  }
  while(!exitCondition(EXIT_CONDITION)){
    if (!SENSOR_STATE[2] && !SENSOR_STATE[3]) {
      writeMotor(1, SPEED, 1, SPEED);
    } else if (!SENSOR_STATE[2] && SENSOR_STATE[3]) {
      writeMotor(1, SPEED, CORRECTION_DIR, CORRECTION_SPEED);
    } else if (!SENSOR_STATE[2] && !SENSOR_STATE[3]) {
      writeMotor(CORRECTION_DIR, CORRECTION_SPEED, 1, SPEED);
    }

    delay(LOOP_DELAY);
  }
  writeMotor(0,0,0,0);
}

void simpleFollow(int EXIT_CONDITION, uint8_t SPEED, uint8_t CORRECTION_SPEED, bool CORRECTION_FLIP, int LOOP_DELAY, uint8_t BLIP_SPEED, int BLIP_DURATION) {
  uint8_t CORRECTION_DIR = 1;
  if (CORRECTION_FLIP) {
    CORRECTION_DIR = 2;
  }
  while(!exitCondition(EXIT_CONDITION)){
    if (!SENSOR_STATE[2] && !SENSOR_STATE[3]) {
      writeMotor(1, BLIP_SPEED, 1, BLIP_SPEED);
      delay(BLIP_DURATION);
      writeMotor(1, SPEED, 1, SPEED);
    } else if (!SENSOR_STATE[2] && SENSOR_STATE[3]) {
      writeMotor(1, BLIP_SPEED, CORRECTION_DIR, BLIP_SPEED);
      delay(BLIP_DURATION);
      writeMotor(1, SPEED, CORRECTION_DIR, CORRECTION_SPEED);
    } else if (!SENSOR_STATE[2] && !SENSOR_STATE[3]) {
      writeMotor(CORRECTION_DIR, BLIP_SPEED, 1, BLIP_SPEED);
      delay(BLIP_DURATION);
      writeMotor(CORRECTION_DIR, CORRECTION_SPEED, 1, SPEED);
    }

    delay(LOOP_DELAY);
  }
  writeMotor(0,0,0,0);
}

void leftTurn() {
  while (!SENSOR_STATE[2]) {
    writeMotor(2, 255, 1, 255);
    readSensors();
    delay(50);
  }
  while (SENSOR_STATE[2]) {
    writeMotor(2, 255, 1, 255);
    readSensors();
    delay(50);
  }
  writeMotor(0,0,0,0);
}

void leftTurn(uint8_t L_SPEED, uint8_t R_SPEED, int LOOP_DELAY) {
  while (!SENSOR_STATE[2]) {
    writeMotor(2, L_SPEED, 1, R_SPEED);
    readSensors();
    delay(LOOP_DELAY);
  }
  while (SENSOR_STATE[2]) {
    writeMotor(2, L_SPEED, 1, R_SPEED);
    readSensors();
    delay(LOOP_DELAY);
  }
  writeMotor(0,0,0,0);
}

void rightTurn() {
  while (!SENSOR_STATE[3]) {
    writeMotor(1, 255, 2, 255);
    readSensors();
    delay(50);
  }
  while (SENSOR_STATE[3]) {
    writeMotor(1, 255, 2, 255);
    readSensors();
    delay(50);
  }
  writeMotor(0,0,0,0);
}

void rightTurn(uint8_t L_SPEED, uint8_t R_SPEED, int LOOP_DELAY) {
  while (!SENSOR_STATE[2]) {
    writeMotor(1, L_SPEED, 2, R_SPEED);
    readSensors();
    delay(LOOP_DELAY);
  }
  while (SENSOR_STATE[2]) {
    writeMotor(1, L_SPEED, 2, R_SPEED);
    readSensors();
    delay(LOOP_DELAY);
  }
  writeMotor(0,0,0,0);
}


