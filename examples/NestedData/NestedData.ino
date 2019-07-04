#include <serde.h>

/**
 * You can nest data structures, as long as everything
 * at every level is POD (Plain Old Data):
 * https://stackoverflow.com/questions/146452/what-are-pod-types-in-c#146454
 */

struct LatLng
{
    float latitude;
    float longitude;
};

struct Data
{
    LatLng from;
    LatLng to;
};

using SerdeTX = Serde<Data>;

void setup()
{
    // SERIAL_PORT_HARDWARE aliases to the default
    // hardware serial port on your board.
    SERIAL_PORT_HARDWARE.begin(115200);
}

void loop()
{
    Data infernal;
    infernal.from.latitude  = 48.8567f;
    infernal.from.longitude = 2.3508f;
    infernal.to.latitude    = 52.516667f;
    infernal.to.longitude   = 13.388889f;
    SerdeTX::send(infernal, SERIAL_PORT_HARDWARE);
}
