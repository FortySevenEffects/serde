# 📡 Serde

[![MIT License](https://img.shields.io/github/license/FortySevenEffects/serde.svg?color=blue)](https://github.com/FortySevenEffects/serde/blob/master/LICENSE)
![GitHub tag (latest SemVer)](https://img.shields.io/github/tag/FortySevenEffects/serde.svg?color=blue&label=release)
[![Travis CI Build](https://img.shields.io/travis/com/FortySevenEffects/serde.svg)](https://travis-ci.com/FortySevenEffects/serde)
[![Coverage Status](https://coveralls.io/repos/github/FortySevenEffects/serde/badge.svg?branch=master)](https://coveralls.io/github/FortySevenEffects/serde?branch=master)

Exchange structured data between Arduino boards.

## Features

- 💪 Cross-device strong data typing
- 📦 Send / receive either structs or scalar types
- 🔌 Use any Stream-based interface (HardwareSerial, SoftwareSerial, I2C, SPI...)
- 🔒 Internal checksum for data integrity verification
- ↔️ Receive and send different types on the same stream
  ([example](./examples/DifferentTypesForTXandRX/DifferentTypesForTXandRX.ino))
- ⚡ Advanced command processing with [Commander](#commander)

## Install

Use the Arduino Library Manager to install the library.

## Usage

1. Define [shared data](#sharing-data) (using a weather station as an example):

```cpp
// libraries/weather-shared/src/sensor-data.h
#pragma once

struct SensorData
{
    float temperature;
    float humidity;
    float latitude;
    float longitude;
    unsigned long time;
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
    data.time = millis();
    SerdeTX::send(data, Serial1);
}
```

3. Receive it on another device:

```cpp
#include <serde.h>
#include <sensor-data.h>

using SerdeRX = Serde<SensorData>;

// This is called when new data is available
void recordSensorData(const SensorData& data)
{
    recordWeather(data.temperature, data.humidity);
    recordPosition(data.latitude, data.longitude);
    logTime(data.time);
}

void setup()
{
    Serial1.begin(115200);
}

void loop()
{
    // Pass it the serial port to read from and a callback:
    SerdeRX::read(Serial1, recordSensorData);

    // Or you can also check manually for incoming data:
    // SensorData data;
    // if (SerdeRX::receive(Serial1, data))
    // {
    //     recordSensorData(data);
    // }
}
```

## Sharing data

This library requires both emitter and receiver to use the same data types.

Copying the definitions back and forth is cumbersome and will lead to errors,
so sharing the type definitions between sketches is essential.

The best way to share files between sketches is to create a library.

1. Go to the Arduino libraries directory:

- `~/Documents/Arduino/libraries` on macOS and Linux
- `~\Documents\Arduino\libraries` on Windows

2. Create a new directory, name it as you wish, for example: `weather-shared`
3. Create a subdirectory `src` in `weather-shared`
4. Place your definitions in a header file in `src`, like `weather-shared/src/sensor-data.h`
5. Create a `library.properties` file in `weather-shared`:

```ini
name=WeatherShared      # This name is only used for published libraries
includes=sensor-data.h  # The name of the file where you placed your definitions

# The rest is required by Arduino:
version=0.0.1
author=
maintainer=
sentence=
paragraph=
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
    char text[32] = { 0 };
};

Message message;
memset(message.text, 0, sizeof(Message::text));  // clear
memcpy(message.text, "Hello, World !", 14);      // copy

Serde<Message>::send(message, Serial);
```

There is nothing particular to do after reception, just use it:

```cpp
Message message;
if (Serde<Message>::receive(Serial1, message))
{
    Serial.println(message.text);
}
```

## Threading considerations

- When calling `receive`, you can be sure that the object passed will
  never have been changed if `receive` returns false.
- Since Arduino is single-threaded, there should be no race conditions
  when `receive` returns true, however interrupts can be seen as threads,
  so if using the received value in an interrupt handler, be sure to know
  that it could be in the middle of an update.

## Commander

Commander is a [Remote Procedure Call](https://en.wikipedia.org/wiki/Remote_procedure_call)
layer built on top of Serde to handle multiple types of messages
(commands) being sent and received, with different attributes:

```cpp
#include <serde-commander.h>

// 1. Define a structure for each of
// the commands you want to receive:
struct SayHello
{
    char name[32];
};

struct SetPinState
{
    byte pinNumber;
    bool state;
};

// 2. Create a CommanderRX interface and
// list the commands it will handle:
SERDE_COMMANDER_CREATE_RX(CommanderRX,
    SayHello,
    SetPinState
);

// 3. Create a function for each command
// with the following signature (required):
// void on{CommandName}Received(const CommandName&)
// It will be called automatically when the
// corresponding command is received.
void onSayHelloReceived(const SayHello& data)
{
    Serial.print("Hello, ");
    Serial.println(data.name);
}

void onSetPinStateReceived(const SetPinState& data)
{
    digitalWrite(data.pinNumber, data.state ? HIGH : LOW);
}

// --

void setup()
{
    Serial.begin(115200);
    Serial1.begin(115200);
}

void loop()
{
    // Just pass it where to read from:
    CommanderRX::read(Serial1);
}
```

To send commands:

```cpp
#include <serde-commander.h>

// Those structs would be defined in a shared file
struct SayHello
{
    char name[32];
};

struct SetPinState
{
    byte pinNumber;
    bool state;
};

// 2. Create a CommanderTX interface and
// list the commands it may send:
SERDE_COMMANDER_CREATE_TX(CommanderTX,
    SayHello,
    SetPinState
);

// No need to define callbacks for Commander TX
// They are only required if you want to receive
// commands (RX-only or bidirectional).

// --

void setup()
{
    Serial1.begin(115200);
}

void loop()
{
    // Create commands and send them:
    SayHello hello;
    memset(hello.name, 0, sizeof(SayHello::name));
    memcpy(hello.name, "Commander", 9);
    CommanderTX::send(hello, Serial1);

    SetPinState pinState;
    pinState.pinNumber = 13;
    pinState.state = true;
    CommanderTX::send(pinState, Serial1);
    delay(100);
    pinState.state = false;
    CommanderTX::send(pinState, Serial1);
    delay(100);
}
```

There are 3 Commander creation macros:

- `SERDE_COMMANDER_CREATE_TX`: Send-only (no need to implement the callbacks
  for this one).
- `SERDE_COMMANDER_CREATE_RX`: Receive-only (callbacks required)
- `SERDE_COMMANDER_CREATE`: Both send and receive the same commands
  (callbacks required)

Checkout the [Commander example](./examples/Commander/Commander.ino)
for more details.

### Commander caveats and limitations

Commander currently supports up to 8 commands. If you need more, please
open a PR on
[`src/serde-macros.h`](./src/serde-macros.h).

Also please note that a Commander message will not be compatible with a
plain Serde message (and vice-versa).

You can have one side of the communication handled by Commander, and the
"reply" handled by Serde (as done in the
[CommanderCalculator](./examples/CommanderCalculator/CommanderCalculator.ino)
example), as long as both devices use the right types:

```
   Device A      Device B
CommanderTX  ->  CommanderRX
    SerdeRX  <-  SerdeTX
```

But this won't work:

```
CommanderTX  ->  SerdeRX
    SerdeTX  ->  CommanderRX
```

## License & Aknowledgements

Inspired from Rust's awesome crate
[serde](https://crates.io/crates/serde). 🦀

[MIT](https://github.com/47ng/typescript-library-starter/blob/master/LICENSE) - Made with ❤️ by [François Best](https://francoisbest.com).
