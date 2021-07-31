#line 1 "c:\\Users\\gabri\\Projetos\\Arduino\\GFramework\\gframework\\bases\\PinRegistrar.hpp"
#pragma once
#include <stdio.h>
#include "Pin.hpp"
#include "../collections/includes.h"

class PinRegistrar
{
public:
    PinRegistrar()
    {
        this->pins = new List<Pin *>();
    }

    template <typename T>
    T *RegisterPin(T *instance)
    {
        for (int i = 0, n = pins->Count(); i < n; i++)
        {
            Pin *pin = pins->GetItem(i);
            if (pin->GetNumber() == instance->GetNumber())
                return NULL;
        }

        pins->Add(instance);
        return instance;
    }

    void Initialize()
    {
        for (int i = 0, n = pins->Count(); i < n; i++)
        {
            Pin *pin = pins->GetItem(i);
            pin->Initialize();
        }
    }

private:
    List<Pin *> *pins;
};