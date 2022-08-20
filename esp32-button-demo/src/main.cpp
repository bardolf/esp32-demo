#include <Arduino.h>
#include <Bounce2.h>

#include "logging.h"

#define BOUNCE_PIN 25  // onboard button
#define LED_PIN 2      // onboard led

Bounce bounce = Bounce();
uint8_t ledState = LOW;

void readButtonTask(void *pvParameters) {
    while (1) {
        vTaskDelay(50 / portTICK_PERIOD_MS);
        bounce.update();
        if (bounce.changed()) {
            Log.infoln("CHNAGED");
            int deboucedInput = bounce.read();
            if (deboucedInput == LOW) {
                ledState = !ledState;
                digitalWrite(LED_PIN, ledState);
            }
        }
    }
}

void setup() {
    Serial.begin(115200);

    // initialize logging
    while (!Serial && !Serial.available()) {
    }
    Log.begin(LOG_LEVEL_VERBOSE, &Serial);
    Log.setPrefix(printPrefix);
    Log.setShowLevel(false);
    Log.infoln("START");

    bounce.attach(BOUNCE_PIN, INPUT_PULLUP);
    bounce.interval(10);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, ledState);

    xTaskCreatePinnedToCore(readButtonTask, "Read Button Task", 2024, NULL, 3, NULL, ARDUINO_RUNNING_CORE);
}

void loop() {
    // ideally do nothing
    vTaskDelay(10000 / portTICK_PERIOD_MS);
}
