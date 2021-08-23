#pragma once
#include <SD.h>
#include <ArduinoJson.h>
#include "../gframework/includes.h"

class Configuration : public Component
{
public:
    IPAddress *ip, *dns, *subnet, *gateway;
    int port = 2020;

    void Initialize()
    {
        Component::Initialize();

        if (SD.begin(4))
        {
            initialized = true;
        }
    }

    bool IsInitialized() { return initialized; }

    bool Load()
    {
        bool result = false;

        if (!SD.exists(configLocation))
        {
            Serial.println("Configuration file not found!");
            return false;
        }

        File configFile = SD.open(configLocation);

        String json = "";
        while (configFile.available())
            json += (char)configFile.read();

        configFile.close();

        DynamicJsonDocument doc(2048);
        DeserializationError error = deserializeJson(doc, json);

        if (error == DeserializationError::Ok)
        {
            JsonObject root = doc.as<JsonObject>();
            if (!root.containsKey("network"))
                goto finally;

            JsonObject networkObj = root["network"].as<JsonObject>();
            if (networkObj.containsKey("client"))
            {
                JsonObject clientObj = networkObj["client"].as<JsonObject>();
                if (clientObj.containsKey("ip"))
                {
                    this->ip = new IPAddress();
                    if (!this->ip->fromString(clientObj["ip"].as<String>()))
                        goto finally;

                    if (clientObj.containsKey("dns"))
                    {
                        this->dns = new IPAddress();
                        if (!this->dns->fromString(clientObj["dns"].as<String>()))
                            goto finally;
                    }

                    if (clientObj.containsKey("subnet"))
                    {
                        this->subnet = new IPAddress();
                        if (!this->subnet->fromString(clientObj["subnet"].as<String>()))
                            goto finally;
                    }

                    if (clientObj.containsKey("gateway"))
                    {
                        this->gateway = new IPAddress();
                        if (!this->gateway->fromString(clientObj["gateway"].as<String>()))
                            goto finally;
                    }
                }
            }

            if (networkObj.containsKey("server"))
            {
                JsonObject serverObj = networkObj["server"].as<JsonObject>();
                if (serverObj.containsKey("port"))
                {
                    this->port = serverObj["port"].as<int>();
                    if (this->port == 0)
                        this->port = 2020;
                }
            }

            doc.garbageCollect();
            result = true;
            goto finally;
        }
        else
            goto finally;

    finally:
    {
        if (configFile)
            configFile.close();

        return result;
    }
    }

    bool Save()
    {
        DynamicJsonDocument doc(2048);
        if (this->ip != NULL)
        {
            String addr = String((*ip)[0]) + "." + String((*ip)[1]) + "." + String((*ip)[2]) + "." + String((*ip)[3]);
            doc["network"]["client"]["ip"] = addr;
        }

        doc["network"]["server"]["port"] = port;

        if (SD.exists(configLocation))
            SD.remove(configLocation);

        File jsonFile = SD.open(configLocation, FILE_WRITE);
        serializeJsonPretty(doc, jsonFile);
        jsonFile.close();

        return false;
    }

private:
    bool initialized = false;
    const String configLocation = "/config.cfg";
};