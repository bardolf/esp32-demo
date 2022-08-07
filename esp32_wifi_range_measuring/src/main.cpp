#include <Arduino.h>
#include <Bounce2.h>
#include <WiFi.h>
#include <Wire.h>
#include <esp_now.h>

#include "logging.h"

#define DEVICE_TYPE 0  // defines whether is it start (0) or finish (1) device
#define BOUNCE_PIN 27  // onboard button
#define LED_PIN 2      // oboard led

// uint8_t startDeviceAddress[] = {0x08, 0x3A, 0xF2, 0x3A, 0x82, 0x30};
uint8_t startDeviceAddress[] = {0x08, 0x3A, 0xF2, 0x3A, 0x81, 0x60};
uint8_t finishDeviceAddress[] = {0x08, 0x3A, 0xF2, 0x3A, 0x81, 0xFC};

Bounce bounce = Bounce();
int ledState = LOW;

typedef struct struct_message {
    byte state;
} struct_message;

struct_message readMessage;
struct_message sentMessage;

esp_now_peer_info_t peerInfo;

/**
 * Specifies whether is the start (master) device or not.
 * Usually based on harware configuration.
 */
boolean isStartDevice() {
    return DEVICE_TYPE == 0;
}

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    if (status == ESP_NOW_SEND_FAIL) {
        Log.errorln("Delivery failed");
    }
}

// Callback when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
    memcpy(&readMessage, incomingData, sizeof(readMessage));

    if (isStartDevice()) {
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState);
    } else {
        sentMessage.state = readMessage.state + 1;
        esp_err_t result = esp_now_send(startDeviceAddress, (uint8_t *)&sentMessage, sizeof(sentMessage));
    }
    Log.noticeln("Message received.");
}

uint8_t *getPeerAddress() {
    if (isStartDevice()) {
        return finishDeviceAddress;
    }
    return startDeviceAddress;
}

void setup() {
    Serial.begin(921600);

    // initialize logging
    while (!Serial && !Serial.available()) {
    }
    Log.begin(LOG_LEVEL_VERBOSE, &Serial);
    Log.setPrefix(printPrefix);
    Log.setShowLevel(false);
    Log.infoln("START");

    if (isStartDevice()) {
        bounce.attach(BOUNCE_PIN, INPUT_PULLUP);
        bounce.interval(10);

        pinMode(LED_PIN, OUTPUT);
        digitalWrite(LED_PIN, ledState);
    }

    WiFi.mode(WIFI_MODE_STA);
    if (esp_now_init() != ESP_OK) {
        Log.errorln("Error initializing ESP-NOW");
        return;
    }
    // Print MAC Address to Serial monitor
    Log.noticeln("MAC Address: %s", WiFi.macAddress().c_str());    

    esp_now_register_send_cb(OnDataSent);

    if (isStartDevice()) {
        memcpy(peerInfo.peer_addr, finishDeviceAddress, 6);
    } else {
        memcpy(peerInfo.peer_addr, startDeviceAddress, 6);
    }
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Log.errorln("Failed to add peer");
        return;
    }
    esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
    bounce.update();

    if (isStartDevice()) {
        if (bounce.changed()) {
            int deboucedInput = bounce.read();
            if (deboucedInput == LOW) {              
                Log.infoln("Sending data...");
                esp_err_t result = esp_now_send(finishDeviceAddress, (uint8_t *)&sentMessage, sizeof(sentMessage));
                if (result != ESP_OK) {
                    Log.errorln("Error sending the data");
                }
            }
        }
    }
}
