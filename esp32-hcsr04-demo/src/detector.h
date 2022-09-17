#ifndef detector_h
#define detector_h

#include <Arduino.h>
#include "BluetoothSerial.h"

#define TRIGGER_PIN 26
#define ECHO_PIN 25
#define RANGE_THRESHOLD_CM 80
#define SOUND_SPEED_HALF 0.017

typedef enum {
  NONE,
  ARRIVED,
  LEFT
} DetectedObjectState;

class Detector {
   public:
    Detector();
    void init(BluetoothSerial* serialBT);
    DetectedObjectState read();
    void startMeasurement();
    void stopMeasurement();    

   private:
    BluetoothSerial* _serialBT;
    bool _prevObjectDetected;
    bool _measurementEnabled = false;
    float _prevDistance;
    unsigned long _detectorPulseInTimeout;
    float measureDistance();    
};

#endif
