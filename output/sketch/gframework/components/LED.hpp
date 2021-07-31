#line 1 "c:\\Users\\gabri\\Projetos\\Arduino\\GFramework\\gframework\\components\\LED.hpp"
#pragma once
#include "Pin.hpp"

class LED : public Pin
{
public:
    LED(int number) : Pin(number, PinMode::DIGITAL, PinType::OUT)
    {
        this->state = LOW;
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