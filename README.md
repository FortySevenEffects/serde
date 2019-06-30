# 📡 Serde

Send / receive structured data across Streams on Arduino.

## Install

Use the Arduino Library Manager to install the library.

## Usage

```cpp
#include <serde.h>

// 1. Define some data structure to share
// across the wire (Serial, I2C, SPI..):
struct Data {
  int mCounter;
  unsigned long mTime;
};

Data sDataTx = { 0, 0.f };

// 2. Define the Serualizer / Deserializer :
using SerdeData = Serde<Data, HardwareSerial>;

void setup() {
  // Hardware setup: bridge Serial1 TX and RX
  // to create a loopback interface

  Serial.begin(115200);   // For serial debugging
  Serial1.begin(115200);  // Hardware loopback
}

void loop() {
  // Send data
  SerdeData::send(sDataTx, Serial1);

  // Receive data
  Data dataRx;
  while (!SerdeData::canReceive(Serial1))
  {
    // Wait for entire struct to be available
    delay(10);
  }

  if (SerdeData::receive(Serial1, dataRx))
  {
    Serial.print("Count: ");
    Serial.println(dataRx.mCounter);
    Serial.print("Time:  ");
    Serial.println(dataRx.mTime);

    // Update send data
    sDataTx.mCounter++;
    sDataTx.mTime = millis() - sDataTx.mTime;
  }
  else
  {
    // Reception can fail due to data corruption
    Serial.println("Transmission error");
  }
}
```

## License & Aknowledgements

Inspired from Rust's awesome crate [serde](https://crates.io/crates/serde). 🦀

[MIT](https://github.com/47ng/typescript-library-starter/blob/master/LICENSE) - Made with ❤️ by [François Best](https://francoisbest.com).
