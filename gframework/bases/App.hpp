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

    void Initialize()
    {
        for (int i = 0, n = components->Count(); i < n; i++)
        {
            Component *component = components->GetItem(i);
            component->Initialize();
        }
    }

    void Run()
    {
        for (int i = 0, n = components->Count(); i < n; i++)
        {
            Component *component = components->GetItem(i);
            component->Update();
        }
        delay(500);
    }

    void InitSerial() { InitSerial(9600); }
    void InitSerial(int baudRate) { Serial.begin(baudRate); }

    template <typename T>
    T *RegisterComponent(T *instance)
    {
        components->Add(instance);
        return instance;
    }

private:
    List<Component *> *components;
};