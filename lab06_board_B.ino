#include <BLEDevice.h>
#include <BLEScan.h>

void sendFrame(int8_t rssi) {
  uint8_t f[4] = {0xAA, 1, (uint8_t)rssi, (uint8_t)(1 ^ (uint8_t)rssi)};
  Serial1.write(f, 4);
}

class CB : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice d) {
    if (d.haveName() && d.getName() == "LAB5-A") {
      sendFrame(d.getRSSI());
      Serial.println(d.getRSSI());         
    }
  }
};

BLEScan *scan;
void setup() {
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, 18, 17);   
  BLEDevice::init("");
  scan = BLEDevice::getScan();
  scan->setAdvertisedDeviceCallbacks(new CB(), true);  
  scan->setActiveScan(true);
  scan->setInterval(100); scan->setWindow(99);
}
void loop() { scan->start(1, false); scan->clearResults(); }
