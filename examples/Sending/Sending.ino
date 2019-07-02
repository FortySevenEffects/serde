#include <serde.h>

struct Output
{
    unsigned long mCounter;
    unsigned long mTime;
};

using SerdeTX = Serde<Output>;

Output output;

void setup()
{
    // SERIAL_PORT_HARDWARE aliases to the default
    // hardware serial port on your board.
    SERIAL_PORT_HARDWARE.begin(115200);
}

void loop()
{
    output.mCounter += 1;
    output.mTime = millis();
    SerdeTX::send(output, SERIAL_PORT_HARDWARE);
}
