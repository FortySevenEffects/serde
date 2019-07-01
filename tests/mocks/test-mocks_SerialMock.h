#pragma once

#include "test-mocks.h"
#include "test-mocks_Defs.h"
#include <string.h> // for size_t
#include <algorithm>

BEGIN_TEST_MOCKS_NAMESPACE

template<typename DataType, int Size>
class RingBuffer
{
public:
     RingBuffer();
    ~RingBuffer();

public:
    int getLength() const;
    bool isEmpty() const;

public:
    void write(DataType inData);
    void write(const DataType* inData, int inSize);
    void clear();

public:
    DataType peek() const;
    DataType read();
    size_t readSafe(DataType* outData, int inSize);
    size_t readOverflow(DataType* outData, int inSize);

private:
    DataType mData[Size];
    DataType* mWriteHead;
    DataType* mReadHead;
};

// -----------------------------------------------------------------------------

template<int BufferSize>
class SerialMock
{
public:
     SerialMock();
    ~SerialMock();

public: // Arduino Serial API
    int available() const;
    void write(uint8 inData);
    void write(const uint8* inData, int inLength);
    uint8 peek();
    uint8 read();
    size_t readBytes(uint8* outBuffer, int inLength);

public: // Test Helpers API
    void moveTxToRx(); // Simulate loopback

public:
    typedef RingBuffer<uint8, BufferSize> Buffer;
    Buffer mTxBuffer;
    Buffer mRxBuffer;
};

END_TEST_MOCKS_NAMESPACE

#include "test-mocks_SerialMock.hpp"
