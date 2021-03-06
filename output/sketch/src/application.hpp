#line 1 "c:\\Users\\gabri\\Projetos\\Arduino\\GFramework\\src\\application.hpp"
#pragma once
#include <TimeLib.h>

#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoJson.h>

#include "../gframework/includes.h"
#include "network/includes.h"

#include "Configuration.hpp"
#include "LCD.hpp"
#include "pins.h"
#include "defines.h"

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

class Application : App
{
private:
    Configuration *configuration;
    LCD *lcd;
    LED *blink, *powerLed, *hddLed, *errorLed;
    Pin *moboPwrLed, *moboPwrBtn, *moboHddLed, *moboResetBtn;
    DS3231 *clock;
    DHT *dht;

    TCPServer<Application> *server;

    StateMachine<Application, int> *intMachine;
    EventHandler<Application, int> *GetPowerLedEvent, *GetHDDLedEvent;
    EventHandler<Application, void, int, int> *PowerLedChangedEvent, *HDDLedChangedEvent;

    StateMachine<Application, float> *floatMachine;
    EventHandler<Application, float> *GetDHTTemperatureEvent, *GetDHTHumidityEvent;
    EventHandler<Application, void, float, float> *DHTTemperatureChangedEvent, *DHTHumidityChangedEvent;

public:
    void Setup()
    {
        InitSerial(115200);
        Serial.println("Creating components...");

        configuration = RegisterComponent(new Configuration());
        lcd = RegisterComponent(new LCD());
        clock = RegisterComponent(new DS3231());
        blink = RegisterComponent(new LED(LED_BUILTIN));
        powerLed = RegisterComponent(new LED(POWER_LED));
        hddLed = RegisterComponent(new LED(HDD_LED));
        errorLed = RegisterComponent(new LED(BEEP_LED));
        moboPwrBtn = RegisterComponent(new Pin(MOBO_PWR_BTN, PinType::DIGITAL, PinMode::OUT, RELEASED));
        moboPwrLed = RegisterComponent(new Pin(MOBO_PWR_LED, PinType::DIGITAL, PinMode::IN_PULLUP));
        moboHddLed = RegisterComponent(new Pin(MOBO_HDD_LED, PinType::DIGITAL, PinMode::IN_PULLUP));
        moboResetBtn = RegisterComponent(new Pin(MOBO_RES_BTN, PinType::DIGITAL, PinMode::OUT, RELEASED));
        dht = RegisterComponent(new DHT(A15, DHTType::DHT11));

        intMachine = new StateMachine<Application, int>();
        floatMachine = new StateMachine<Application, float>();

        CreateEvents();
        App::Setup();
    }

    void CreateEvents()
    {
        GetPowerLedEvent = new EventHandler<Application, int>(this, &Application::GetPowerLed);
        PowerLedChangedEvent = new EventHandler<Application, void, int, int>(this, &Application::PowerLedChanged);
        GetHDDLedEvent = new EventHandler<Application, int>(this, &Application::GetHDDLed);
        HDDLedChangedEvent = new EventHandler<Application, void, int, int>(this, &Application::HDDLedChanged);
        GetDHTTemperatureEvent = new EventHandler<Application, float>(this, &Application::GetDHTTemperature);
        DHTTemperatureChangedEvent = new EventHandler<Application, void, float, float>(this, &Application::DHTTemperatureChanged);
        GetDHTHumidityEvent = new EventHandler<Application, float>(this, &Application::GetDHTHumidity);
        DHTHumidityChangedEvent = new EventHandler<Application, void, float, float>(this, &Application::DHTHumidityChanged);
    }

