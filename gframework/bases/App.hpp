#pragma once
#include "../collections/includes.h"
#include "Component.hpp"

class App
{
public:
    App()
    {
        this->components = new List<Component *>();
    }

    void Setup()
    {
    }

    void Initialize()
    {
        for (int i = 0, n = components->Count(); i < n; i++)
        {
            Component *component = components->GetItem(i);
            component->Initialize();
        }

        Serial.print(components->Count(), DEC);
        Serial.println(" components has been initialized!");
    }

    void Run()
    {
        for (int i = 0, n = components->Count(); i < n; i++)
        {
            Component *component = components->GetItem(i);
            component->Update();
        }
        delay(100);
    }

    void InitSerial() { InitSerial(9600); }
    void InitSerial(unsigned long baudRate)
    {
        Serial.begin(baudRate);
        while (!Serial)
        {
            delay(100);
        }
    }

    template <typename T>
    T *RegisterComponent(T *instance)
    {
        components->Add(instance);
        return instance;
    }

private:
    List<Component *> *components;
};