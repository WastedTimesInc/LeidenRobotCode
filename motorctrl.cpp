#include "motorctrl.h"

void writeMotor(uint8_t MOTOR, uint8_t DIR, uint8_t SPEED) {
  switch (MOTOR) { // Switch to select between motors
    case 0: // Left motor selected
      switch (DIR) { // Switch to select direction
        case 0: // Motor stopped
          CURRENT = (M_NULL & MOTOR_A_MASK) | (CURRENT & MOTOR_A_INV_MASK); // Append current to stop selected motor
          MOTOR_A_STATE = 0; // Write motor state stopped
          break;
        case 1:
          CURRENT = (MOTOR_A_FWD & MOTOR_A_MASK) | (CURRENT & MOTOR_A_INV_MASK); // Append current to fwd selected motor
          MOTOR_A_STATE = 1; // Write motor state fwd
          break;
        case 2:
          CURRENT = (MOTOR_A_REV & MOTOR_A_MASK) | (CURRENT & MOTOR_A_INV_MASK); // Append current to rev selected motor
          MOTOR_A_STATE = 2; // Write motor state rev
          break;
        default: // Error returns function
          Serial.println("OUT OF RANGE");
          return;
      }
      writeI2C(CTRL_ADDR, CURRENT); // Write appended CURRENT to motor control chip

      if (SPEED < 256) { // Validate speed (not really needed since uint8_t)
        analogWrite(SPD_PIN_A, SPEED); // Write speed to corresponding motor pwm pin
        MOTOR_A_SPEED = SPEED; // Update motor speed state
      } else {
        Serial.println("OUT OF RANGE"); // Out of range error (should NEVER occur)
      }
      break;

    case 1: // All comments same as above but for motor B (right)
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

      SPEED = SPEED*245/255;
    
      if (SPEED < 256) {
        analogWrite(SPD_PIN_B, SPEED);
        MOTOR_B_SPEED = SPEED;
      } else {
        Serial.println("OUT OF RANGE");
      }
      break;

    default: // Error handling for unknown motor
      Serial.println("INVALID MOTOR");
      break;
  }
}

void writeMotor(uint8_t DIR0, uint8_t SPEED0, uint8_t DIR1, uint8_t SPEED1) {
  // Motor A control, similar arch to above
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

  // Motor B control, similar arch to above
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

  SPEED1 = SPEED1*245/255;
  // Motor B speed
  if (SPEED1 < 256) {
    analogWrite(SPD_PIN_B, SPEED1);
    MOTOR_B_SPEED = SPEED1;
  } else {
    Serial.println("SPEED1 OUT OF RANGE");
  }
}