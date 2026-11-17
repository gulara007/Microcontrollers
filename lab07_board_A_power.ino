// ================= Lab 6B — Board A for current measurements =================
// Board A advertises on its own (no STM32, no USB, no UART wires) so the
// current through the 1 ohm shunt is board A only.
//
// For each capture change the three settings below, Upload (with USB),
// then unplug USB and power from the bench supply through the shunt.
//
//   Capture  INTERVAL   TX_POWER          PAYLOAD_BYTES
//     1        160      ESP_PWR_LVL_N0         0         100 ms, 0 dBm, empty
//     2       1600      ESP_PWR_LVL_N0         0         1 s,    0 dBm, empty
//     3        160      ESP_PWR_LVL_P9         0         100 ms, +9 dBm, empty
//     4        160      ESP_PWR_LVL_N0        25         100 ms, 0 dBm, 25 bytes
//     5       1600      ESP_PWR_LVL_P9        25         1 s,   +9 dBm, 25 bytes
//     6        160      ESP_PWR_LVL_P9        25         100 ms, +9 dBm, 25 bytes
//
// PAYLOAD_BYTES = total advertising data on air:
//   0  -> "empty": only the 3-byte Flags structure, no manufacturer data
//   25 -> 02 01 06 | 16 FF FF FF + 18 data bytes   (3 + 22 = 25 bytes)
// =============================================================================

#include <BLEDevice.h>
#include <BLEAdvertising.h>
#include "esp_bt.h"

#define INTERVAL       160              // x 0.625 ms : 160 = 100 ms, 1600 = 1 s
#define TX_POWER       ESP_PWR_LVL_N0   // ESP_PWR_LVL_N0 = 0 dBm, ESP_PWR_LVL_P9 = +9 dBm
#define PAYLOAD_BYTES  0                // 0 or 25

#if ESP_ARDUINO_VERSION_MAJOR >= 3
typedef String BleStr;
#else
typedef std::string BleStr;
#endif

void setup() {
  Serial.begin(115200);               // only for the USB test; harmless without USB
  delay(200);
  Serial.println("board_A_power start");

  BLEDevice::init("");
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, TX_POWER);

  BLEAdvertising *adv = BLEDevice::getAdvertising();
  adv->setAdvertisementType(ADV_TYPE_NONCONN_IND);
  adv->setMinInterval(INTERVAL);
  adv->setMaxInterval(INTERVAL);
  adv->setScanResponse(false);

  BLEAdvertisementData ad;
#if PAYLOAD_BYTES == 0
  ad.setFlags(0x06);                  // "empty": flags only (3 bytes), no data
#elif PAYLOAD_BYTES == 25
  uint8_t md[20];                     // company ID (2) + 18 data bytes
  md[0] = 0xFF; md[1] = 0xFF;
  for (int i = 2; i < 20; i++) md[i] = i;
  ad.setFlags(0x06);                                // 3 bytes
  ad.setManufacturerData(BleStr((char *)md, 20));   // 2 + 20 = 22 bytes
#endif
  adv->setAdvertisementData(ad);
  adv->start();
}

void loop() {
  delay(1000);
  Serial.printf("alive %lu s\n", millis() / 1000);
}
