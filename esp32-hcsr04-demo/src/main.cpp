#include <Arduino.h>

#include "detector.h"
#include "logging.h"

Detector detector;

void readDetectorTask(void *pvParameters) {
    while (1) {
        DetectedObjectState detectedObjectState = detector.read();
        if (detectedObjectState == ARRIVED) {
            Log.infoln("%d object arrived", millis());
        } else if (detectedObjectState == LEFT) {
            Log.infoln("%d object left", millis());
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
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

    detector.init();
    detector.startMeasurement();
    xTaskCreatePinnedToCore(readDetectorTask, "Read detector", 8000, NULL, 4, NULL, ARDUINO_RUNNING_CORE);
}

void loop() {
    vTaskDelay(10000 / portTICK_PERIOD_MS);  // do nothing in loop
}