#include "detector.h"

Detector::Detector() {
    _detectorPulseInTimeout = 2 * (unsigned long)((float)RANGE_THRESHOLD_CM / (float)SOUND_SPEED_HALF);
}

void Detector::startMeasurement() {
    _prevObjectDetected = false;
    _measurementEnabled = true;
}

void Detector::stopMeasurement() {
    _measurementEnabled = false;
}

void Detector::init(BluetoothSerial* serialBT) {
    pinMode(TRIGGER_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT_PULLDOWN);
    _serialBT = serialBT;
}

float Detector::measureDistance() {
    digitalWrite(TRIGGER_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN, LOW);
    // Serial.print(millis());
    // Serial.print("ms ");
    long duration = pulseIn(ECHO_PIN, HIGH, _detectorPulseInTimeout);
    // Serial.print(millis());
    // Serial.print("ms ~ ");
    // Serial.println(duration);
    if (duration == 0) {  // pulseIn timeout
        return 10 *  ;
    }
    return duration * SOUND_SPEED_HALF;
}

DetectedObjectState Detector::read() {
    if (_measurementEnabled) {
        float distance = measureDistance();
        // if (distance < RANGE_THRESHOLD_CM) {
        //     String s = String("> ") + millis() + " distance: " + distance + " cm";
        //     _serialBT->println(s);
        // }
        if (distance > RANGE_THRESHOLD_CM && _prevObjectDetected) {
            _prevObjectDetected = false;
            Serial.println(distance);
            String s = String("> LEFT ") + millis() + " distance: " + distance + " cm";
            _serialBT->println(s);
            return LEFT;
        } else if (distance <= RANGE_THRESHOLD_CM && !_prevObjectDetected) {
            _prevObjectDetected = true;
            Serial.println(distance);
            Serial.println(distance);
            String s = String("> ARRIVED ") + millis() + " distance: " + distance + " cm";
            _serialBT->println(s);
            return ARRIVED;
        }
        _prevDistance = distance;
    }
    return NONE;
}
