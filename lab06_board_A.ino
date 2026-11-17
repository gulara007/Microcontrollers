// Lab 5 (Task 5) - Board A: BLE advertiser
// Board: ESP32-S3-DevKitC-1, Arduino IDE, "ESP32S3 Dev Module"
// Advertises as "LAB5-A". After RST the power is -24 dBm;
// each BOOT press raises it one 3 dB step, up to +18 dBm (then back to -24).
// 8 presses after RST = 0 dBm.

#include <BLEDevice.h>
#include "esp_bt.h"

esp_power_level_t lv[] = {ESP_PWR_LVL_N24, ESP_PWR_LVL_N21, ESP_PWR_LVL_N18, ESP_PWR_LVL_N15,
  ESP_PWR_LVL_N12, ESP_PWR_LVL_N9, ESP_PWR_LVL_N6, ESP_PWR_LVL_N3, ESP_PWR_LVL_N0,
  ESP_PWR_LVL_P3, ESP_PWR_LVL_P6, ESP_PWR_LVL_P9, ESP_PWR_LVL_P12, ESP_PWR_LVL_P15,
  ESP_PWR_LVL_P18};
int i = 0;                               // after RST: -24 dBm

void setup() {
  pinMode(0, INPUT_PULLUP);              // BOOT button
  BLEDevice::init("LAB5-A");
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, lv[i]);
  BLEDevice::getAdvertising()->setScanResponse(true);
  BLEDevice::startAdvertising();
}

void loop() {
  if (digitalRead(0) == LOW) {           // button pressed
    delay(30);                           // let the contact settle
    if (digitalRead(0) == LOW) {         // still pressed: a real press
      i = (i + 1) % 15;
      esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, lv[i]);
      while (digitalRead(0) == LOW) { }  // wait for release
      delay(100);                        // ignore bounce on release
    }
  }
}
