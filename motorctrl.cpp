#include "motorctrl.h"

void writeMotor(uint8_t MOTOR, uint8_t DIR, uint8_t SPEED) {
  switch (MOTOR) {
    case 0:
      switch (DIR) {
        case 0:
          CURRENT = (M_NULL & MOTOR_A_MASK) | (CURRENT & MOTOR_A_INV_MASK);
          MOTOR_A_STATE = 0;
          break;
        case 1:
          CURRENT = (MOTOR_A_FWD & MOTOR_A_MASK) | (CURRENT & MOTOR_A_INV_MASK);
          MOTOR_A_STATE = 1;
          break;
        case 2:
          CURRENT = (MOTOR_A_REV & MOTOR_A_MASK) | (CURRENT & MOTOR_A_INV_MASK);
          MOTOR_A_STATE = 2;
          break;
        default:
          Serial.println("OUT OF RANGE");
          return;
      }
      writeI2C(CTRL_ADDR, CURRENT);

      if (SPEED < 256) {
        analogWrite(SPD_PIN_A, SPEED);
        MOTOR_A_SPEED = SPEED;
      } else {
        Serial.println("OUT OF RANGE");
      }
      break;

    case 1:
      switch (DIR) {
        case 0:
          CURRENT = (M_NULL & MOTOR_B_MASK) | (CURRENT & MOTOR_B_INV_MASK);
          MOTOR_A_STATE = 0;
          break;
        case 1:
          CURRENT = (MOTOR_B_FWD & MOTOR_B_MASK) | (CURRENT & MOTOR_B_INV_MASK);
          MOTOR_A_STATE = 1;
          break;
        case 2:
          CURRENT = (MOTOR_B_REV & MOTOR_B_MASK) | (CURRENT & MOTOR_B_INV_MASK);
          MOTOR_A_STATE = 2;
          break;
        default:
          Serial.println("OUT OF RANGE");
          return;
      }
      writeI2C(CTRL_ADDR, CURRENT);

      if (SPEED < 256) {
        analogWrite(SPD_PIN_B, SPEED);
        MOTOR_B_SPEED = SPEED;
      } else {
        Serial.println("OUT OF RANGE");
      }
      break;

    default:
      Serial.println("INVALID MOTOR");
      break;
  }
}

void writeMotor(uint8_t DIR0, uint8_t SPEED0, uint8_t DIR1, uint8_t SPEED1) {
  // Motor A control
  switch (DIR0) {
    case 0:
      CURRENT = (M_NULL & MOTOR_A_MASK) | (CURRENT & MOTOR_A_INV_MASK);
      MOTOR_A_STATE = 0;
      break;
    case 1:
      CURRENT = (MOTOR_A_FWD & MOTOR_A_MASK) | (CURRENT & MOTOR_A_INV_MASK);
      MOTOR_A_STATE = 1;
      break;
    case 2:
      CURRENT = (MOTOR_A_REV & MOTOR_A_MASK) | (CURRENT & MOTOR_A_INV_MASK);
      MOTOR_A_STATE = 2;
      break;
    default:
      Serial.println("DIR0 OUT OF RANGE");
      return;
  }

  // Motor B control
  switch (DIR1) {
    case 0:
      CURRENT = (M_NULL & MOTOR_B_MASK) | (CURRENT & MOTOR_B_INV_MASK);
      MOTOR_B_STATE = 0;
      break;
    case 1:
      CURRENT = (MOTOR_B_FWD & MOTOR_B_MASK) | (CURRENT & MOTOR_B_INV_MASK);
      MOTOR_B_STATE = 1;
      break;
    case 2:
      CURRENT = (MOTOR_B_REV & MOTOR_B_MASK) | (CURRENT & MOTOR_B_INV_MASK);
      MOTOR_B_STATE = 2;
      break;
    default:
      Serial.println("DIR1 OUT OF RANGE");
      return;
  }

  // Write combined I2C update once
  writeI2C(CTRL_ADDR, CURRENT);

  // Motor A speed
  if (SPEED0 < 256) {
    analogWrite(SPD_PIN_A, SPEED0);
    MOTOR_A_SPEED = SPEED0;
  } else {
    Serial.println("SPEED0 OUT OF RANGE");
  }

  // Motor B speed
  if (SPEED1 < 256) {
    analogWrite(SPD_PIN_B, SPEED1);
    MOTOR_B_SPEED = SPEED1;
  } else {
    Serial.println("SPEED1 OUT OF RANGE");
  }
}