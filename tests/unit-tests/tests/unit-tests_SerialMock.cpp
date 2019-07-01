#include "unit-tests.h"
#include <tests/mocks/test-mocks_SerialMock.h>

BEGIN_UNNAMED_NAMESPACE

USING_NAMESPACE_TEST_MOCKS
using namespace testing;

TEST(RingBufferMock, initialState)
{
    typedef RingBuffer<uint8, 32> Buffer;
    Buffer buffer;
    EXPECT_EQ(buffer.getLength(), 0);
    EXPECT_EQ(buffer.isEmpty(),   true);
    buffer.clear();
    EXPECT_EQ(buffer.getLength(), 0);
    EXPECT_EQ(buffer.isEmpty(),   true);
}

TEST(RingBufferMock, uint8)
{
    typedef RingBuffer<uint8, 8> Buffer;
    Buffer buffer;

    buffer.write(42);
    EXPECT_EQ(buffer.getLength(), 1);
    EXPECT_EQ(buffer.isEmpty(),   false);

    const uint8 read = buffer.read();
    EXPECT_EQ(read, 42);
    EXPECT_EQ(buffer.getLength(), 0);
    EXPECT_EQ(buffer.isEmpty(),   true);

    const uint8 data[] = "Hello, World!";
    buffer.write(data, 13);
    EXPECT_EQ(buffer.getLength(), 5); // 13 % 8
    EXPECT_EQ(buffer.isEmpty(), false);

    uint8 output[8] = { 0 };
    const size_t overflowLength = buffer.readOverflow(output, 8);
    EXPECT_EQ(overflowLength, 8);
    const uint8 expectedOverflow[8] = {
        'o', 'r', 'l', 'd', '!', ',', ' ', 'W',
    };
    EXPECT_THAT(output, ContainerEq(expectedOverflow));

    buffer.clear();
    EXPECT_EQ(buffer.getLength(), 0);
    EXPECT_EQ(buffer.isEmpty(),   true);

    buffer.write(data, 13);
    memset(output, 0, 8);
    const size_t safeLength = buffer.readSafe(output, 8);
    EXPECT_EQ(safeLength, 5);
    const uint8 expectedSafe[8] = {
        // does this make sense ?
        'o', 'r', 'l', 'd', '!', 0, 0, 0,
    };
    EXPECT_THAT(output, ContainerEq(expectedSafe));
}

TEST(RingBufferMock, uint32)
{
    typedef RingBuffer<uint32_t, 32> Buffer;
    Buffer buffer;
    buffer.write(42);
    EXPECT_EQ(buffer.getLength(), 1);
    EXPECT_EQ(buffer.isEmpty(),   false);
    const uint8 read = buffer.read();
    EXPECT_EQ(read, 42);
    EXPECT_EQ(buffer.getLength(), 0);
    EXPECT_EQ(buffer.isEmpty(),   true);
}

END_UNNAMED_NAMESPACE
