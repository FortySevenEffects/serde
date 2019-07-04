#include <serde.h>

struct Command
{
    bool ledStatus;
    float pwmLevel;  // Between 0.f and 1.f
};

using SerdeRX = Serde<Command>;

static const int sLedPin = 13; // on Arduino Leonardo
static const int sPwmPin = 3;

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
    Command command;
    if (SerdeRX::receive(SERIAL_PORT_HARDWARE, command))
    {
        digitalWrite(sLedPin, command.ledStatus ? HIGH : LOW);
        analogWrite(sPwmPin, int(command.pwmLevel * 255));
    }
}
