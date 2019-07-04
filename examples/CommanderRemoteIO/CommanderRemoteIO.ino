#include <serde-commander.h>

// TX Commands

struct PinState
{
    byte pinNumber;
    bool state;
};

SERDE_COMMANDER_CREATE_TX(CommanderTX, PinState);

// RX Commands

struct SetPinState
{
    byte pinNumber;
    bool state;
};

struct GetPinState
{
    byte pinNumber;
};

void onSetPinStateReceived(const SetPinState& args)
{
    digitalWrite(args.pinNumber, args.state ? HIGH : LOW);
}

void onGetPinStateReceived(const GetPinState& args)
{
    PinState reply;
    reply.pinNumber = args.pinNumber;
    reply.state     = digitalRead(args.pinNumber);
    CommanderTX::send(reply, SERIAL_PORT_HARDWARE);
}

SERDE_COMMANDER_CREATE_RX(CommanderRX, SetPinState, GetPinState);

void setup()
{
    SERIAL_PORT_HARDWARE.begin(115200);
}

void loop()
{
    CommanderRX::read(SERIAL_PORT_HARDWARE);
}
