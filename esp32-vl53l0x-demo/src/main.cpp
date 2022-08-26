#include <Arduino.h>
#include <ArduinoLog.h>
#include <Wire.h>

#include "Adafruit_VL53L0X.h"
#include "BluetoothSerial.h"

#define TIMING_BUDGET 30000
#define RANGE_THRESHOLD 600

Adafruit_VL53L0X lox = Adafruit_VL53L0X();
BluetoothSerial serialBT;

void setup() {
    Serial.begin(115200);

    // initialize logging
    while (!Serial && !Serial.available()) {
    }
    Log.begin(LOG_LEVEL_INFO, &Serial);
    Log.setShowLevel(true);
    Log.infoln("START");

    Log.infoln("Adafruit VL53L0X test.");
    if (!lox.begin()) {
        Log.errorln("Failed to boot VL53L0X");
        while (1)
            ;
    }
    serialBT.begin("ESP32BT_TEST");  // Bluetooth device name


    // start continuous ranging
    lox.setMeasurementTimingBudgetMicroSeconds(TIMING_BUDGET);
    lox.configSensor(lox.VL53L0X_SENSE_HIGH_SPEED);
    lox.startRangeContinuous();
}

void loop() {
    if (lox.isRangeComplete()) {
        uint16_t distance = lox.readRange();
        if (distance < 500) {
            Log.infoln("%d Distance: %d", millis(), distance);
            String s = String(millis()) + " range " + String(distance) + "mm ";
            serialBT.println(s);          
        } else {
            Log.infoln("%d No object detected.", millis());
        }
    } else {
        // Log.noticeln("%d not ready yet.", millis());
    }
}