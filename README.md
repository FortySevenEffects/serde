# 📡 Serde

[![MIT License](https://img.shields.io/github/license/FortySevenEffects/serde.svg?color=blue)](https://github.com/FortySevenEffects/serde/blob/master/LICENSE)
![GitHub tag (latest SemVer)](https://img.shields.io/github/tag/FortySevenEffects/serde.svg?color=blue&label=release)
[![Travis CI Build](https://img.shields.io/travis/com/FortySevenEffects/serde.svg)](https://travis-ci.com/FortySevenEffects/serde)
[![Average issue resolution time](https://isitmaintained.com/badge/resolution/FortySevenEffects/serde.svg)](https://isitmaintained.com/project/FortySevenEffects/serde)
[![Number of open issues](https://isitmaintained.com/badge/open/FortySevenEffects/serde.svg)](https://isitmaintained.com/project/FortySevenEffects/serde)

Exchange structured data between Arduino boards.

## Features

- 💪 Cross-device strong data typing
- 📦 Send / receive either structs or scalar types
- 🔌 Use any Stream-based interface (HardwareSerial, SoftwareSerial, I2C, SPI...)
- 🔒 Internal checksum for data integrity verification
- ↔️ Receive and send different types on the same stream
  ([example](./examples/DifferentTypesForTXandRX/DifferentTypesForTXandRX.ino))

## Install

Use the Arduino Library Manager to install the library.

## Usage

1. Define [shared data](#sharing-data) (using a weather station as an example):

```cpp
// libraries/weather-shared/src/sensor-data.h
#pragma once

struct SensorData
{
    float mTemperature;
    float mHumidity;
    float mLatitude;
    float mLongitude;
    unsigned long mTime;
};
```

2. Send it from one device:

```cpp
#include <serde.h>
#include <sensor-data.h>

using SerdeTX = Serde<SensorData>;

void setup()
{
    Serial1.begin(115200);
}

void loop()
{
    SensorData data = getSensorData();
    data.mTime = millis();
    SerdeTX::send(data, Serial1);
}
```

3. Receive it on another device:

```cpp
#include <serde.h>
#include <sensor-data.h>

using SerdeRX = Serde<SensorData>;

void setup()
{
    Serial1.begin(115200);
}

void loop()
{
    SensorData data;
    if (SerdeRX::receive(Serial1, data))
    {
        recordWeather(data.mTemperature, data.mHumidity);
        recordPosition(data.mLatitude, data.mLongitude);
        logTime(data.mTime);
    }
}
```

## Sharing data

This library requires both emitter and receiver to use the same data types.

Copying the definitions back and forth is cumbersome and will lead to errors,
so sharing the type definitions between sketches is essential.

The best way to share files between sketches is to create a library.

1. Go to the Arduino libraries directory

- ~/Documents/Arduino/libraries on macOS and Linux
- ~\Documents\Arduino\libraries on Windows

2. Create a new directory, name it as you wish, for example: `weather-shared`
3. Create a subdirectory `src` in `weather-shared`
4. Place your definitions in a header file in `src`, like `weather-shared/src/sensor-data.h`
5. Create a `library.properties` file under `weather-shared`:

```ini
name=WeatherShared      # This name is only used for published libraries
includes=sensor-data.h  # The name of the file where you placed your definitions

# The rest is required by Arduino:
version=0.0.1
author=
maintainer=
category=Uncategorized
architectures=*
```

You can now include your definitions as such:

```cpp
#include <serde.h>
#include <sensor-data.h>

using SerdeTX = Serde<SensorData>;
```

## Caveats

- Both sending and receiving devices must have the same
  [endianness](https://en.wikipedia.org/wiki/Endianness).
- You can only exchange [Plain Old Data (POD)](https://stackoverflow.com/questions/146452/what-are-pod-types-in-c)
  objects, nothing allocated or which size is unknown at compile time.

## Sending / Receiving strings

Because strings are of an arbitrary, runtime-defined length, you will have
to send a `char` buffer of a fixed length, capable of containing the
largest string you need (size it appropriately, as all messages will be
this big), plus one byte for
[null-termination](https://en.wikipedia.org/wiki/Null-terminated_string).

To send a string:

```cpp
struct Message
{
    // Can hold at maximum 31 characters + 1 null terminator
    char mText[32] = { 0 };
};

Message message;
memset(message.mText, 0, sizeof(Message::mText)); // clear
memcpy(message.mText, "Hello, World !", 14);      // copy

Serde<Message>::send(message, Serial);
```

There is nothing particular to do after reception, just use it:

```cpp
Message message;
if (Serde<Message>::receive(Serial1, message))
{
    Serial.println(message.mText);
}
```

## Threading considerations

- When calling `receive`, you can be sure that the object passed will
  never have been changed if `receive` returns false.
- Since Arduino is single-threaded, there should be no race conditions
  when `receive` returns true, however interrupts can be seen as threads,
  so if using the received value in an interrupt handler, be sure to know
  that it could be in the middle of an update.

## License & Aknowledgements

Inspired from Rust's awesome crate [serde](https://crates.io/crates/serde). 🦀

[MIT](https://github.com/47ng/typescript-library-starter/blob/master/LICENSE) - Made with ❤️ by [François Best](https://francoisbest.com).
