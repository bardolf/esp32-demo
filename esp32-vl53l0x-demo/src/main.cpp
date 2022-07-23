#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_VL53L0X.h"

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

void setup() {
  Serial.begin(921600);
  // wait until serial port opens for native USB devices
  while (! Serial) {
    delay(1);
  }

  Serial.println("Adafruit VL53L0X test.");
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }
  // power
  Serial.println(F("VL53L0X API Continuous Ranging example\n\n"));

  // start continuous ranging
  lox.setMeasurementTimingBudgetMicroSeconds(20000);
  lox.startRangeContinuous();
}

void loop() {
  if (lox.isRangeComplete()) {
    // Serial.print("Distance in mm: ");
    uint16_t distance = lox.readRange();
    if (distance < 500) {
      Serial.print("HERE ");
      Serial.println(distance);
    }
  }
}