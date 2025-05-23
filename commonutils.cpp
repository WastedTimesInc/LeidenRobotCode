#include "commonutils.h"

void writeI2C(uint8_t ADDR, uint8_t VAL){
  Wire.beginTransmission(ADDR);
  Wire.write(VAL);
  Wire.endTransmission();
}

void readSensors() {
  for (int i = 0; i < 6; i++) {
    SENSOR_STATE[i] = true;
  }
}

int junctionDetect(int POS) {
  //Returns 0 for X
  //Returns 1 for T to left 
  //Returns 2 for T to right
  //Returns -1 for error
  switch (POS) {
    case 1:
      if (SENSOR_STATE[0] && SENSOR_STATE[1]) {
        return 0;
      } else if (SENSOR_STATE[0] && !SENSOR_STATE[1]) {
        return 1;
      } else if (!SENSOR_STATE[0] && SENSOR_STATE[1]) {
        return 2;
      }
      return -1;
      break;
    case 2:
      if (SENSOR_STATE[4] && SENSOR_STATE[5]) {
        return 0;
      } else if (SENSOR_STATE[4] && !SENSOR_STATE[5]) {
        return 1;
      } else if (!SENSOR_STATE[4] && SENSOR_STATE[5]) {
        return 2;
      }
      return -1;
      break;
    default:
      return -1;
      break;
  }
}

bool exitCondition(int CHECK) {
  //EXIT CONDITIONS ARE AS FOLLOWS
  // 1 - Detect both front sensors
  // 2 - Detect both rear sensors
  // 3 - Detect A front sensors
  // 4 - Detect A rear sensor
  // 5 - Detect both central sensors
  // 6 - Detect A central sensor
  // 7 - Detect TL Sensor
  // 8 - Detect TR Sensor
  // 9 - Detect CL Sensor
  // 10 - Detect CR Sensor
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

    case 4: // Detect any rear sensor (fixed)
      if (SENSOR_STATE[4] || SENSOR_STATE[5])
        return true;
      break;

    case 5:
      if (SENSOR_STATE[2] && SENSOR_STATE[3])
        return true;
      break;

    case 6:
      if (SENSOR_STATE[2] || SENSOR_STATE[3])
        return true;
      break;

    case 7:
      if (SENSOR_STATE[0])
        return true;
      break;

    case 8:
      if (SENSOR_STATE[1])
        return true;
      break;

    case 9:
      if (SENSOR_STATE[2])
        return true;
      break;

    case 10:
      if (SENSOR_STATE[3])
        return true;
      break;


    default:
      break;
  }

  return false;
}

void initSystems(bool VERBOSE) {
  if (VERBOSE == true) {
    Serial.begin(115200);
  }
  pinMode(SPD_PIN_A, OUTPUT);
  pinMode(SPD_PIN_B, OUTPUT);
  for (int i = 0; i < 6; i++) {
    pinMode(SENSOR_PINS[i], OUTPUT);
  }
  Wire.begin();
  readSensors();
}

void initSystems() {
  pinMode(SPD_PIN_A, OUTPUT);
  pinMode(SPD_PIN_B, OUTPUT);
  for (int i = 0; i < 6; i++) {
    pinMode(SENSOR_PINS[i], OUTPUT);
  }
  Wire.begin();
  readSensors();
}