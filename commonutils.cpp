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
  US_STATE[0] = US_SENSORS[0].ping_median(5);
  US_STATE[1] = US_SENSORS[1].ping_median(5);
}

void readUltrasonic(uint8_t PING_ITER) {
  US_STATE[0] = US_SENSORS[0].ping_median(PING_ITER);
  US_STATE[1] = US_SENSORS[1].ping_median(PING_ITER);
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

void writeStepper(int STEPS, int PAUSE_DELAY, bool REV) {
  switch (REV) {
    case false : {
      for (int step = 0; step < STEPS; step++) {
        for (int phase = 0; phase < 4; phase++) {
          for (int pin = 0; pin < 4; pin++) {
            digitalWrite(STEPPER_PINS[pin], (pin == phase) ? HIGH : LOW);
          }
          delayMicroseconds(PAUSE_DELAY);
        }
      }
      break;
    }
    case true : {
      for (int step = 0; step < STEPS; step++) {
        for (int phase = 3; phase >= 0; phase--) {
          for (int pin = 0; pin < 4; pin++) {
            digitalWrite(STEPPER_PINS[pin], (pin == phase) ? HIGH : LOW);
          }
          delayMicroseconds(PAUSE_DELAY);
        }
      }
      break;      
    }
  }
}