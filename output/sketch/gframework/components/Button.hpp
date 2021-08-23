#line 1 "c:\\Users\\gabri\\Projetos\\Arduino\\GFramework\\gframework\\components\\Button.hpp"
#pragma once
#include "Pin.hpp"

class Button : public Pin
{
public:
    Button(int pin) : Pin(pin, PinType::DIGITAL, PinMode::IN_PULLUP)
    {
    }
};