    void Initialize()
    {
        Serial.println("Initializing...");
        App::Initialize();

        lcd->ShowInitializing();
        blink->Enable();

#ifdef WITH_SDCARD
        lcd->ShowSDCardInit();

        Serial.println("Loading configuration...");
        if (configuration->Load())
        {
            Serial.println("Configuration loaded successfully!");
            lcd->ShowSDCardPreConfig();
            //TODO: SDCardPreConfig();
        }
        else
        {
            Serial.println("Failed to load the configuration!");
            configuration->Save();
            lcd->ShowSDCardFailed();
        }
#endif

#ifdef WITH_ETHERNET

        bool netInitResult = false;
        if (configuration->ip == NULL)
        {
            Serial.println("Initialize Ethernet with DHCP:");
            lcd->ShowFetchingDHCP();

            netInitResult = Ethernet.begin(mac) == 0;
        }
        else
        {
            if (configuration->dns != NULL && configuration->gateway == NULL)
            {
                Ethernet.begin(mac, *configuration->ip, *configuration->dns);
            }
            else if (configuration->dns != NULL && configuration->gateway != NULL && configuration->subnet == NULL)
            {
                Ethernet.begin(mac, *configuration->ip, *configuration->dns, *configuration->gateway);
            }
            else if (configuration->dns != NULL && configuration->gateway != NULL && configuration->subnet == NULL)
            {
                Ethernet.begin(mac, *configuration->ip, *configuration->dns, *configuration->gateway, *configuration->subnet);
            }
            else
            {
                Ethernet.begin(mac, *configuration->ip);
            }

            netInitResult = true;
        }

        if (!netInitResult)
        {
            Serial.println("Failed to configure Ethernet");

            if (Ethernet.hardwareStatus() == EthernetNoHardware)
            {
                Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
                lcd->ShowNoEthernetHadware();
            }
            else if (Ethernet.linkStatus() == LinkOFF)
            {
                Serial.println("Ethernet cable is not connected.");
                lcd->ShowNoEthernetCable();
            }
            else
                lcd->ShowNoEthernet();
        }
        else
        {
            Serial.print("IP: ");
            Serial.println(Ethernet.localIP());

            Serial.print("Subnet: ");
            Serial.println(Ethernet.subnetMask());

            Serial.print("Gateway: ");
            Serial.println(Ethernet.gatewayIP());

            Serial.print("DNS: ");
            Serial.println(Ethernet.dnsServerIP());

            lcd->ShowEthernetIP(Ethernet.localIP());
            lcd->ShowStartingServer();

            server = new TCPServer<Application>(configuration->port);
            server->Start();

            lcd->ShowSyncing();
            if (SyncClock(clock))
                Serial.println("Clock synced!");
            else
                Serial.println("Failed to sync clock!");
        }
#endif

        Serial.println("System initialized");
        lcd->ShowInitialized();

        intMachine->AddHandler("POWER_LED", GetPowerLedEvent, PowerLedChangedEvent);
        intMachine->AddHandler("HDD_LED", GetHDDLedEvent, HDDLedChangedEvent);

        floatMachine->AddHandler("DHT_TEMPERATURE", GetDHTTemperatureEvent, DHTTemperatureChangedEvent);
        floatMachine->AddHandler("DHT_HUMIDITY", GetDHTHumidityEvent, DHTHumidityChangedEvent);
        delay(500);
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
            if (cmd == "P_READ")
                Serial.println(GetPowerLed() ? "ON" : "OFF");
        }

        intMachine->Update();
        floatMachine->Update();

        DateTime dt;
        if (clock->Get(&dt))
        {
            lcd->ShowClock(dt);
        }
        else
        {
            lcd->ShowClockError();
        };

        //blink->Toggle();
        App::Run();
    }

    void PowerOn()
    {
        //lcd.clear();
        if (GetPowerLed() == HIGH)
        {
            lcd->ShowPCAlreadyOn();
            Serial.println("Already powered on!");
        }
        else
        {
            lcd->ShowPCPoweringOn();

            Serial.print("Powering on...");
            moboPwrBtn->Write(PRESSED);
            while (!GetPowerLed())
            {
                Serial.print(GetPowerLed());
                delay(50);
            }
            moboPwrBtn->Write(RELEASED);
            Serial.println("Powered on!");

            lcd->ShowPCPoweredOn();
        }

        lcd->ShowInitialized();
        intMachine->TriggerChanged();
    }

    void PowerOff()
    {
        //lcd.clear();
        if (GetPowerLed() == LOW)
        {
            Serial.println("Already powered off!");
            lcd->ShowPCAlreadyOff();
        }
        else
        {
            lcd->ShowPCPoweringOff();

            Serial.println("Powering off...");
            moboPwrBtn->Write(PRESSED);
            Serial.print("Please wait...");
            while (GetPowerLed())
            {
                Serial.print(GetPowerLed());
                delay(50);
            }
            moboPwrBtn->Write(RELEASED);
            Serial.println("Powered off");

            lcd->ShowPCPoweredOff();
        }

        lcd->ShowInitialized();
        intMachine->TriggerChanged();
    }

private:
    int GetPowerLed()
    {
        //Serial.println("Getting power led value");
        return moboPwrLed->Read() == LOW;
    }

    void PowerLedChanged(int oldValue, int newValue)
    {
        Serial.print("Power Led from ");
        Serial.print(oldValue);
        Serial.print(" to ");
        Serial.println(newValue);

        if (newValue == HIGH)
            powerLed->Enable();
        else
            powerLed->Disable();

        lcd->ShowLedState(newValue, moboHddLed->Read() == LOW);
    }

    int GetHDDLed()
    {
        //Serial.println("Getting hdd led value");
        return moboHddLed->Read() == LOW;
    }

    void HDDLedChanged(int oldValue, int newValue)
    {
        Serial.print("HDD Led from ");
        Serial.print(oldValue);
        Serial.print(" to ");
        Serial.println(newValue);

        if (newValue == HIGH)
            hddLed->Enable();
        else
            hddLed->Disable();

        lcd->ShowLedState(moboPwrLed->Read() == LOW, newValue);
    }

    float GetDHTTemperature() { return dht->ReadTemperature(false); }
    void DHTTemperatureChanged(float oldValue, float newValue) { lcd->ShowTemperature(newValue); }

    float GetDHTHumidity() { return dht->ReadHumidity(false); }
    void DHTHumidityChanged(float oldValue, float newValue) { lcd->ShowHumidity(newValue); }
};