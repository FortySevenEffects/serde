#pragma once

#include "serde-macros.h"
#include "serde.h"

// Name resolution

#define _SERDE_COMMANDER_COMMAND_TYPE_NAME(name)    name##SCTypes
#define _SERDE_COMMANDER_MESSAGE_TYPE_NAME(name)    name##SCMessage
#define _SERDE_COMMANDER_INTERFACE_NAME(name)       name##SCInterface

// Code fragment generation

#define _SERDE_COMMANDER_DECLARE_UNION_MEMBER(Type) Type m##Type;

#define _SERDE_COMMANDER_DECLARE_SWITCH_CASE(Type)                              \
    case Enum::Type:                                                            \
        on##Type##Received(inMessage.m##Type);                                  \
        break;

#define _SERDE_COMMANDER_DECLARE_SENDING_METHOD(Type)                           \
    static void send(const Type& inData, Stream& inStream)                      \
    {                                                                           \
        Message message;                                                        \
        message.mType = Enum::Type;                                             \
        message.m##Type = inData;                                               \
        Super::send(message, inStream);                                         \
    }

// Object generation

#define _SERDE_COMMANDER_DECLARE_COMMAND_TYPES(prefix, ...)                     \
    enum class _SERDE_COMMANDER_COMMAND_TYPE_NAME(prefix)                       \
    {                                                                           \
        __VA_ARGS__                                                             \
    };

#define _SERDE_COMMANDER_DECLARE_MESSAGE_TYPE(name, ...)                        \
    struct _SERDE_COMMANDER_MESSAGE_TYPE_NAME(name)                             \
    {                                                                           \
        _SERDE_COMMANDER_COMMAND_TYPE_NAME(name) mType;                         \
        union                                                                   \
        {                                                                       \
            CALL_MACRO_X_FOR_EACH(                                              \
                _SERDE_COMMANDER_DECLARE_UNION_MEMBER,                          \
                __VA_ARGS__                                                     \
            )                                                                   \
        };                                                                      \
    };

// Interface generation (sharded for separate TX/RX/both operation)

#define _SERDE_COMMANDER_BEGIN_INTERFACE(name, ...)                             \
    template<typename Stream>                                                   \
    struct _SERDE_COMMANDER_INTERFACE_NAME(name)                                \
    {                                                                           \
    private:                                                                    \
        using Enum      = _SERDE_COMMANDER_COMMAND_TYPE_NAME(name);             \
        using Message   = _SERDE_COMMANDER_MESSAGE_TYPE_NAME(name);             \
        using Self      = _SERDE_COMMANDER_INTERFACE_NAME(name)<Stream>;        \
        using Super     = Serde<Message, Stream>;

#define _SERDE_COMMANDER_INTERFACE_TX_SHARD(name, ...)                          \
    public:                                                                     \
        CALL_MACRO_X_FOR_EACH(                                                  \
            _SERDE_COMMANDER_DECLARE_SENDING_METHOD,                            \
            __VA_ARGS__                                                         \
        )

#define _SERDE_COMMANDER_INTERFACE_RX_SHARD(name, ...)                          \
    private:                                                                    \
        static void callback(const Message& inMessage)                          \
        {                                                                       \
            switch (inMessage.mType)                                            \
            {                                                                   \
                CALL_MACRO_X_FOR_EACH(                                          \
                    _SERDE_COMMANDER_DECLARE_SWITCH_CASE,                       \
                    __VA_ARGS__                                                 \
                )                                                               \
            }                                                                   \
        }                                                                       \
                                                                                \
    public:                                                                     \
        static void read(Stream& inStream)                                      \
        {                                                                       \
            Super::read(inStream, Self::callback);                              \
        }

#define _SERDE_COMMANDER_END_INTERFACE(name, ...)                               \
    };

#define _SERDE_COMMANDER_DECLARE_COMMON_TYPES(name, ...)                        \
    _SERDE_COMMANDER_DECLARE_COMMAND_TYPES(name, __VA_ARGS__ )                  \
    _SERDE_COMMANDER_DECLARE_MESSAGE_TYPE(name, __VA_ARGS__)                    \

