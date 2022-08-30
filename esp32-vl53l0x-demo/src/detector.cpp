#include "detector.h"

#include "logging.h"

#define TIMING_BUDGET 20000
#define RANGE_THRESHOLD 600

#define MEASUREMENTS_LENGTH 3
#define MEASUREMENT_TOLERANCE 50

Detector::Detector() {
    _measurements = new long[MEASUREMENTS_LENGTH];
}

void Detector::clearMeasurements() {
    for (int i = 0; i < MEASUREMENTS_LENGTH; i++) {
        _measurements[i] = -1;
    }
}

bool Detector::init() {
    clearMeasurements();
    _lox = new Adafruit_VL53L0X();
    if (_lox->begin()) {
        _lox->configSensor(_lox->VL53L0X_SENSE_LONG_RANGE);
        _lox->setMeasurementTimingBudgetMicroSeconds(TIMING_BUDGET);
        _lox->startRangeContinuous();
        return true;
    }
    return false;
}

void Detector::addMeasurement(long value) {    
    for (int i = MEASUREMENTS_LENGTH - 1; i >= 1; i--) {
        _measurements[i] = _measurements[i - 1];
    }
    _measurements[0] = value;
}

void Detector::startMeasurement() {
    _prevObjectDetected = false;
    _measurementEnabled = true;
}

void Detector::stopMeasurement() {
    _measurementEnabled = false;
}

void Detector::update() {
    if (!_measurementEnabled) {
        return;
    }
    if (_lox->isRangeComplete()) {
        uint16_t range = _lox->readRange();
        if (range > 0 && range < RANGE_THRESHOLD) {
            addMeasurement(range);
        } else {
            addMeasurement(-1);
        }
    }
}

bool Detector::isObjectLeft() {
    if (!_measurementEnabled) {
        return false;
    }
    if (!_prevObjectDetected) {
        return false;
    }
    for (int i = 0; i < MEASUREMENTS_LENGTH; i++) {
        if (_measurements[i] > 0) {
            return false;
        }
    }    
    _prevObjectDetected = false;
    return true;
}

bool Detector::isObjectArrived() {
    if (!_measurementEnabled) {
        return false;
    }
    if (_prevObjectDetected) {
        return false;
    }
    long sum = 0;
    for (int i = 0; i < MEASUREMENTS_LENGTH; i++) {
        if (_measurements[i] <= 0 || _measurements[i] > RANGE_THRESHOLD) {
            return false;
        }
        sum += _measurements[i];
    }
    long avg = sum / MEASUREMENTS_LENGTH;
    for (int i = 0; i < MEASUREMENTS_LENGTH; i++) {
        if (abs(_measurements[i] - avg) > MEASUREMENT_TOLERANCE) {
            return false;
        }
    }    
    _prevObjectDetected = true;
    return true;
}
