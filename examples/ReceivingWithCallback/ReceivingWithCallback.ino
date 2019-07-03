#include <serde.h>

struct Command
{
    bool mLedStatus;
    float mPwmLevel;  // Between 0.f and 1.f
};

using SerdeRX = Serde<Command>;

static const int sLedPin = 13; // on Arduino Leonardo
static const int sPwmPin = 3;

void handleCommand(const Command& command)
{
    digitalWrite(sLedPin, command.mLedStatus ? HIGH : LOW);
    analogWrite(sPwmPin, int(command.mPwmLevel * 255));
}

void setup()
{
    // SERIAL_PORT_HARDWARE aliases to the default
    // hardware serial port on your board.
    SERIAL_PORT_HARDWARE.begin(115200);
    pinMode(sLedPin, OUTPUT);
    pinMode(sPwmPin, OUTPUT);
}

void loop()
{
    SerdeRX::read(SERIAL_PORT_HARDWARE, handleCommand);
}
