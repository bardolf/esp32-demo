#include <Arduino.h>

#include "logging.h"

const int trigPin = 26;  
const int echoPin = 25;  

#define SOUND_SPEED 0.034  // definice rychlosti zvuku (cm/us)

long duration;     
float distanceCm;  

void setup() {
    Serial.begin(115200);      
    pinMode(trigPin, OUTPUT);  
    pinMode(echoPin, INPUT_PULLDOWN);   
}

// funkce LOOP bezi stale dokola.
void loop() {
    digitalWrite(trigPin, LOW);   // nastaveni TRIG na uroven LOW
    delayMicroseconds(2);         // 2us pockat, aby se se LOW ustalilo
    digitalWrite(trigPin, HIGH);  // zacatek 10us startovniho pulzu (TRIG na HIGH)
    delayMicroseconds(10);        // cekani 10us
    digitalWrite(trigPin, LOW);   // konec start pulze (trig na LOW)
    duration = pulseIn(echoPin, HIGH);  // urci pocet us stavu HIGH na pinu ECHO

    distanceCm = duration * SOUND_SPEED / 2;  // vypocet vzdalenosti z casu a rychlosti
    if (distanceCm < 100) {
        Serial.print(millis());
        Serial.print(" object detected [cm]: ");
        Serial.println(distanceCm);
    }
    delay(10);  // 5s pocakt, pak nasleduje dalsi mereni
}