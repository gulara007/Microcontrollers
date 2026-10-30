// Lab 5 (Task 5) - Board B: BLE scanner, RSSI to the STM32
// Board: ESP32-S3-DevKitC-1, Arduino IDE, "ESP32S3 Dev Module"
//
// Scans continuously. For every advertisement from "LAB5-A" it sends one
// frame with the RSSI to the NUCLEO-F401RE on UART1, and prints the RSSI
// on the USB serial port for debugging.
//
// Wiring (UART1, 115200 8N1):
//   ESP32 GPIO18 (RX)  <-  Nucleo D8 / PA9  (USART1_TX)
//   ESP32 GPIO17 (TX)  ->  Nucleo D2 / PA10 (USART1_RX)
//   ESP32 GND          --  Nucleo GND
//
// Frame (4 bytes):  0xAA | LEN = 1 | RSSI (int8) | CHK = LEN XOR RSSI

#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

const char *TARGET_NAME = "LAB5-A";
const int UART1_RX_PIN = 18;
const int UART1_TX_PIN = 17;

void sendFrame(int8_t rssi) {
  uint8_t len = 1;
  uint8_t data = (uint8_t)rssi;
  uint8_t frame[4] = {0xAA, len, data, (uint8_t)(len ^ data)};
  Serial1.write(frame, sizeof(frame));
}

class ScanCallback : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice dev) {
    if (dev.haveName() && dev.getName() == TARGET_NAME) {
      int rssi = dev.getRSSI();
      sendFrame((int8_t)rssi);
      Serial.println(rssi);
    }
  }
};

BLEScan *scan;

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, UART1_RX_PIN, UART1_TX_PIN);  // RX first

  BLEDevice::init("");
  scan = BLEDevice::getScan();
  scan->setAdvertisedDeviceCallbacks(new ScanCallback(), true);    // keep duplicates
  scan->setActiveScan(true);
  scan->setInterval(100);
  scan->setWindow(99);
}

void loop() {
  scan->start(1, false);   // scan for 1 s
  scan->clearResults();    // free memory, keep reporting duplicates
}
