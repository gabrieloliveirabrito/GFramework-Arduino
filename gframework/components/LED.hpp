#pragma once
#include "Pin.hpp"

class LED : public Pin
{
public:
    LED(int number) : Pin(number, PinType::DIGITAL, PinMode::OUT)
    {
        this->state = LOW;
        this->Write(state);
    }

    LED(int number, PinType type) : Pin(number, type, PinMode::OUT)
    {
        this->state = LOW;
        this->Write(state);
    }

    LED(int number, int startState) : Pin(number, PinType::DIGITAL, PinMode::OUT)
    {
        this->state = startState;
        this->Write(state);
    }

    void Enable()
    {
        this->state = HIGH;
        this->Write(HIGH);
    }

    void Disable()
    {
        this->state = LOW;
        this->Write(LOW);
    }

    void Toggle()
    {
        if (this->state == LOW)
            this->Enable();
        else
            this->Disable();
    }

private:
    int state;
};