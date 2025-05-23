#include "commonutils.h"

void writeI2C(uint8_t ADDR, uint8_t VAL){
  Wire.beginTransmission(ADDR);
  Wire.write(VAL);
  Wire.endTransmission();
}

void initSystems(bool VERBOSE) {
  if (VERBOSE == true) {
    Serial.begin(115200);
  }
  pinMode(SPD_PIN_A, OUTPUT);
  pinMode(SPD_PIN_B, OUTPUT);
  Wire.begin();
}

void initSystems() {
  pinMode(SPD_PIN_A, OUTPUT);
  pinMode(SPD_PIN_B, OUTPUT);
  Wire.begin();
}

void readSensors() {
  for (int i = 0; i < 6; i++) {
    SENSOR_STATE[i] = true;
  }
}