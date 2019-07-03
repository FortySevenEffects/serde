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

using SerdeTX = Serde<Command>;

void setup()
{
    // SERIAL_PORT_HARDWARE aliases to the default
    // hardware serial port on your board.
    SERIAL_PORT_HARDWARE.begin(115200);
}

void loop()
{
    if (digitalRead(1))
    {
        Command command;
        command.type = CommandTypes::Move;
        command.move.x = analogRead(0);
        command.move.y = analogRead(1);
        SerdeTX::send(command, SERIAL_PORT_HARDWARE);
    }
    if (digitalRead(2))
    {
        Command command;
        command.type = CommandTypes::Rotate;
        command.rotate.angle = 90;
        SerdeTX::send(command, SERIAL_PORT_HARDWARE);
    }
}
