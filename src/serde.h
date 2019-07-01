#pragma once

#include <inttypes.h>
#include <string.h>

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
 * struct Foo {
 *   char mName[16];
 *   byte mAge;
 * };
 *
 * using FooSerde = Serde<Foo, HardwareSerial>;
 *
 * Foo fooTx;
 * // Strings are tricky: set the buffer first to all zeros,
 * // then memcpy your desired variable-length string into the buffer.
 * memset(fooTx.mName, 0, 16);
 * memcpy(fooTx.mName, "John Doe", 8);
 * fooTx.mAge = 42;
 *
 * FooSerde::send(fooTx, Serial);
 *
 * Foo fooRx;
 * if (FooSerde::receive(Serial, fooRx))
 * {
 *   fooRx.mName;
 *   fooRx.mAge;
 * }
 * ```
 */
template <typename T, typename Stream>
struct Serde
{
public:
  using Packet = SerdePacket<sizeof(T)>;

public: // TX
  static inline void send(const T &inObject, Stream &inStream);

public: // RX
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
  while (inStream.available() && inStream.peek() != sSerdeHeaderMsb) {
    inStream.read(); // Drop non-header bytes
  }
  if (size_t(inStream.available()) < sizeof(Packet)) {
    return false; // Not enough data
  }

  // At this point, the first byte in the buffer is sSerdeHeaderMsb
  // and there is enough data to try to recompose a message.

  // Verify header
  outPacket.mHeaderMsb = inStream.read();
  if (outPacket.mHeaderMsb != sSerdeHeaderMsb)
  {
    // Technically this was covered earlier and should not happen.
    return false;
  }
  if (inStream.peek() != sSerdeHeaderLsb)
  {
    // Don't drop it as it could be a header MSB.
    // Abort now and try next time.
    // Serial.println("Second byte is not LSB, aborting");
    return false;
  }
  outPacket.mHeaderLsb = inStream.read();

  // Read data
  const size_t readBytes = inStream.readBytes(outPacket.mData, sizeof(T));
  if (readBytes != sizeof(T))
  {
    // Mismatching read size
    return false;
  }

  // Read checksum and verify
  outPacket.mChecksum = inStream.read();
  return outPacket.verifyChecksum();
}
