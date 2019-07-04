#include <serde.h>

struct Output
{
    unsigned long counter;
    unsigned long time;
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
    output.counter += 1;
    output.time = millis();
    SerdeTX::send(output, SERIAL_PORT_HARDWARE);
}
