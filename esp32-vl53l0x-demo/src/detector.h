#ifndef detector_h
#define detector_h

#include <Adafruit_VL53L0X.h>
#include <Arduino.h>
#include <Wire.h>

class Detector {
   public:
    Detector();
    bool init();
    void update();
    void startMeasurement();
    void stopMeasurement();
    bool isObjectLeft();
    bool isObjectArrived();

   private:    
    Adafruit_VL53L0X* _lox;
    long* _measurements;
    bool _prevObjectDetected;

    void addMeasurement(long value);
    void clearMeasurements();
    bool _measurementEnabled = false;    
};

#endif