#pragma once

#include <inttypes.h>
#include <string.h>

#ifdef ARDUINO
#include <Arduino.h>
#define SERDE_DEFAULT_STREAM = Stream
#else
#define SERDE_DEFAULT_STREAM
#endif

static const uint8_t sSerdeHeaderMsb  = 0xf0;
static const uint8_t sSerdeHeaderLsb  = 0x0d;

template <size_t Size>
struct SerdePacket
{
public:
    uint8_t mHeaderMsb  = sSerdeHeaderMsb;
    uint8_t mHeaderLsb  = sSerdeHeaderLsb;
    uint8_t mData[Size] = { 0x00 };
    uint8_t mChecksum   = 0x00;

public:
    inline uint8_t generateChecksum() const;
    inline bool verifyChecksum() const;
};

// --

/**
 * Send and receive typed objects over a Stream.
 *
 * Serial data format is sizeof(T) + 3 bytes long, accounting for
 * detection header and checksum.
 *
 * ## Caveats
 *
 * This will only successfully send and receive POD (plain old data)
 * objects, everything that might be dynamically allocated will
 * not work. Strings can be sent/received if their length is static,
 * is known at compile-time and will not change across calls.
 *
 * Note that this will only work if both ends of the transmission
 * have the same endianness. Endianness adaptation happens within
 * T and cannot be handled by this adapter code.
 *
 * ## Usage
 *
 * ```
 * // Emitter board
 * #include <serde.h>
 *
 * struct SensorData
 * {
 *     float mTemperature;
 *     float mHumidity;
 *     float mLatitude;
 *     float mLongitude;
 *     unsigned long mTime;
 * };
 *
 * using SerdeTX = Serde<SensorData>;
 *
 * void setup()
 * {
 *     Serial1.begin(115200);
 * }
 *
 * void loop()
 * {
 *     SensorData data = getSensorData();
 *     data.mTime = millis();
 *     SerdeTX::send(data, Serial1);
 * }
 *
 * // Receiver board
 * #include <serde.h>
 *
 * struct SensorData
 * {
 *     float mTemperature;
 *     float mHumidity;
 *     float mLatitude;
 *     float mLongitude;
 *     unsigned long mTime;
 * };
 *
 * using SerdeRX = Serde<SensorData>;
 *
 * void setup()
 * {
 *     Serial1.begin(115200);
 * }
 *
 * void loop()
 * {
 *     SensorData data;
 *     if (SerdeRX::receive(Serial1, data))
 *     {
 *         handleSensor(data.mTemperature, data.mHumidity);
 *         handlePosition(data.mLatitude, data.mLongitude);
 *         logTime(data.mTime);
 *     }
 * }
 * ```
 */
template <
    typename T,     /// The type you want to send (struct or scalar)
    typename Stream SERDE_DEFAULT_STREAM
>
struct Serde
{
public:
    using Packet = SerdePacket<sizeof(T)>;
    using Callback = void (*)(const T&);

public: // TX
    static inline void send(const T &inObject, Stream &inStream);

public: // RX
    static inline void read(Stream& inStream, Callback inCallback);
    static inline bool receive(Stream &inStream, T &outObject);

private:
    static inline void pack(const T &inObject, Packet &outPacket);
    static inline bool unpack(Stream &inStream, Packet &outPacket);
};

// -----------------------------------------------------------------------------

template <size_t Size>
inline uint8_t SerdePacket<Size>::generateChecksum() const
{
    uint8_t x = 0x2a; // Initialization vector
    x ^= mHeaderMsb;
    x ^= mHeaderLsb;
    for (size_t i = 0; i < Size; ++i)
    {
        x ^= mData[i];
    }
    return x;
}

template <size_t Size>
inline bool SerdePacket<Size>::verifyChecksum() const
{
    const uint8_t computedChecksum = generateChecksum();
    return mChecksum == computedChecksum;
}

// -----------------------------------------------------------------------------

template <typename T, typename Stream>
inline void Serde<T, Stream>::send(const T &inObject, Stream &inStream)
{
    Packet packet;
    pack(inObject, packet);
    inStream.write(packet.mHeaderMsb);
    inStream.write(packet.mHeaderLsb);
    inStream.write(packet.mData, sizeof(T));
    inStream.write(packet.mChecksum);
}

// --

template <typename T, typename Stream>
inline void Serde<T, Stream>::read(Stream &inStream, Callback inCallback)
{
    T object;
    if (receive(inStream, object))
    {
        inCallback(object);
    }
}

template <typename T, typename Stream>
inline bool Serde<T, Stream>::receive(Stream &inStream, T &outObject)
{
    Packet packet;
    if (!unpack(inStream, packet))
    {
        return false;
    }
    T *const addr = &outObject;
    memcpy(addr, packet.mData, sizeof(T));
    return true;
}

// --

template <typename T, typename Stream>
inline void Serde<T, Stream>::pack(const T &inObject, Packet &outPacket)
{
    const T *const addr = &inObject;
    outPacket.mHeaderMsb = sSerdeHeaderMsb;
    outPacket.mHeaderLsb = sSerdeHeaderLsb;
    memcpy(outPacket.mData, addr, sizeof(T));
    outPacket.mChecksum = outPacket.generateChecksum();
}

template <typename T, typename Stream>
inline bool Serde<T, Stream>::unpack(Stream &inStream, Packet &outPacket)
{
    while (inStream.available() && inStream.peek() != sSerdeHeaderMsb)
    {
        inStream.read(); // Drop non-header bytes
    }
    if (size_t(inStream.available()) < sizeof(Packet))
    {
        return false; // Not enough data
    }

    // At this point, the first byte in the buffer is sSerdeHeaderMsb
    // and there is enough data to try to recompose a message.

    // Verify header
    outPacket.mHeaderMsb = inStream.read();
    if (inStream.peek() != sSerdeHeaderLsb)
    {
        // Don't drop it as it could be a header MSB.
        // Abort now and try next time.
        return false;
    }
    outPacket.mHeaderLsb = inStream.read();

    // Read data
    inStream.readBytes(outPacket.mData, sizeof(T));

    // Read checksum and verify
    outPacket.mChecksum = inStream.read();
    return outPacket.verifyChecksum();
}
