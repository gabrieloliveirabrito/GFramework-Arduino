#line 1 "c:\\Users\\gabri\\Projetos\\Arduino\\GFramework\\gframework\\bases\\Pin.hpp"
#pragma once
#include "../enums/includes.h"

class Pin
{
public:
    Pin(int number, PinMode mode, PinType type)
    {
        this->number = number;
        this->mode = mode;
        this->type = type;
    }

    void Initialize()
    {
        pinMode(this->number, this->type == PinType::IN ? INPUT : OUTPUT);
    }

    int GetNumber() { return this->number; }
    PinMode GetMode() { return this->mode; }
    PinType GetType() { return this->type; }

    int Read()
    {
        if (this->mode == PinMode::DIGITAL)
            return digitalRead(this->number);
        else
            return analogRead(this->number);
    }

    void Write(int value)
    {
        if (this->mode == PinMode::DIGITAL)
            return digitalWrite(this->number, value);
        else
            return analogWrite(this->number, value);
    }

private:
    PinMode mode;
    PinType type;
    int number;
};