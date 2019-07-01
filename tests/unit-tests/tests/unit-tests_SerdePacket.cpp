#include "unit-tests.h"
#include <serde.h>
#include <tests/mocks/test-mocks_SerialMock.h>

BEGIN_UNNAMED_NAMESPACE

using namespace testing;
using namespace TEST_MOCKS_NAMESPACE;

TEST(SerdePacket, defaultValues)
{
    using Packet = SerdePacket<4>;
    const Packet packet;
    EXPECT_EQ(packet.mHeaderMsb, sSerdeHeaderMsb);
    EXPECT_EQ(packet.mHeaderLsb, sSerdeHeaderLsb);
    EXPECT_EQ(packet.mData[0], 0);
    EXPECT_EQ(packet.mData[1], 0);
    EXPECT_EQ(packet.mData[2], 0);
    EXPECT_EQ(packet.mData[3], 0);
    EXPECT_EQ(packet.mChecksum, 0);
}

TEST(SerdePacket, checksumMatching)
{
    using Packet = SerdePacket<4>;
    Packet packet;
    packet.mData[0] = 1;
    packet.mData[1] = 2;
    packet.mData[2] = 3;
    packet.mData[3] = 4;
    packet.mChecksum = packet.generateChecksum();
    const bool verified = packet.verifyChecksum();
    EXPECT_TRUE(verified);
}

TEST(SerdePacket, checksumMismatching)
{
    using Packet = SerdePacket<4>;
    Packet packet;
    packet.mData[0] = 1;
    packet.mData[1] = 2;
    packet.mData[2] = 3;
    packet.mData[3] = 4;
    packet.mChecksum = packet.generateChecksum();

    // Modify data
    packet.mData[2] = 42;
    const bool verified = packet.verifyChecksum();
    EXPECT_FALSE(verified);
}

END_UNNAMED_NAMESPACE
