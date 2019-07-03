#include <serde.h>

enum class CommandTypes
{
    Move,
    Rotate,
};

struct Command
{
    // Define data that is common to all message
    // types at the top level
    CommandTypes type;

    /**
     * Unions allow to use *either* of their contents,
     * rather than all of it at once.
     *
     * It's a contract based on the context where
     * the message is generated / used.
     * Here it's decided by the opcode above.
     */
    union
    {
        struct MoveArgs
        {
            int x;
            int y;
        } move;

        struct RotateArgs
        {
            float angle;
        } rotate;
    };
};

using SerdeRX = Serde<Command>;

void handleCommand(const Command& command)
{
    switch (command.type)
    {
    case CommandTypes::Move:
        analogWrite(5, command.move.x);
        analogWrite(6, command.move.y);
        break;
    case CommandTypes::Rotate:
        analogWrite(10, command.rotate.angle);
        break;
    }
}

void setup()
{
    pinMode(5, OUTPUT); // PWM
    pinMode(6, OUTPUT); // PWM
    pinMode(10, OUTPUT); // PWM

    // SERIAL_PORT_HARDWARE aliases to the default
    // hardware serial port on your board.
    SERIAL_PORT_HARDWARE.begin(115200);
}

void loop()
{
    SerialRX::read(SERIAL_PORT_HARDWARE, handleCommand);
}
