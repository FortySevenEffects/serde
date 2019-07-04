#include <serde-commander.h>

/**
 * This example shows the same use-case as DifferentTypesForTXandRX,
 * but using Commander instead of manually checking the operation type.
 *
 * As the result is a plain float value, we'll use a standard Serde
 * interface, no need for a Commander here, as long as the other
 * end uses CommanderTX and SerdeRX.
 */

using SerdeTX = Serde<float>;

// RX Commands

struct UnaryOperation
{
    float a;
};
struct TwoArgumentsOperation
{
    float a;
    float b;
};

using Add       = TwoArgumentsOperation;
using Subtract  = TwoArgumentsOperation;
using Multiply  = TwoArgumentsOperation;
using Divide    = TwoArgumentsOperation;
using Negate    = UnaryOperation;
using Abs       = UnaryOperation;

void onAddReceived(const Add& args)
{
    SerdeTX::send(args.a + args.b, SERIAL_PORT_HARDWARE);
}

void onSubtractReceived(const Subtract& args)
{
    SerdeTX::send(args.a - args.b, SERIAL_PORT_HARDWARE);
}

void onMultiplyReceived(const Multiply& args)
{
    SerdeTX::send(args.a * args.b, SERIAL_PORT_HARDWARE);
}

void onDivideReceived(const Divide& args)
{
    SerdeTX::send(args.a / args.b, SERIAL_PORT_HARDWARE);
}

void onNegateReceived(const Negate& args)
{
    SerdeTX::send(-args.a, SERIAL_PORT_HARDWARE);
}

void onAbsReceived(const Abs& args)
{
    SerdeTX::send(args.a > 0 ? args.a : -args.a, SERIAL_PORT_HARDWARE);
}

SERDE_COMMANDER_CREATE_RX(CommanderRX,
    Add,
    Subtract,
    Multiply,
    Divide,
    Negate,
    Abs
);

// --

void setup()
{
    // SERIAL_PORT_HARDWARE aliases to the default
    // hardware serial port on your board.
    SERIAL_PORT_HARDWARE.begin(115200);
}

void loop()
{
    CommanderRX::read(SERIAL_PORT_HARDWARE);
}
