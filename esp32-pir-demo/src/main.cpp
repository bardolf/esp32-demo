#include <Arduino.h>
#include <FastLED.h>

#define NUM_LEDS 1

#define DATA_PIN_ONBOARD 5
#define PIN_TO_SENSOR 26

#define DATA_PIN DATA_PIN_EXT_D6

CRGB leds[NUM_LEDS];
int pinStateCurrent   = LOW;  // current state of pin
int pinStatePrevious  = LOW;  // previous state of pin

void setup() {
    Serial.begin(921600);
    pinMode(PIN_TO_SENSOR, INPUT);
    FastLED.addLeds<NEOPIXEL, DATA_PIN_ONBOARD>(leds, NUM_LEDS);  // GRB ordering is assumed
}

void loop() {
    pinStatePrevious = pinStateCurrent;
    pinStateCurrent = digitalRead(PIN_TO_SENSOR);

    if (pinStatePrevious == LOW && pinStateCurrent == HIGH) {  // pin state change: LOW -> HIGH
        Serial.println("Motion detected!");
        leds[0] = CRGB::Red;
        FastLED.show();
    } else if (pinStatePrevious == HIGH && pinStateCurrent == LOW) {  // pin state change: HIGH -> LOW
        Serial.println("Motion stopped!");
        leds[0] = CRGB::Green;
        FastLED.show();
    }
}