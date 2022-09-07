#include <Arduino.h>

#define ANALOG_PIN 36

int value = 0;

void setup() {
    Serial.begin(115200);
    delay(1000);
}

void loop() {
    // Reading analog value
    value = analogRead(ANALOG_PIN);
    Serial.println(value);
    delay(500);
}