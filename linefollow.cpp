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

void dynamicFollow(int EXIT_CONDITION, uint8_t SPEED, int LOOP_DELAY, float DAMPING_FACTOR){
  float CORR_MULTIPLIER = 1.0f;  // Starts at 100 % of SPEED
  while (!exitCondition(EXIT_CONDITION)) {
    if (!SENSOR_STATE[2] && !SENSOR_STATE[3]) {
      writeMotor(1, SPEED, 1, SPEED);
      CORR_MULTIPLIER = 1.0f;
    }
    else if (!SENSOR_STATE[2] && SENSOR_STATE[3]) {
      CORR_MULTIPLIER *= DAMPING_FACTOR;
      if (CORR_MULTIPLIER < 0.20f) {
        CORR_MULTIPLIER = 0.20f;
      }
      uint8_t CORR_SPEED = uint8_t(SPEED * CORR_MULTIPLIER);
      writeMotor(1, SPEED, 1, CORR_SPEED);
    }
    else if (SENSOR_STATE[2] && !SENSOR_STATE[3]) {
      CORR_MULTIPLIER *= DAMPING_FACTOR;
      if (CORR_MULTIPLIER < 0.20f) {
        CORR_MULTIPLIER = 0.20f;
      }
      uint8_t CORR_SPEED = uint8_t(SPEED * CORR_MULTIPLIER);
      writeMotor(1, CORR_SPEED, 1, SPEED);
    }
    delay(LOOP_DELAY);
  }
  writeMotor(0, 0, 0, 0);
}

void dynamicFollow(int EXIT_CONDITION, uint8_t SPEED, int LOOP_DELAY, float DAMPING_FACTOR, uint8_t BLIP_SPEED, int BLIP_DURATION){
  float CORR_MULTIPLIER = 1.0f;  // Starts at 100 % of SPEED
  while (!exitCondition(EXIT_CONDITION)) {
    if (!SENSOR_STATE[2] && !SENSOR_STATE[3]) {
      writeMotor(1, BLIP_SPEED, 1, BLIP_SPEED);
      delay(BLIP_DURATION);
      writeMotor(1, SPEED, 1, SPEED);
      CORR_MULTIPLIER = 1.0f;
    }
    else if (!SENSOR_STATE[2] && SENSOR_STATE[3]) {
      CORR_MULTIPLIER *= DAMPING_FACTOR;
      if (CORR_MULTIPLIER < 0.20f) {
        CORR_MULTIPLIER = 0.20f;
      }
      uint8_t CORR_SPEED = uint8_t(SPEED * CORR_MULTIPLIER);
      writeMotor(1, BLIP_SPEED, 1, BLIP_SPEED);
      delay(BLIP_DURATION);
      writeMotor(1, SPEED, 1, CORR_SPEED);
    }
    else if (SENSOR_STATE[2] && !SENSOR_STATE[3]) {
      CORR_MULTIPLIER *= DAMPING_FACTOR;
      if (CORR_MULTIPLIER < 0.0f) {
        CORR_MULTIPLIER = 0.0f;
      }
      uint8_t CORR_SPEED = uint8_t(SPEED * CORR_MULTIPLIER);
      writeMotor(1, BLIP_SPEED, 1, BLIP_SPEED);
      delay(BLIP_DURATION);
      writeMotor(1, CORR_SPEED, 1, SPEED);
    }
    delay(LOOP_DELAY);
  }
  writeMotor(0, 0, 0, 0);
}

void highSpeedFollowLegacy(int EXIT_CONDITION, uint8_t SPEED, int LOOP_DELAY, float CORR_GAIN) {
  readUltrasonic(); // Update ultrasonic sensors
  uint16_t BASE_LEFT = US_STATE[0]; // Left calibration shot on function call
  uint16_t BASE_RIGHT = US_STATE[1]; // Right calibration shot on function call
  int16_t K = int16_t(CORR_GAIN * 1000.0f); // Convert CORR_GAIN to int for faster intiger math on 8bit AVR
  writeMotor(1, SPEED, 1, SPEED); // Set cruising speed
  while(!exitCondition(EXIT_CONDITION)) { // Run until exit condition met
    readUltrasonic(); // Update ultrasonic sensors, no need to update line sensors as they are called in exitCondition()
    int16_t ERR_LEFT = int16_t(US_STATE[0]) - int16_t(BASE_LEFT); // Calculate left error
    int16_t ERR_RIGHT = int16_t(US_STATE[1]) - int16_t(BASE_RIGHT); // Calculate right error
    int16_t ERROR = ERR_RIGHT - ERR_LEFT; // Calculate total error, + = far left, - = far right
    int16_t DELTA = (ERROR * K) / 1000; // Compute DELTA with correction factor and account for 1000
    if (DELTA > SPEED) DELTA = SPEED; // Clamp high DELTA
    if (DELTA < -SPEED) DELTA = -SPEED; // Clamp low DELTA
    uint8_t SPEED_LEFT  = uint8_t( constrain(int16_t(SPEED) - DELTA, 0, 255) ); // Calculate left speed with oflow protection
    uint8_t SPEED_RIGHT = uint8_t( constrain(int16_t(SPEED) + DELTA, 0, 255) ); // Calculate right speed with oflow protection
    writeMotor(1, SPEED_LEFT, 1, SPEED_RIGHT); // Write corrected speed
    delay(LOOP_DELAY); // Loop delay
  }
  writeMotor(0,0,0,0); // Stop motors on exit
}

