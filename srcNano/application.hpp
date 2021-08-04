#pragma once
#include <SPI.h>
#include <Ethernet.h>

#include "../gframework/includes.h"

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

class Application : App
{
private:
    LED *blink, *potLed;
    Pin *potentiometer, *lm35, *optoLed;
    StateMachine<Application, int> *intMachine;
    StateMachine<Application, float> *floatMachine;

    EventHandler<Application, int> *GetPotentiometerEvent, *GetOptoEvent;
    EventHandler<Application, void, int, int> *PotentiometerValueChangedEvent, *OptoValueChangedEvent;
    EventHandler<Application, float> *GetLM35Event;
    EventHandler<Application, void, float, float> *LM35ValueChangedEvent;

public:
    void Setup()
    {
        InitSerial();
        Serial.println("Creating components...");

        blink = RegisterComponent(new LED(LED_BUILTIN));
        potLed = RegisterComponent(new LED(11, PinType::ANALOG));
        potentiometer = RegisterComponent(new Pin(A0, PinType::ANALOG, PinMode::IN));
        lm35 = RegisterComponent(new Pin(A7, PinType::ANALOG, PinMode::IN));
        optoLed = RegisterComponent(new Pin(10, PinType::DIGITAL, PinMode::IN));

        intMachine = new StateMachine<Application, int>();
        floatMachine = new StateMachine<Application, float>();

        CreateEvents();
        App::Setup();
    }

    void CreateEvents()
    {
        GetPotentiometerEvent = new EventHandler<Application, int>(this, &Application::GetPotentiometerValue);
        PotentiometerValueChangedEvent = new EventHandler<Application, void, int, int>(this, &Application::PotentiometerValueChanged);

        GetOptoEvent = new EventHandler<Application, int>(this, &Application::GetOptoValue);
        OptoValueChangedEvent = new EventHandler<Application, void, int, int>(this, &Application::OptoValueChanged);

        GetLM35Event = new EventHandler<Application, float>(this, &Application::GetLM35Value);
        LM35ValueChangedEvent = new EventHandler<Application, void, float, float>(this, &Application::LM35ValueChanged);
    }

    void Initialize()
    {
        Serial.println("Initializing...");
        App::Initialize();

        intMachine->AddHandler("POTENTIOMETER", GetPotentiometerEvent, PotentiometerValueChangedEvent);
        intMachine->AddHandler("OPTOCOPLER", GetOptoEvent, OptoValueChangedEvent);
        floatMachine->AddHandler("LM35", GetLM35Event, LM35ValueChangedEvent);

        Serial.println("System initialized");
        delay(500);
    }

    String cmd;
    void Run()
    {
        if (Serial.available() > 0)
        {
            cmd = Serial.readString();
            cmd.trim();

            Serial.println(cmd);
        }

        intMachine->Update();
        //floatMachine->Update();

        //blink->Toggle();
        App::Run();
    }

private:
    int GetPotentiometerValue() { return map(potentiometer->Read(10), 0, 1023, 0, 255); }

    void PotentiometerValueChanged(int oldValue, int newValue)
    {
        if (abs(newValue - oldValue) > 5)
        {
            potLed->Write(newValue);
        }
    }

    int GetOptoValue() { return optoLed->Read() == LOW; }

    void OptoValueChanged(int oldValue, int newValue)
    {
        Serial.print("Optocopler changed from ");
        Serial.print(oldValue);
        Serial.print(" to ");
        Serial.println(newValue);
    }

    float GetLM35Value() { return float(lm35->Read(100)); }

    void LM35ValueChanged(float oldValue, float newValue)
    {
        if (abs(newValue - oldValue) > 3)
        {
            Serial.print("Temperature: ");
            Serial.print((float(newValue) * 5 / (1023)) / 0.01);
            Serial.println(" ºC");
        }
    }
};