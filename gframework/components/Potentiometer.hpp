#pragma once
#include "Pin.hpp"
#include "../bases/includes.h"

class Potentiometer : public Pin
{
public:
    Potentiometer(int pin) : Pin(pin, PinType::ANALOG, PinMode::IN)
    {
        precision = 5;
    }

    void SetPrecision(uint8_t precision) { this->precision = precision; }
    uint8_t GetPrecision() { return this->precision; }

    int Read() { return Pin::Read(precision); }

private:
    uint8_t precision;
};