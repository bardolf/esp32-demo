#include <Arduino.h>
#include <Bounce2.h>
#include <FastLED.h>
#include <WiFi.h>
#include <Wire.h>
#include <esp_now.h>
#include <esp_wifi.h>

#include "logging.h"

#define DEVICE_TYPE 1   // defines whether is it start (0) or finish (1) device
#define BOUNCE_PIN 0   // onboard button
#define LED_PIN 2       // onboard led
#define RDG_DATA_PIN 5  // onboard rgb led

typedef struct Message {
    char text[8];
    unsigned long time;
    byte retry;
} Message;

// settings
static const int sendQueueLen = 2;

<<<<<<< Updated upstream
uint8_t startDeviceAddress[] = {0x08, 0x3A, 0xF2, 0x3A, 0x81, 0xFC};   // white
uint8_t finishDeviceAddress[] = {0x08, 0x3A, 0xF2, 0x3A, 0x81, 0x60};  // red
=======
// uint8_t startDeviceAddress[] = {0x08, 0x3A, 0xF2, 0x3A, 0x82, 0x30};
uint8_t startDeviceAddress[] = {0x08, 0x3A, 0xF2, 0x3A, 0x81, 0xFC};
// uint8_t finishDeviceAddress[] = {0x08, 0x3A, 0xF2, 0x3A, 0x81, 0x60};
uint8_t finishDeviceAddress[] = {0x08, 0x3A, 0xF2, 0x3A, 0x5D, 0xA0};
>>>>>>> Stashed changes

Bounce bounce = Bounce();
int ledState = LOW;
CRGB leds[1];
static QueueHandle_t sendQueue;

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
        leds[0] = CRGB::Red;
        FastLED.show();
    }
}

// Callback when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
    Message readMessage;
    memcpy(&readMessage, incomingData, sizeof(readMessage));
    Log.noticeln("Received message %s - %d - %d", readMessage.text, readMessage.retry, readMessage.time);

    if (isStartDevice()) {
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState);
    } else {
        strcpy(readMessage.text, "BYE\0");
        esp_err_t result = esp_now_send(startDeviceAddress, (uint8_t *)&readMessage, sizeof(readMessage));
    }
}

uint8_t *getPeerAddress() {
    if (isStartDevice()) {
        return finishDeviceAddress;
    }
    return startDeviceAddress;
}

// void blinkTask(void *pvParameters) {
//     while (1) {
//         digitalWrite(LED_PIN, HIGH);
//         vTaskDelay(500 / portTICK_PERIOD_MS);
//         digitalWrite(LED_PIN, LOW);
//         vTaskDelay(233 / portTICK_PERIOD_MS);
//     }
// }

void readButtonTask(void *pvParameters) {
    while (1) {
        vTaskDelay(50 / portTICK_PERIOD_MS);
        bounce.update();
        if (bounce.changed()) {
            int deboucedInput = bounce.read();
            if (deboucedInput == LOW) {
                FastLED.clear(true);
                Message message;
                strcpy(message.text, "HELLO!\0");
                message.retry = 0;
                message.time = 912345678ul;
                if (xQueueSend(sendQueue, &message, 0) != pdTRUE) {
                    Log.errorln("Problem while putting a message to a queue, queue is full?");
                }
            }
        }
    }
}

void sendTask(void *pvParameters) {
    Message message;
    while (1) {
        if (xQueueReceive(sendQueue, &message, 10000) == pdTRUE) {
            Log.infoln("Sending data... %s", message.text);
            esp_err_t result = esp_now_send(finishDeviceAddress, (uint8_t *)&message, sizeof(message));
            if (result != ESP_OK) {
                Log.errorln("Error sending the data");
            }
        }
    }
}

void setup() {
    Serial.begin(115200);

    // initialize logging
    while (!Serial && !Serial.available()) {
    }
    Log.begin(LOG_LEVEL_VERBOSE, &Serial);
    Log.setPrefix(printPrefix);
    Log.setShowLevel(false);
    Log.infoln("START");

    // init rgb led
    FastLED.addLeds<NEOPIXEL, RDG_DATA_PIN>(leds, 1);

    if (isStartDevice()) {
        bounce.attach(BOUNCE_PIN, INPUT_PULLUP);
        bounce.interval(10);

        pinMode(LED_PIN, OUTPUT);
        digitalWrite(LED_PIN, ledState);
    }

    WiFi.mode(WIFI_MODE_STA);
    esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_LR);
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

    sendQueue = xQueueCreate(sendQueueLen, sizeof(Message));

    // xTaskCreatePinnedToCore(blinkTask, "Blink Task", 1024, NULL, 3, NULL, ARDUINO_RUNNING_CORE);
    if (isStartDevice()) {
        xTaskCreatePinnedToCore(readButtonTask, "Read Button Task", 2024, NULL, 3, NULL, ARDUINO_RUNNING_CORE);
        xTaskCreatePinnedToCore(sendTask, "Send Task", 2024, NULL, 3, NULL, ARDUINO_RUNNING_CORE);
    }
}

void loop() {
    // ideally do nothing
    vTaskDelay(10000 / portTICK_PERIOD_MS);
}
