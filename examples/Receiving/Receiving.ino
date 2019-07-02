#include <serde.h>

struct Command
{
    bool mLedStatus;
    float mPwmLevel;  // Between 0.f and 1.f
};

using SerdeRX = Serde<Command>;

static const int sLedPin = 13; // on Arduino Leonardo
static const int sPwmPin = 3;

void setup()
{
    Serial1.begin(115200);
    pinMode(sLedPin, OUTPUT);
    pinMode(sPwmPin, OUTPUT);
}

void loop()
{
    Command command;
    if (SerdeRX::receive(Serial1, command))
    {
        digitalWrite(sLedPin, command.mLedStatus ? HIGH : LOW);
        analogWrite(sPwmPin, int(command.mPwmLevel * 255));
    }
}