// Top-level macros

#define SERDE_COMMANDER_CREATE_TX(name, ...)                                    \
    _SERDE_COMMANDER_DECLARE_COMMON_TYPES(name, __VA_ARGS__)                    \
    _SERDE_COMMANDER_BEGIN_INTERFACE(name, __VA_ARGS__)                         \
    _SERDE_COMMANDER_INTERFACE_TX_SHARD(name, __VA_ARGS__)                      \
    _SERDE_COMMANDER_END_INTERFACE(name, __VA_ARGS__)                           \
    using name = _SERDE_COMMANDER_INTERFACE_NAME(name)<Stream>;

#define SERDE_COMMANDER_CREATE_RX(name, ...)                                    \
    _SERDE_COMMANDER_DECLARE_COMMON_TYPES(name, __VA_ARGS__)                    \
    _SERDE_COMMANDER_BEGIN_INTERFACE(name, __VA_ARGS__)                         \
    _SERDE_COMMANDER_INTERFACE_RX_SHARD(name, __VA_ARGS__)                      \
    _SERDE_COMMANDER_END_INTERFACE(name, __VA_ARGS__)                           \
    using name = _SERDE_COMMANDER_INTERFACE_NAME(name)<Stream>;

#define SERDE_COMMANDER_CREATE(name, ...)                                       \
    _SERDE_COMMANDER_DECLARE_COMMON_TYPES(name, __VA_ARGS__)                    \
    _SERDE_COMMANDER_BEGIN_INTERFACE(name, __VA_ARGS__)                         \
    _SERDE_COMMANDER_INTERFACE_TX_SHARD(name, __VA_ARGS__)                      \
    _SERDE_COMMANDER_INTERFACE_RX_SHARD(name, __VA_ARGS__)                      \
    _SERDE_COMMANDER_END_INTERFACE(name, __VA_ARGS__)                           \
    using name = _SERDE_COMMANDER_INTERFACE_NAME(name)<Stream>;

// --

#define SERDE_COMMANDER_CREATE_CUSTOM_TX(name, Stream, ...)                     \
    _SERDE_COMMANDER_DECLARE_COMMON_TYPES(name, __VA_ARGS__)                    \
    _SERDE_COMMANDER_BEGIN_INTERFACE(name, __VA_ARGS__)                         \
    _SERDE_COMMANDER_INTERFACE_TX_SHARD(name, __VA_ARGS__)                      \
    _SERDE_COMMANDER_END_INTERFACE(name, __VA_ARGS__)                           \
    using name = _SERDE_COMMANDER_INTERFACE_NAME(name)<Stream>;

#define SERDE_COMMANDER_CREATE_CUSTOM_RX(name, Stream, ...)                     \
    _SERDE_COMMANDER_DECLARE_COMMON_TYPES(name, __VA_ARGS__)                    \
    _SERDE_COMMANDER_BEGIN_INTERFACE(name, __VA_ARGS__)                         \
    _SERDE_COMMANDER_INTERFACE_RX_SHARD(name, __VA_ARGS__)                      \
    _SERDE_COMMANDER_END_INTERFACE(name, __VA_ARGS__)                           \
    using name = _SERDE_COMMANDER_INTERFACE_NAME(name)<Stream>;

#define SERDE_COMMANDER_CREATE_CUSTOM(name, Stream, ...)                        \
    _SERDE_COMMANDER_DECLARE_COMMON_TYPES(name, __VA_ARGS__)                    \
    _SERDE_COMMANDER_BEGIN_INTERFACE(name, __VA_ARGS__)                         \
    _SERDE_COMMANDER_INTERFACE_TX_SHARD(name, __VA_ARGS__)                      \
    _SERDE_COMMANDER_INTERFACE_RX_SHARD(name, __VA_ARGS__)                      \
    _SERDE_COMMANDER_END_INTERFACE(name, __VA_ARGS__)                           \
    using name = _SERDE_COMMANDER_INTERFACE_NAME(name)<Stream>;
