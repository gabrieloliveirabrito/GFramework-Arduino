#line 1 "c:\\Users\\gabri\\Projetos\\Arduino\\GFramework\\src\\LCD.hpp"
#pragma once
#include <LiquidCrystal.h>
#include "../gframework/includes.h"
#include "pins.h"
#include "LCDIcons.hpp"

//#define CUSTOM_DELAY

class LCD : public Component
{
private:
    DateTime lastDT;

    void Delay(int n)
    {
#ifdef CUSTOM_DELAY
        delay(n);
#else
        delay(100);
#endif
    }

public:
    LCD()
    {
        Serial.println("Creating LCD");
        lcd = new LiquidCrystal(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
        contrastPin = new Pin(LCD_CON, PinType::ANALOG, PinMode::OUT);
    }

    void Initialize()
    {
        Serial.println("Initializing LCD");

        contrastPin->Initialize();

        lcd->begin(16, 2);
        lcd->createChar(ICON_TEMPERATURE, CHAR_TEMPERATURE);
        lcd->createChar(ICON_HUMIDITY, CHAR_HUMIDITY);

        lcd->clear();
        lcd->display();

        contrastPin->Write(30);
        this->Delay(50);
    }

    void Clear()
    {
        lcd->clear();
    }

    void ShowInitializing()
    {
        lcd->clear();

        lcd->print("===ATX Panel===");
        lcd->setCursor(0, 1);
        lcd->print("Initializing....");

        this->Delay(2000);
    }

    void ShowSDCardInit()
    {
        lcd->clear();

        lcd->print("Initializing");
        lcd->setCursor(0, 1);
        lcd->print("SD Card");

        this->Delay(500);
    }

    void ShowSDCardFailed()
    {
        lcd->clear();

        lcd->print("Failed to load");
        lcd->setCursor(0, 1);
        lcd->print("SD Card");

        this->Delay(1500);
    }

    void ShowSDCardPreConfig()
    {
        lcd->clear();

        lcd->print("SD Card Loaded");
        lcd->setCursor(0, 1);
        lcd->print("Loading config..");

        this->Delay(500);
    }

    void ShowFetchingDHCP()
    {
        lcd->clear();
        lcd->print("Fetching DHCP...");
    }

    void ShowNoEthernetHadware()
    {
        lcd->clear();
        lcd->print("No Ethernet");
        lcd->setCursor(0, 1);
        lcd->print("Shield installed");
        this->Delay(1500);
    }

    void ShowNoEthernetCable()
    {
        lcd->clear();
        lcd->print("No Ethernet");
        lcd->setCursor(0, 1);
        lcd->print("Cable plugged");
        this->Delay(1500);
    }

    void ShowDHCPFailed()
    {
        lcd->clear();
        lcd->print("Failed to fetch");
        lcd->setCursor(0, 1);
        lcd->print("IP from DHCP");
        this->Delay(1500);
    }

    void ShowNoEthernet()
    {
        lcd->clear();
        lcd->print("The Ethernet mod");
        lcd->setCursor(0, 1);
        lcd->print("not enabled now");
        this->Delay(1500);
    }

    void ShowEthernetIP(IPAddress ip)
    {
        lcd->setCursor(0, 1);
        lcd->print(ip);
        this->Delay(1500);
    }

    void ShowStartingServer()
    {
        lcd->clear();
        lcd->print("Starting ");
        lcd->setCursor(0, 1);
        lcd->print("TCP Server");
        this->Delay(1000);
    }

    void ShowSyncing()
    {
        lcd->clear();

        lcd->setCursor(0, 0);
        lcd->print("===ATX Panel===");
        lcd->setCursor(0, 1);
        lcd->print("Sync clock....");
    }

    void ShowInitialized()
    {
        lcd->clear();

        lcd->setCursor(0, 0);
        lcd->print("PC");
    }

    void ShowClock(DateTime dt)
    {
        lcd->setCursor(0, 1);
        if (lastDT.hour != dt.hour)
        {
            if (dt.hour < 10)
                lcd->print("0");
            lcd->print(dt.hour);
        }

        lcd->setCursor(3, 1);
        if (lastDT.min != dt.min)
        {
            if (dt.min < 10)
                lcd->print("0");
            lcd->print(dt.min);
        }

        /*lcd->setCursor(6, 1);
        if (lastDT.sec != dt.sec)
        {
            if (dt.sec < 10)
                lcd->print("0");
            lcd->print(dt.sec);
        }*/
    }

    void ShowClockError()
    {
        lcd->setCursor(0, 1);
        lcd->print("Clock Mod. Error");
    }

    void ShowPCAlreadyOn()
    {
        lcd->clear();
        lcd->print("Computer");

        lcd->setCursor(0, 1);
        lcd->print("Already on!");
        this->Delay(2000);
    }

    void ShowPCPoweringOn()
    {
        lcd->clear();
        lcd->print("Computer");

        lcd->setCursor(0, 1);
        lcd->print("Starting...");
    }

    void ShowPCPoweredOn()
    {
        lcd->clear();
        lcd->print("Computer");
        lcd->setCursor(0, 1);
        lcd->print("Powered on!");
        this->Delay(2000);
    }

    void ShowPCAlreadyOff()
    {
        lcd->clear();
        lcd->print("Computer");
        lcd->setCursor(0, 1);
        lcd->print("Already on!");
        this->Delay(2000);
    }

    void ShowPCPoweringOff()
    {
        lcd->clear();
        lcd->print("Computer");
        lcd->setCursor(0, 1);
        lcd->print("Shutdown...");
    }

    void ShowPCPoweredOff()
    {
        lcd->clear();
        lcd->print("Computer");
        lcd->setCursor(0, 1);
        lcd->print("Powered off!");
        this->Delay(2000);
        lcd->clear();
    }

    void ShowTemperature(float temp)
    {
        lcd->setCursor(10, 0);
        lcd->write(ICON_TEMPERATURE);
        lcd->print(' ');
        lcd->print(temp, 0);
        lcd->print((char)223);
        lcd->print('C');
    }

    void ShowHumidity(float humidity)
    {
        lcd->setCursor(11, 1);
        lcd->write(ICON_HUMIDITY);
        lcd->print(' ');
        lcd->print(humidity, 0);
        lcd->print('%');
    }

    void ShowLedState(int pwr, int hdd)
    {
        lcd->clear();
        lcd->print("PC");
        lcd->setCursor(3, 0);
        lcd->print(pwr == LOW ? "OFF" : "ON");

        if (pwr == HIGH && hdd == HIGH)
        {
            lcd->setCursor(5, 0);
            lcd->print("HDD");
        }
    }

private:
    LiquidCrystal *lcd;
    Pin *contrastPin;
};