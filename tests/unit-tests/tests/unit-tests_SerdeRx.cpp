#include "unit-tests.h"
#include <serde.h>
#include <tests/mocks/test-mocks_SerialMock.h>

BEGIN_UNNAMED_NAMESPACE

using namespace testing;
using namespace TEST_MOCKS_NAMESPACE;

TEST(SerdeRx, shouldDropNonHeaderBytes)
{
    using Stream = SerialMock<16>;
    using SerdeRX = Serde<uint8, Stream>;
    uint8 data;
    const uint8 input[8] = {
        0, 1, 2, 3, 4, 5, 6, 7
    };
    Stream stream;
    stream.mRxBuffer.write(input, 8);
    const bool success = SerdeRX::receive(stream, data);
    EXPECT_FALSE(success);
    EXPECT_TRUE(stream.mRxBuffer.isEmpty());
}

TEST(SerdeRx, shouldDropNonHeaderBytesUntilOneIsFound)
{
    using Stream = SerialMock<16>;
    using SerdeRX = Serde<uint8, Stream>;
    uint8 data;
    const uint8 input[5] = {
        0, 1, 2, 3, sSerdeHeaderMsb
    };
    Stream stream;
    stream.mRxBuffer.write(input, 5);
    const bool success = SerdeRX::receive(stream, data);
    EXPECT_FALSE(success);
    EXPECT_EQ(stream.mRxBuffer.getLength(), 1);
}

TEST(SerdeRx, shouldReturnFalseIfHeaderIsNotComplete)
{
    using Stream = SerialMock<16>;
    using SerdeRX = Serde<uint8, Stream>;
    uint8 data;
    const uint8 input[8] = {
        sSerdeHeaderMsb, 1, 2, 3, 4, 5, 6, 7
    };
    Stream stream;
    stream.mRxBuffer.write(input, 8);
    const bool success = SerdeRX::receive(stream, data);
    EXPECT_FALSE(success);
    EXPECT_EQ(stream.mRxBuffer.getLength(), 7) << "It only consumes the header MSB";
    EXPECT_EQ(stream.mRxBuffer.peek(), 1) << "The next non-MSB byte should be left intact";
}

TEST(SerdeRx, shouldReturnFalseIfTheresNotEnoughData)
{
    using Stream = SerialMock<16>;
    using SerdeRX = Serde<uint8, Stream>;
    uint8 data;
    const uint8 input[2] = {
        sSerdeHeaderMsb, sSerdeHeaderLsb,
    };
    Stream stream;
    stream.mRxBuffer.write(input, 2);
    const bool success = SerdeRX::receive(stream, data);
    EXPECT_FALSE(success);
    EXPECT_EQ(stream.mRxBuffer.getLength(), 2);
}

TEST(SerdeRx, shouldReturnFalseIfTheChecksumMismatches)
{
    using Stream = SerialMock<16>;
    using SerdeRX = Serde<uint8, Stream>;
    uint8 data;
    SerdeRX::Packet packet;
    packet.mData[0] = 42;
    const uint8 checksumOk = packet.generateChecksum();
    const uint8 checksumKo = checksumOk + 1;
    const uint8 input[4] = {
        sSerdeHeaderMsb, sSerdeHeaderLsb, 42, checksumKo
    };
    Stream stream;
    stream.mRxBuffer.write(input, 4);
    const bool success = SerdeRX::receive(stream, data);
    EXPECT_FALSE(success);
    EXPECT_EQ(stream.mRxBuffer.getLength(), 0);
}

TEST(SerdeRx, shouldReturnTrueWhenChecksumMatches)
{
    using Stream = SerialMock<16>;
    using SerdeRX = Serde<uint8, Stream>;
    uint8 data;
    SerdeRX::Packet packet;
    packet.mData[0] = 42;
    const uint8 checksumOk = packet.generateChecksum();
    const uint8 input[4] = {
        sSerdeHeaderMsb, sSerdeHeaderLsb, 42, checksumOk
    };
    Stream stream;
    stream.mRxBuffer.write(input, 4);
    const bool success = SerdeRX::receive(stream, data);
    EXPECT_TRUE(success);
    EXPECT_EQ(data, 42);
    EXPECT_EQ(stream.mRxBuffer.getLength(), 0);
}

template<typename T, T Value>
struct CallbackContainer
{
    static void callback(const T& data)
    {
        EXPECT_EQ(data, Value);
        sWasCalled = true;
        sWasCalledWith = data;
    }

    static bool sWasCalled;
    static T sWasCalledWith;
};

template<typename T, T Value>
bool CallbackContainer<T, Value>::sWasCalled = false;

template<typename T, T Value>
T CallbackContainer<T, Value>::sWasCalledWith = T(0);

TEST(SerdeRx, shouldCallTheCallback)
{
    using Stream = SerialMock<16>;
    using SerdeRX = Serde<uint8, Stream>;
    using Cbk = CallbackContainer<uint8, 42>;
    Cbk::sWasCalled = false;
    Cbk::sWasCalledWith = 0;

    SerdeRX::Packet packet;
    packet.mData[0] = 42;
    const uint8 checksum = packet.generateChecksum();
    const uint8 input[4] = {
        sSerdeHeaderMsb, sSerdeHeaderLsb, 42, checksum
    };
    Stream stream;
    stream.mRxBuffer.write(input, 4);
    SerdeRX::read(stream, Cbk::callback);
    EXPECT_TRUE(Cbk::sWasCalled);
    EXPECT_EQ(Cbk::sWasCalledWith, 42);
    EXPECT_EQ(stream.mRxBuffer.getLength(), 0);
}

END_UNNAMED_NAMESPACE
