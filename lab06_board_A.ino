// Lab 5 (Task 5) - Board A: BLE advertiser
// Board: ESP32-S3-DevKitC-1, Arduino IDE, "ESP32S3 Dev Module"
//
// Advertises under the name "LAB5-A". The BOOT button (GPIO0) steps the
// advertising transmit power through 15 levels, -24 to +18 dBm in 3 dB steps.
// After reset the power is -24 dBm; press BOOT 8 times for 0 dBm.
// The current level is also printed on the USB serial port (115200) if connected.

#include <BLEDevice.h>
#include "esp_bt.h"

const esp_power_level_t lv[] = {
  ESP_PWR_LVL_N24, ESP_PWR_LVL_N21, ESP_PWR_LVL_N18, ESP_PWR_LVL_N15,
  ESP_PWR_LVL_N12, ESP_PWR_LVL_N9,  ESP_PWR_LVL_N6,  ESP_PWR_LVL_N3,
  ESP_PWR_LVL_N0,  ESP_PWR_LVL_P3,  ESP_PWR_LVL_P6,  ESP_PWR_LVL_P9,
  ESP_PWR_LVL_P12, ESP_PWR_LVL_P15, ESP_PWR_LVL_P18
};
const int dbm[] = {-24, -21, -18, -15, -12, -9, -6, -3, 0, 3, 6, 9, 12, 15, 18};
const int N_LEVELS = sizeof(dbm) / sizeof(dbm[0]);

const int BOOT_PIN = 0;
int level = 0;                          // index into lv[]: 0 = -24 dBm

void applyLevel() {
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, lv[level]);
  Serial.printf("TX power = %d dBm (level %d)\n", dbm[level], level);
}

void setup() {
  Serial.begin(115200);
  pinMode(BOOT_PIN, INPUT_PULLUP);

  BLEDevice::init("LAB5-A");
  BLEAdvertising *adv = BLEDevice::getAdvertising();
  adv->setScanResponse(true);           // name also sent in the scan response
  BLEDevice::startAdvertising();

  applyLevel();
}

void loop() {
  static bool wasHigh = true;
  bool isHigh = digitalRead(BOOT_PIN) == HIGH;

  if (wasHigh && !isHigh) {             // falling edge: button pressed
    level = (level + 1) % N_LEVELS;
    applyLevel();
    delay(300);                         // debounce
  }
  wasHigh = isHigh;
}
