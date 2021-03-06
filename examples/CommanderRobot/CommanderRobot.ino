#include <serde-commander.h>

// TX commands

struct Feedback
{
    float batteryLevel;
};

SERDE_COMMANDER_CREATE_TX(CommanderTX, Feedback);

// RX Commands --

struct Move
{
    int x;
    int y;
};

struct Rotate
{
    float angle;
};

struct RequestFeedback {}; // Command has no arguments

// Callbacks must have the following signature:
// void onTypeNameReceived(const TypeName&)
void onMoveReceived(const Move& move)
{
    analogWrite(5, move.x);
    analogWrite(6, move.y);
}

void onRotateReceived(const Rotate& rotate)
{
    analogWrite(10, rotate.angle);
}

void onRequestFeedbackReceived(const RequestFeedback& /* unused */)
{
    Feedback feedback;
    feedback.batteryLevel = 1.f;
    CommanderTX::send(feedback, SERIAL_PORT_HARDWARE);
}

SERDE_COMMANDER_CREATE_RX(CommanderRX, Move, Rotate, RequestFeedback);

// --

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
    CommanderRX::read(SERIAL_PORT_HARDWARE);
}
