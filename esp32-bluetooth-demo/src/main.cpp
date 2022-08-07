#include <Arduino.h>

#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial serialBT;

void setup() {
    // Serial.begin(19200);
    serialBT.begin("ESP32_1");  // Bluetooth device name
    
    // Serial.println("The device started, now you can pair it with bluetooth!");
}

void loop() {
    serialBT.write(1); 
    delay(2000);
}