#pragma once
#include "../enums/includes.h"
#include "../bases/includes.h"

class Pin : public Component
{
public:
    Pin(int number, PinType type, PinMode mode)
    {
        this->number = number;
        this->type = type;
        this->mode = mode;
    }

    Pin(int number, PinType type, PinMode mode, int initialValue)
    {
        this->number = number;
        this->type = type;
        this->mode = mode;

        Write(initialValue);
    }

    void Initialize()
    {
        SetMode(mode);
        initialized = true;
        Component::Initialize();
    }

    int GetNumber() { return this->number; }
    PinType GetType() { return this->type; }
    PinMode GetMode() { return this->mode; }

    void SetMode(PinMode mode)
    {
        if (!initialized)
        {
            Serial.print("Pin ");
            Serial.print(number, DEC);
            Serial.print(" has been set to ");
        }

        this->mode = mode;

        int nmode;
        switch (this->mode)
        {
        case PinMode::IN:
            nmode = INPUT;

            if (!initialized)
                Serial.println("INPUT!");
            break;
        case PinMode::IN_PULLUP:
            nmode = INPUT_PULLUP;

            if (!initialized)
                Serial.println("INPUT_PULLUP!");
            break;
        case PinMode::OUT:
            nmode = OUTPUT;

            if (!initialized)
                Serial.println("OUTPUT!");
            break;
        }

        pinMode(this->number, nmode);
    }

    int Read()
    {
        if (this->type == PinType::DIGITAL)
            return digitalRead(this->number);
        else
            return analogRead(this->number);
    }

    int Read(uint8_t precision)
    {
        int value = 0;

        for (uint8_t i = 0; i < precision; i++)
            value += this->Read();

        return value / precision;
    }

    void Write(int value)
    {
        if (this->type == PinType::DIGITAL)
            digitalWrite(this->number, value);
        else
            analogWrite(this->number, value);
    }

private:
    PinType type;
    PinMode mode;
    int number;
    bool initialized = false;
};