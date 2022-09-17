#ifndef detector_h
#define detector_h

#include <Adafruit_VL53L0X.h>
#include <Arduino.h>
#include <Wire.h>

#define TIMING_BUDGET 20000
#define RANGE_THRESHOLD 600
#define WINDOW_SIZE 20

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
    uint32_t _ambient[WINDOW_SIZE]; 
    uint32_t _signal[WINDOW_SIZE];     
};

#endif