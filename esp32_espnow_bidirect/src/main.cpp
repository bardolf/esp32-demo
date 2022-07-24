// 08:3A:F2:3A:82:30
// 08:3A:F2:3A:81:FC

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

#define DEVICE_TYPE 0  // defines whether is it start (0) or finish (1) device

uint8_t startDeviceAddress[] = {0x08, 0x3A, 0xF2, 0x3A, 0x82, 0x30};
uint8_t finishDeviceAddress[] = {0x08, 0x3A, 0xF2, 0x3A, 0x81, 0xFC};

// Variable to store if sending data was successful
String success;

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
        success = "Delivery Fail :(";
    }
}

// Callback when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
    memcpy(&readMessage, incomingData, sizeof(readMessage));

    if (!isStartDevice()) {
        sentMessage.state = readMessage.state + 1;
        esp_err_t result = esp_now_send(startDeviceAddress, (uint8_t *)&sentMessage, sizeof(sentMessage));
    }

    Serial.print(millis());
    Serial.print(" State: ");
    Serial.println(readMessage.state);
}

uint8_t *getPeerAddress() {
    if (isStartDevice()) {
        return finishDeviceAddress;
    }
    return startDeviceAddress;
}

void setup() {
    Serial.begin(921600);
    // Put ESP32 into Station mode
    WiFi.mode(WIFI_MODE_STA);
    // Init ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // Print MAC Address to Serial monitor
    Serial.print("MAC Address: ");
    Serial.println(WiFi.macAddress());

    // Once ESPNow is successfully Init, we will register for Send CB to
    // get the status of Trasnmitted packet
    esp_now_register_send_cb(OnDataSent);

    // Register peer
    if (isStartDevice()) {
        memcpy(peerInfo.peer_addr, finishDeviceAddress, 6);
    } else {
        memcpy(peerInfo.peer_addr, startDeviceAddress, 6);
    }
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    // Add peer
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
    }
    // Register for a callback function that will be called when data is received
    esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
    // Send message via ESP-NOW
    sentMessage.state = 1;

    if (isStartDevice()) {
        Serial.print(millis());
        Serial.println(" Sending data...");
        esp_err_t result = esp_now_send(finishDeviceAddress, (uint8_t *)&sentMessage, sizeof(sentMessage));
        if (result == ESP_OK) {
            // Serial.println("Sent with success");
        } else {
            Serial.println("Error sending the data");
        }
    }
    delay(5000);
}
