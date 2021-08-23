#pragma once
#include "Pin.hpp"
#include "../enums/includes.h"

class Button : public Pin
{
public:
    Button(int pin) : Pin(pin, PinType::DIGITAL, PinMode::IN_PULLUP)
    {
        state = Idle;
    }

    ButtonState GetState() { return state; }

    bool IsIdle() { return state == Idle; }
    bool IsPressed() { return state == Pressed; }
    bool IsHolding() { return state == Holding; }
    bool IsReleased() { return state == Released; }

    void Update()
    {
        Pin::Update();

        int lState = Read();
        if (lState == LOW)
        {
            if (state == Idle)
            {
                Serial.println("Pressed");
                state = Pressed;
            }
            else if (state == Pressed)
            {
                Serial.println("Holding");
                state = Holding;
            }
        }
        else if (lState == HIGH)
        {
            if (state == Pressed || state == Holding)
            {
                Serial.println("Released");
                state = Released;
            }
            else if (state == Released)
            {
                Serial.println("Idle");
                state = Idle;
            }
        }
    }

private:
    ButtonState state;
};