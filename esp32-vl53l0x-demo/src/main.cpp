#include <Arduino.h>
#include <ArduinoLog.h>
#include <Wire.h>

#include "Adafruit_VL53L0X.h"
#include "BluetoothSerial.h"
#include "detector.h"

#define TIMING_BUDGET 20000
#define RANGE_THRESHOLD 600

BluetoothSerial serialBT;
Detector detector;

void updateDetectorTask(void *pvParameters) {
    while (1) {
        detector.update();
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
}

void readDetectorTask(void *pvParameters) {
    while (1) {
        if (detector.isObjectArrived()) {
            Log.infoln("Object arrived.");
            String s = String("Object arrived");
            serialBT.println(s);
        }
        if (detector.isObjectLeft()) {
            Log.infoln("Object left.");
            String s = String("Object left");
            serialBT.println(s);
        }
        vTaskDelay(3 / portTICK_PERIOD_MS);
    }
}

void setup() {
    Serial.begin(115200);

    // initialize logging
    while (!Serial && !Serial.available()) {
    }
    Log.begin(LOG_LEVEL_INFO, &Serial);
    Log.setShowLevel(true);
    Log.infoln("START");

    Log.infoln("Adafruit VL53L0X test.");
    if (!detector.init()) {
        Log.errorln("Failed to boot VL53L0X");
        while (1)
            ;
    }
    serialBT.begin("ESP32BT_TEST");  // Bluetooth device name

    xTaskCreatePinnedToCore(updateDetectorTask, "Upd. detector", 8000, NULL, 4, NULL, ARDUINO_RUNNING_CORE);
    xTaskCreatePinnedToCore(readDetectorTask, "Read detector", 8000, NULL, 4, NULL, ARDUINO_RUNNING_CORE);
    detector.startMeasurement();
}

void loop() {
    vTaskDelay(10000 / portTICK_PERIOD_MS);  // do nothing in loop
}