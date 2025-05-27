#include "linefollow.h"

void leftTurn(int LOOP_DELAY, int BLIND_DELAY) {
  writeMotor(2, 255, 1, 255); // Full L differential
  delay(BLIND_DELAY); // Blind delay
  while (!SENSOR_STATE[2] && !SENSOR_STATE[0]) { // While CL sensor reads white, turn full differential left
    readSensors(); // Update sensors
    delay(LOOP_DELAY); // Loop delay
  }
  writeMotor(2, 190, 1, 190);
  while (!SENSOR_STATE[2]) {
    readSensors();
    delay(LOOP_DELAY);
  }
  writeMotor(2, 150, 1, 150);
  while (SENSOR_STATE[2]) { // CL sensor then detects line, keep turning while it detects the line
    readSensors(); // Update sensors
    delay(LOOP_DELAY); // Loop delay
  }
  writeMotor(0,0,0,0); // Stop motors on exit, turn finished
}

void leftTurn(uint8_t L_SPEED, uint8_t R_SPEED, int LOOP_DELAY, int BLIND_DELAY, float MULT_1, float MULT_2) {
  writeMotor(2, L_SPEED, 1, R_SPEED); // Custom L differential
  delay(BLIND_DELAY); // Blind delay
  while (!SENSOR_STATE[2] && !SENSOR_STATE[0]) { // While CL sensor reads white, turn full differential left
    readSensors(); // Update sensors
    delay(LOOP_DELAY); // Loop delay
  }
  writeMotor(2, uint8_t(L_SPEED*MULT_1), 1, uint8_t(R_SPEED*MULT_1));
  while (!SENSOR_STATE[2]) {
    readSensors();
    delay(LOOP_DELAY);
  }
  writeMotor(2, uint8_t(L_SPEED*MULT_2), 1, uint8_t(R_SPEED*MULT_2));
  while (SENSOR_STATE[2]) { // CL sensor then detects line, keep turning while it detects the line
    readSensors(); // Update sensors
    delay(LOOP_DELAY); // Loop delay
  }
  writeMotor(0,0,0,0); // Stop motors on exit, turn finished
}

void rightTurn(int LOOP_DELAY, int BLIND_DELAY) {
  writeMotor(1, 255, 2, 255); // Full R differential
  delay(BLIND_DELAY); // Blind delay
  while (!SENSOR_STATE[3] && !SENSOR_STATE[1]) { // While CL sensor reads white, turn full differential left
    readSensors(); // Update sensors
    delay(LOOP_DELAY); // Loop delay
  }
  writeMotor(1, 190, 2, 190);
  while (!SENSOR_STATE[3]) {
    readSensors();
    delay(LOOP_DELAY);
  }
  writeMotor(1, 150, 2, 150);
  while (SENSOR_STATE[3]) { // CL sensor then detects line, keep turning while it detects the line
    readSensors(); // Update sensors
    delay(LOOP_DELAY); // Loop delay
  }
  writeMotor(0,0,0,0); // Stop motors on exit, turn finished
}

void rightTurn(uint8_t L_SPEED, uint8_t R_SPEED, int LOOP_DELAY, int BLIND_DELAY, float MULT_1, float MULT_2) {
  writeMotor(1, L_SPEED, 2, R_SPEED); // Custom L differential
  delay(BLIND_DELAY); // Blind delay
  while (!SENSOR_STATE[3] && !SENSOR_STATE[1]) { // While CL sensor reads white, turn full differential left
    readSensors(); // Update sensors
    delay(LOOP_DELAY); // Loop delay
  }
  writeMotor(1, uint8_t(L_SPEED*MULT_1), 2, uint8_t(R_SPEED*MULT_1));
  while (!SENSOR_STATE[3]) {
    readSensors();
    delay(LOOP_DELAY);
  }
  writeMotor(1, uint8_t(L_SPEED*MULT_2), 2, uint8_t(R_SPEED*MULT_2));
  while (SENSOR_STATE[3]) { // CL sensor then detects line, keep turning while it detects the line
    readSensors(); // Update sensors
    delay(LOOP_DELAY); // Loop delay
  }
  writeMotor(0,0,0,0); // Stop motors on exit, turn finished
}

void blindMove(int DIR, uint8_t SPEED, int DURATION) {
  writeMotor(DIR, SPEED, DIR, SPEED);
  delay(DURATION);
  writeMotor(0,0,0,0);
}


