#include "commonutils.h"

void writeI2C(uint8_t ADDR, uint8_t VAL){
  Wire.beginTransmission(ADDR); // Open transimssion at ADDR
  Wire.write(VAL); // Write VAL
  Wire.endTransmission(); // Close transmission
}

void readSensors() {
  for (int i = 0; i < 2; i++) {
    SENSOR_STATE[i] = !digitalRead(SENSOR_PINS[i]);
  }
  for (int i = 2; i < 6; i++) {
    SENSOR_STATE[i] = !(analogRead(SENSOR_PINS[i]) > SENSOR_THRESHOLD);
  }
}

void readUltrasonic() {
  US_STATE[0] = US_SENSORS[0].ping_median(1);
  US_STATE[1] = US_SENSORS[1].ping_median(1);
}

void readUltrasonic(uint8_t PING_ITER) {
  US_STATE[0] = US_SENSORS[0].ping_median(PING_ITER);
  US_STATE[1] = US_SENSORS[1].ping_median(PING_ITER);
}

int junctionDetect(int POS) {
  switch (POS) { // Switch on POS to select front or rear sensors
    case 1: // Checks both front sensors
      if (SENSOR_STATE[0] && SENSOR_STATE[1]) {
        return 0; // X Junction detected (0 & 1 true)
      } else if (SENSOR_STATE[0] && !SENSOR_STATE[1]) {
        return 1; // T Junction (left) detected (0 true & 1 false)
      } else if (!SENSOR_STATE[0] && SENSOR_STATE[1]) {
        return 2; // T Junction (right) detected (0 false & 1 true)
      }
      return -1; // General return error
      break;
    case 2: // Same as above...
      if (SENSOR_STATE[4] && SENSOR_STATE[5]) {
        return 0;
      } else if (SENSOR_STATE[4] && !SENSOR_STATE[5]) {
        return 1;
      } else if (!SENSOR_STATE[4] && SENSOR_STATE[5]) {
        return 2;
      }
      return -1;
      break;
    default: // Default return error
      return -1;
      break;
  }
}

bool exitCondition(int CHECK) {
  readSensors();  // Update sensor values
  switch (CHECK) {
    case 1: // Detect both front sensors
      if (SENSOR_STATE[0] && SENSOR_STATE[1])
        return true;
      break;

    case 2: // Detect both rear sensors
      if (SENSOR_STATE[4] && SENSOR_STATE[5])
        return true;
      break;

    case 3: // Detect any front sensor
      if (SENSOR_STATE[0] || SENSOR_STATE[1])
        return true;
      break;

    case 4: // Detect any rear sensor
      if (SENSOR_STATE[4] || SENSOR_STATE[5])
        return true;
      break;

    case 5: // Detect both central sensors
      if (SENSOR_STATE[2] && SENSOR_STATE[3])
        return true;
      break;

    case 6: // Detect any central sensor
      if (SENSOR_STATE[2] || SENSOR_STATE[3])
        return true;
      break;

    case 7: // Detect TL sensor
      if (SENSOR_STATE[0])
        return true;
      break;

    case 8: // Detect TR sensor
      if (SENSOR_STATE[1])
        return true;
      break;

    case 9: // Detect CL sensor
      if (SENSOR_STATE[2])
        return true;
      break;

    case 10: // Detect CR sensor
      if (SENSOR_STATE[3])
        return true;
      break;
    default: // Default break and return false
      break;
  }

  return false;
}

void initSystems(bool VERBOSE) {
  if (VERBOSE == true) {
    Serial.begin(115200); // If verbose, begin serial
  }
  pinMode(SPD_PIN_A, OUTPUT); // Init motor A pwm pin
  pinMode(SPD_PIN_B, OUTPUT); // Init motor B pwm pin
  for (int i = 0; i < 6; i++) { // For loop for sensors
    pinMode(SENSOR_PINS[i], INPUT); // Init sensor i pin
  }
  Wire.begin(); // Init I2C
  readSensors(); // Pre read sensors
}

void initSystems() {
  if (GLOBAL_VERBOSE == true) {
    Serial.begin(115200); // If global verbose, begin serial
  }
  pinMode(SPD_PIN_A, OUTPUT); // Init motor A pwm pin
  pinMode(SPD_PIN_B, OUTPUT); // Init motor B pwm pin
  for (int i = 0; i < 6; i++) { // For loop for sensors
    pinMode(SENSOR_PINS[i], INPUT); // Init sensor i pin
  }
  Wire.begin(); // Init I2C
  readSensors(); // Pre read sensors
}