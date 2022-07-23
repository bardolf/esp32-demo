#include <Arduino.h>
#include <FastLED.h>

#define NUM_LEDS 1

#define DATA_PIN_ONBOARD 5
#define DATA_PIN_EXT_D6 14

#define DATA_PIN DATA_PIN_EXT_D6


CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(921600);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed
}

void loop() { 
  leds[0] = CRGB::Red;
  FastLED.show();
  delay(500);
  
  leds[0] = CRGB::Green;
  FastLED.show();
  delay(500);
  
  leds[0] = CRGB::Blue;
  FastLED.show();
  delay(500);
}