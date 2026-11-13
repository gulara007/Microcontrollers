#include <BLEDevice.h>
#include "esp_bt.h"

esp_power_level_t lv[] = {ESP_PWR_LVL_N24, ESP_PWR_LVL_N21, ESP_PWR_LVL_N18, ESP_PWR_LVL_N15,
  ESP_PWR_LVL_N12, ESP_PWR_LVL_N9, ESP_PWR_LVL_N6, ESP_PWR_LVL_N3, ESP_PWR_LVL_N0,
  ESP_PWR_LVL_P3, ESP_PWR_LVL_P6, ESP_PWR_LVL_P9, ESP_PWR_LVL_P12, ESP_PWR_LVL_P15,
  ESP_PWR_LVL_P18};
int i = 0;                               

void setup() {
  pinMode(0, INPUT_PULLUP);              
  BLEDevice::init("LAB5-A");
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, lv[i]);
  BLEDevice::getAdvertising()->setScanResponse(true);
  BLEDevice::startAdvertising();
}

void loop() {
  if (digitalRead(0) == LOW) {           
    delay(30);                           
    if (digitalRead(0) == LOW) {        
      i = (i + 1) % 15;
      esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, lv[i]);
      while (digitalRead(0) == LOW) { }  
      delay(100);                       
    }
  }
}
