#include "unit-tests.h"
#include <serde.h>
#include <tests/mocks/test-mocks_SerialMock.h>

BEGIN_UNNAMED_NAMESPACE

using namespace testing;
using namespace TEST_MOCKS_NAMESPACE;

TEST(SerdeTxScalar, bool)
{
    using Stream = SerialMock<16>;
    using SerdeTX = Serde<bool, Stream>;

    Stream stream;
    SerdeTX::send(true, stream);
    EXPECT_EQ(sizeof(SerdeTX::Packet), stream.mTxBuffer.getLength());
    stream.mTxBuffer.clear();
    SerdeTX::send(false, stream);
    EXPECT_EQ(sizeof(SerdeTX::Packet), stream.mTxBuffer.getLength());
}

TEST(SerdeTxScalar, float)
{
    using Stream = SerialMock<16>;
    using SerdeTX = Serde<float, Stream>;

    Stream stream;
    SerdeTX::send(0.1234f, stream);
    EXPECT_EQ(sizeof(SerdeTX::Packet), stream.mTxBuffer.getLength());
}

TEST(SerdeTxScalar, byte)
{
    using Stream = SerialMock<16>;
    using SerdeTX = Serde<uint8_t, Stream>;

    Stream stream;
    SerdeTX::send(42, stream);
    EXPECT_EQ(sizeof(SerdeTX::Packet), stream.mTxBuffer.getLength());
}

TEST(SerdeTx, structPack)
{
    struct Foo
    {
        bool      mBoolean = false;
        uint8_t   mUint8   = 0;
        uint16_t  mUint16  = 0;
        uint32_t  mUint32  = 0;
        uint64_t  mUint64  = 0;
        int8_t    mInt8    = 0;
        int16_t   mInt16   = 0;
        int32_t   mInt32   = 0;
        int64_t   mInt64   = 0;
        float     mFloat   = 0.f;
        double    mDouble  = 0.;
    };

    using Stream = SerialMock<64>;
    using SerdeTX = Serde<Foo, Stream>;

    Stream stream;
    const Foo foo;
    SerdeTX::send(foo, stream);
    EXPECT_EQ(sizeof(SerdeTX::Packet), stream.mTxBuffer.getLength());
}

END_UNNAMED_NAMESPACE
