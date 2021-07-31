#line 1 "c:\\Users\\gabri\\Projetos\\Arduino\\GFramework\\src\\application.hpp"
#pragma once
#include <LiquidCrystal.h>

#include "../gframework/includes.h"
#include "pins.h"
#include "defines.h"

class Application : App
{
private:
    LiquidCrystal *lcd;
    LED *blink, *powerLed, *hddLed, *errorLed;
    StateMachine<Application, int> *intMachine;
    DS3231 *clock;

public:
    void Initialize()
    {
        InitSerial();
        lcd = new LiquidCrystal(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
        lcd->begin(16, 2);
        lcd->clear();

        pinMode(LCD_CON, OUTPUT);
        digitalWrite(LCD_CON, 0);

        Serial.println("Initializing...");

        lcd->setCursor(0, 0);
        lcd->write("===ATX Panel===");
        lcd->setCursor(0, 1);
        lcd->write("Initializing....");

        blink = RegisterComponent(new LED(LED_BUILTIN));
        powerLed = RegisterComponent(new LED(POWER_LED));
        hddLed = RegisterComponent(new LED(HDD_LED));
        errorLed = RegisterComponent(new LED(BEEP_LED));
        clock = RegisterComponent(new DS3231());
        clock->Initialize();

        blink->Enable();

        pinMode(MOBO_PWR_BTN, OUTPUT);
        pinMode(MOBO_PWR_LED, INPUT_PULLUP);
        pinMode(MOBO_HDD_LED, INPUT_PULLUP);
        pinMode(MOBO_SPEAKER_HIGH, INPUT);
        digitalWrite(BEEP_LED, HIGH);
        digitalWrite(HDD_LED, HIGH);
        digitalWrite(MOBO_PWR_BTN, RELEASED);

        lcd->setCursor(0, 1);
        lcd->write("   Components   ");

        delay(250);
        digitalWrite(POWER_LED, LOW);
        digitalWrite(BEEP_LED, LOW);
        digitalWrite(HDD_LED, LOW);

        lcd->setCursor(0, 1);
        lcd->write("  St. Machines  ");

        intMachine = new StateMachine<Application, int>();
        intMachine->AddHandler("POWER_LED",
                               new EventHandler<Application, int>(this, &Application::GetPowerLedState),
                               new EventHandler<Application, void, int, int>(this, &Application::PowerLedStateChanged));

        App::Initialize();

        lcd->setCursor(0, 1);
        lcd->write("  Initialized!  ");
        delay(2000);
    }

    String cmd;
    void Run()
    {
        if (Serial.available() > 0)
        {
            cmd = Serial.readString();
            cmd.trim();

            if (cmd == "P_ON")
                PowerOn();
            if (cmd == "P_OFF")
                PowerOff();
        }

        intMachine->Update();

        DateTime dt;
        if (clock->Get(&dt))
        {
            lcd->setCursor(0, 1);
            char buf[8];
            sprintf(buf, "%d:%d:%d", dt.hour, dt.min, dt.sec);

            lcd->write(buf);
        }
        else
        {
            lcd->setCursor(0, 1);
            lcd->write("Clock Mod. Error");
        }

        blink->Toggle();
        App::Run();
    }

    void PowerOn()
    {
        //lcd.clear();
        if (digitalRead(MOBO_PWR_LED) == HIGH)
        {
            //lcd.print("Already ON!");
            Serial.print("Already powered on!");
            delay(100);
        }
        else
        {
            //lcd.print("Powering on...");

            Serial.print("Powering on...");
            digitalWrite(MOBO_PWR_BTN, PRESSED);
            while (digitalRead(MOBO_PWR_LED) == LOW)
                delay(50);
            digitalWrite(MOBO_PWR_BTN, RELEASED);
            Serial.println("Powered on!");

            //lcd.clear();
        }
    }

    void PowerOff()
    {
        //lcd.clear();
        if (digitalRead(MOBO_PWR_LED) == LOW)
        {
            //lcd.print("Already OFF!");
            Serial.print("Already powered off!");
            delay(1000);
        }
        else
        {
            //lcd.print("Powering off...");

            Serial.println("Powering off...");
            digitalWrite(MOBO_PWR_BTN, PRESSED);
            Serial.print("Please wait...");
            while (digitalRead(MOBO_PWR_LED) == HIGH)
                delay(50);
            digitalWrite(MOBO_PWR_BTN, RELEASED);
            Serial.println("Powered off");

            //lcd.clear();
        }
    }

private:
    int GetPowerLedState()
    {
        return digitalRead(MOBO_PWR_LED);
    }

    void PowerLedStateChanged(int oldValue, int newValue)
    {
        Serial.print("Power Led from ");
        Serial.print(oldValue);
        Serial.print(" to ");
        Serial.println(newValue);

        if (newValue == HIGH)
            blink->Enable();
        else
            blink->Disable();
    }
};