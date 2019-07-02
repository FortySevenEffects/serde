#include <serde.h>

/**
 * This example shows how to use the same serial port
 * to send and receive different types of data.
 *
 * The use case of this example is to build a co-processor
 * out of an Arduino, where calculations are sent via
 * the serial port and the result is sent back also
 * via the same serial port.
 */

// First we define the data models to exchange
// These definition should be shared with the sending device.
enum class Operations
{
    Add = 0,
    Subtract,
    Multiply,
    Divide,
};

struct Command
{
    Operations mOperation;
    float mOperandA;
    float mOperandB;
};

using Result = float; // You can use free-form scalar types too.

using SerdeRX = Serde<Command>; // Receive Command objects
using SerdeTX = Serde<Result>;  // Send a single float back

// --

void setup()
{
    // Example for an Arduino Leonardo, you might need
    // to swap Serial1 for the default hardware Serial
    // port for your board.

    Serial1.begin(115200);
}

void loop()
{
    Command command;
    if (SerdeRX::receive(Serial1, command))
    {
        switch (command.mOperation)
        {
        case Operations::Add:
            SerdeTX::send(command.mOperandA + command.mOperandB, Serial1);
            break;
        case Operations::Subtract:
            SerdeTX::send(command.mOperandA - command.mOperandB, Serial1);
            break;
        case Operations::Multiply:
            SerdeTX::send(command.mOperandA * command.mOperandB, Serial1);
            break;
        case Operations::Divide:
            SerdeTX::send(command.mOperandA / command.mOperandB, Serial1);
            break;
        }
    }
}
