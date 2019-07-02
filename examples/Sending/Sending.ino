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
    Serial1.begin(115200);
}

void loop()
{
    output.mCounter += 1;
    output.mTime = millis();
    SerdeTX::send(output, Serial1);
}