void highSpeedFollow(int EXIT_CONDITION, uint8_t SPEED, int LOOP_DELAY, float CORR_GAIN) {
  readUltrasonic(); // Update ultrasonic sensors
  uint16_t BASE_LEFT = US_STATE[0]; // Left calibration shot
  uint16_t BASE_RIGHT = US_STATE[1]; // Right calibration shot
  int16_t  BASE_NUM = int16_t(BASE_RIGHT) - int16_t(BASE_LEFT); // Numerator for base ratio
  uint16_t BASE_DEN = BASE_LEFT + BASE_RIGHT; // Denominator for base ratio
  int16_t  BASE_RATIO = (int32_t(BASE_NUM) * 1000L) / BASE_DEN; // Base ratio ×1000
  int16_t  K = int16_t(CORR_GAIN * 1000.0f); // Convert CORR_GAIN to int for faster intiger math on 8bit AVR
  writeMotor(1, SPEED, 1, SPEED); // Set cruising speed
  while(!exitCondition(EXIT_CONDITION)) { // Run until exit condition met
    readUltrasonic(); // Update ultrasonic sensors
    uint16_t CUR_LEFT  = US_STATE[0]; // Current left
    uint16_t CUR_RIGHT = US_STATE[1]; // Current right
    int16_t  CUR_NUM = int16_t(CUR_RIGHT) - int16_t(CUR_LEFT); // Numerator
    uint16_t CUR_DEN = CUR_LEFT + CUR_RIGHT; // Denominator
    int16_t  CUR_RATIO = (int32_t(CUR_NUM) * 1000L) / CUR_DEN; // Current ratio ×1000
    int16_t  ERROR = CUR_RATIO - BASE_RATIO; // Proportional error
    int16_t  DELTA = (int32_t(ERROR) * K) / 1000; // Scaled correction
    if(DELTA >  SPEED) DELTA =  SPEED; // Clamp high
    if(DELTA < -SPEED) DELTA = -SPEED; // Clamp low
    int16_t TMP_LEFT  = constrain(int16_t(SPEED) - DELTA , 0, 255); // Calculate left speed with oflow protection
    int16_t TMP_RIGHT = constrain(int16_t(SPEED) + DELTA , 0, 255); // Calculate right speed with oflow protection
    writeMotor(1, uint8_t(TMP_LEFT), 1, uint8_t(TMP_RIGHT)); // Write corrected speed
    delay(LOOP_DELAY); // Loop delay
  }
  writeMotor(0,0,0,0); // Stop motors on exit
}

void leftTurn(int LOOP_DELAY, int BLIND_DELAY) {
  writeMotor(2, 255, 1, 255); // Full L differential
  delay(BLIND_DELAY); // Blind delay
  while (!SENSOR_STATE[2]) { // While CL sensor reads white, turn full differential left
    readSensors(); // Update sensors
    delay(LOOP_DELAY); // Loop delay
  }
  while (SENSOR_STATE[2]) { // CL sensor then detects line, keep turning while it detects the line
    readSensors(); // Update sensors
    delay(LOOP_DELAY); // Loop delay
  }
  writeMotor(0,0,0,0); // Stop motors on exit, turn finished
}

void leftTurn(uint8_t L_SPEED, uint8_t R_SPEED, int LOOP_DELAY, int BLIND_DELAY) {
  writeMotor(2, L_SPEED, 1, R_SPEED); // Custom L differential
  delay(BLIND_DELAY); // Blind delay
  while (!SENSOR_STATE[2]) { // While CL sensor reads white, turn custom differential left
    readSensors(); // Update sensors
    delay(LOOP_DELAY); // Loop delay
  }
  while (SENSOR_STATE[2]) { // CL sensor then detects line, keep turning while it detects the line
    readSensors(); // Update sensors
    delay(LOOP_DELAY); // Loop delay
  }
  writeMotor(0,0,0,0); // Stop motors on exit, turn finished
}

void rightTurn(int LOOP_DELAY, int BLIND_DELAY) {
  writeMotor(1, 255, 2, 255); // Full R differential
  delay(BLIND_DELAY); // Blind delay
  while (!SENSOR_STATE[3]) { // While CR sensor reads white, turn full differential right
    readSensors(); // Update sensors
    delay(LOOP_DELAY); // Loop delay
  }
  while (SENSOR_STATE[3]) { // CR sensor then detects line, keep turning while it detects the line
    readSensors(); // Update sensors
    delay(LOOP_DELAY); // Loop delay
  }
  writeMotor(0,0,0,0);
}

void rightTurn(uint8_t L_SPEED, uint8_t R_SPEED, int LOOP_DELAY, int BLIND_DELAY) {
  writeMotor(1, L_SPEED, 2, R_SPEED); // Custom R differential
  delay(BLIND_DELAY); // Blind delay
  while (!SENSOR_STATE[3]) { // While CR sensor reads white, turn custom differential right
    readSensors(); // Update sensors
    delay(LOOP_DELAY); // Loop delay
  }
  while (SENSOR_STATE[3]) { // CR sensor then detects line, keep turning while it detects the line
    readSensors(); // Update sensors
    delay(LOOP_DELAY); // Loop delay
  }
  writeMotor(0,0,0,0);
}


