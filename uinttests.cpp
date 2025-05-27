#include "unittests.h"
#include <avr/pgmspace.h>

void printFromPROGMEM(const char* p) {
  char c;
  while ((c = pgm_read_byte(p++))) {
    Serial.write(c);
    delay(1);
  }
}

void clearConsole() {
  for (int i = 0; i < 50; i++) Serial.println();
}

void testSensors() {
  clearConsole();
  printFromPROGMEM(SENSOR_LOGO);
  Serial.println("Press [Enter] to continue...");

  while (true) {
    if (Serial.available() && Serial.read() == '\n') break;
  }

  clearConsole();

  auto testSingleSensor = [](const char* label, int index) {
    String lastColor = "";

    while (true) {
      readSensors();
      String currentColor = SENSOR_STATE[index] ? "Black" : "White";

      // Only update if the color changed
      if (currentColor != lastColor) {
        clearConsole();
        Serial.println(label);
        Serial.print("Color: ");
        Serial.println(currentColor);
        lastColor = currentColor;
      }

      // Exit when Enter is pressed
      if (Serial.available() && Serial.read() == '\n') break;

      delay(100);
    }

    clearConsole();  // Clear before next sensor
  };

  testSingleSensor("Testing TL Sensor", 0);
  testSingleSensor("Testing TR Sensor", 1);
  testSingleSensor("Testing CL Sensor", 2);
  testSingleSensor("Testing CR Sensor", 3);
  testSingleSensor("Testing BL Sensor", 4);
  testSingleSensor("Testing BR Sensor", 5);

  clearConsole();
  Serial.println("Exit condition tests.");
  Serial.println("Press [Enter] to continue...");

  while (true) {
    if (Serial.available() && Serial.read() == '\n') break;
  }

  auto testingExitCondition = [](const char* label, int index) {
    clearConsole();
    Serial.print("Exit condition ");
    Serial.print(index);
    Serial.print(": ");
    Serial.println(label);
    while (!exitCondition(index));
    Serial.println("Latched");
    Serial.println("Press [Enter] to continue...");
    while (true) {
      if (Serial.available() && Serial.read() == '\n') break;
    }
  };

  testingExitCondition("Both front sensors", 1);
  testingExitCondition("Both rear sensors", 1);

  Serial.println("Sensors Testing Complete");
}

void testMotors() {
  clearConsole();
  printFromPROGMEM(MOTOR_LOGO);
  Serial.println("Press [Enter] to continue...");
  while (true) {
    if (Serial.available() && Serial.read() == '\n') break;
  }

  clearConsole();
  writeMotor(0,0,0);
  writeMotor(1,0,0);
  Serial.println("Motor 1       :   0%");
  Serial.println("Motor 2       :   0%");
  Serial.println("Write Method  :   Individual");
  Serial.println("Press [Enter] to continue...");
  while (true) {
    if (Serial.available() && Serial.read() == '\n') break;
  }

  clearConsole();
  writeMotor(0,1,255);
  writeMotor(1,2,255);
  Serial.println("Motor 1       :   100% FWD");
  Serial.println("Motor 2       :   100% REV");
  Serial.println("Write Method  :   Individual");
  Serial.println("Press [Enter] to continue...");
  while (true) {
    if (Serial.available() && Serial.read() == '\n') break;
  }

  clearConsole();
  writeMotor(0,2,255);
  writeMotor(1,1,255);
  Serial.println("Motor 1       :   100% REV");
  Serial.println("Motor 2       :   100% FWD");
  Serial.println("Write Method  :   Individual");
  Serial.println("Press [Enter] to continue...");
  while (true) {
    if (Serial.available() && Serial.read() == '\n') break;
  }

  clearConsole();
  writeMotor(0,0,0,0);
  Serial.println("Motor 1       :   0%");
  Serial.println("Motor 2       :   0%");
  Serial.println("Write Method  :   Grouped");
  Serial.println("Press [Enter] to continue...");
  while (true) {
    if (Serial.available() && Serial.read() == '\n') break;
  }

  clearConsole();
  writeMotor(1,127,1,127);
  Serial.println("Motor 1       :   50% FWD");
  Serial.println("Motor 2       :   50% FWD");
  Serial.println("Write Method  :   Grouped");
  Serial.println("Press [Enter] to continue...");
  while (true) {
    if (Serial.available() && Serial.read() == '\n') break;
  }

  clearConsole();
  writeMotor(2,127,2,127);
  Serial.println("Motor 1       :   50% REV");
  Serial.println("Motor 2       :   50% REV");
  Serial.println("Write Method  :   Grouped");
  Serial.println("Press [Enter] to continue...");
  while (true) {
    if (Serial.available() && Serial.read() == '\n') break;
  }
  clearConsole();
  writeMotor(0,0,0,0);

  Serial.println("Motors Testing Complete");
}

void testUltrasonics() {
  clearConsole();
  printFromPROGMEM(US_LOGO);
  Serial.println("Press [Enter] to continue...");

  while (true) {
    if (Serial.available() && Serial.read() == '\n') break;
  }

  clearConsole();
  while (true) {
    readUltrasonic();
    uint16_t lsensor = US_STATE[0];
    uint16_t rsensor = US_STATE[1];

    // Only update if the color changed
    clearConsole();
    Serial.print("L Sensor: ");
    Serial.println(lsensor);
    Serial.print("R Sensor: ");
    Serial.println(rsensor);

    // Exit when Enter is pressed
    if (Serial.available() && Serial.read() == '\n') break;

    delay(100);
  }
  clearConsole();
}