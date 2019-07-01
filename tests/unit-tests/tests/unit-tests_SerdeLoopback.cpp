#include "unit-tests.h"
#include <serde.h>
#include <tests/mocks/test-mocks_SerialMock.h>

BEGIN_UNNAMED_NAMESPACE

using namespace testing;
using namespace TEST_MOCKS_NAMESPACE;

TEST(SerdeLoopbackScalar, bool)
{
    using Stream = SerialMock<16>;
    using Serde = Serde<bool, Stream>;

    bool value;
    Stream stream;
    {
        Serde::send(true, stream);
        stream.moveTxToRx();
        const bool success = Serde::receive(stream, value);
        EXPECT_TRUE(success);
        EXPECT_EQ(true, value);
    }
    {
        Serde::send(false, stream);
        stream.moveTxToRx();
        const bool success = Serde::receive(stream, value);
        EXPECT_TRUE(success);
        EXPECT_EQ(false, value);
    }
}

TEST(SerdeLoopbackScalar, float)
{
    using Stream = SerialMock<16>;
    using Serde = Serde<float, Stream>;

    float value;
    Stream stream;
    {
        Serde::send(0.1234f, stream);
        stream.moveTxToRx();
        const bool success = Serde::receive(stream, value);
        EXPECT_TRUE(success);
        EXPECT_FLOAT_EQ(0.1234f, value);
    }
    {
        Serde::send(-1e-9f, stream);
        stream.moveTxToRx();
        const bool success = Serde::receive(stream, value);
        EXPECT_TRUE(success);
        EXPECT_FLOAT_EQ(-1e-9f, value);
    }
}

TEST(SerdeLoopbackScalar, byte)
{
    using Stream = SerialMock<16>;
    using Serde = Serde<uint8_t, Stream>;

    uint8_t value;
    Stream stream;
    {
        Serde::send(42, stream);
        stream.moveTxToRx();
        const bool success = Serde::receive(stream, value);
        EXPECT_TRUE(success);
        EXPECT_EQ(42, value);
    }
    {
        Serde::send(0xff, stream);
        stream.moveTxToRx();
        const bool success = Serde::receive(stream, value);
        EXPECT_TRUE(success);
        EXPECT_EQ(0xff, value);
    }
}

TEST(Serde, structPack)
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
    using Serde = Serde<Foo, Stream>;

    Stream stream;
    Foo fooTx;
    fooTx.mBoolean  = true;
    fooTx.mUint8    = 0x2a;
    fooTx.mUint16   = 0xf00d;
    fooTx.mUint32   = 0xbaadf00d;
    fooTx.mUint64   = 0xdeadcafebaadf00d;
    fooTx.mInt8     = -1;
    fooTx.mInt16    = -2;
    fooTx.mInt32    = -3;
    fooTx.mInt64    = -4;
    fooTx.mFloat    = 0.1234f;
    fooTx.mDouble   = 0.123;
    Serde::send(fooTx, stream);
    stream.moveTxToRx();
    Foo fooRx;
    const bool success = Serde::receive(stream, fooRx);
    EXPECT_TRUE(success);
    EXPECT_EQ(fooTx.mBoolean, fooRx.mBoolean);
    EXPECT_EQ(fooTx.mUint8,   fooRx.mUint8);
    EXPECT_EQ(fooTx.mUint16,  fooRx.mUint16);
    EXPECT_EQ(fooTx.mUint32,  fooRx.mUint32);
    EXPECT_EQ(fooTx.mUint64,  fooRx.mUint64);
    EXPECT_EQ(fooTx.mInt8,    fooRx.mInt8);
    EXPECT_EQ(fooTx.mInt16,   fooRx.mInt16);
    EXPECT_EQ(fooTx.mInt32,   fooRx.mInt32);
    EXPECT_EQ(fooTx.mInt64,   fooRx.mInt64);
    EXPECT_FLOAT_EQ(fooTx.mFloat, fooRx.mFloat);
    EXPECT_DOUBLE_EQ(fooTx.mDouble, fooRx.mDouble);
}

END_UNNAMED_NAMESPACE
