#include <serde.h>

/**
 * This example shows how to use the same serial port
 * to send and receive different types of data.
 *
 * The use case of this example is to build a co-processor
 * using an Arduino, where calculations are sent via
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

using SerdeRX = Serde<Command>; // Receive Command objects
using SerdeTX = Serde<float>;   // Send a single float back

// --

void setup()
{
    // SERIAL_PORT_HARDWARE aliases to the default
    // hardware serial port on your board.
    SERIAL_PORT_HARDWARE.begin(115200);
}

void loop()
{
    Command command;
    if (SerdeRX::receive(SERIAL_PORT_HARDWARE, command))
    {
        switch (command.mOperation)
        {
        case Operations::Add:
            SerdeTX::send(
              command.mOperandA + command.mOperandB,
              SERIAL_PORT_HARDWARE
            );
            break;
        case Operations::Subtract:
            SerdeTX::send(
              command.mOperandA - command.mOperandB,
              SERIAL_PORT_HARDWARE
            );
            break;
        case Operations::Multiply:
            SerdeTX::send(
              command.mOperandA * command.mOperandB,
              SERIAL_PORT_HARDWARE
            );
            break;
        case Operations::Divide:
            SerdeTX::send(
              command.mOperandA / command.mOperandB,
              SERIAL_PORT_HARDWARE
            );
            break;
        }
    }
}
