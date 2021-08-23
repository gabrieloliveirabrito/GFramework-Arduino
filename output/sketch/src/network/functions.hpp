#line 1 "c:\\Users\\gabri\\Projetos\\Arduino\\GFramework\\src\\network\\functions.hpp"
#pragma once
#include <Ethernet.h>
#include <ArduinoJson.h>
#include "../../gframework/includes.h"

boolean SyncClock(DS3231 *clock)
{
    EthernetClient syncClient;
    if (syncClient.connect("worldtimeapi.org", 80))
    {
        Serial.println("Connected!");

        syncClient.print("GET /api/ip HTTP/1.1\r\n");
        syncClient.print("HOST: worldtimeapi.org\r\n");
        syncClient.print("Connection: Close\r\n");
        syncClient.print("\r\n");

        int timeout = millis();
        while (syncClient.connected() && !syncClient.available())
            delay(100);

        String response = "";
        while (syncClient.connected() && syncClient.available())
        {
            response += char(syncClient.read());
            if (response.endsWith("\r\n\r\n"))
            {
                response = "";
            }
        }

        if (response.length() == 0)
        {
            Serial.println("Empty response!");
            return false;
        }
        else
        {
            Serial.println(response);

            DynamicJsonDocument doc(2048);
            deserializeJson(doc, response);

            String dateTime = doc["datetime"].as<String>();
            Serial.println(dateTime);

            DateTime dt;
            dt.year = dateTime.substring(0, 4).toInt();
            dt.mon = dateTime.substring(5, 7).toInt();
            dt.mday = dateTime.substring(8, 10).toInt();
            dt.hour = dateTime.substring(11, 13).toInt();
            dt.min = dateTime.substring(14, 16).toInt();
            dt.sec = dateTime.substring(17, 19).toInt();

            doc.garbageCollect();
            clock->Set(dt);
            Serial.println("Time sync!");
        }

        syncClient.stop();
        return true;
    }
    else
    {
        Serial.println("Failed to connect to worldtimeapi.org");
        return false;
    }
}