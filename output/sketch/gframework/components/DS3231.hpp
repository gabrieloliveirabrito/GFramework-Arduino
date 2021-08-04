#line 1 "c:\\Users\\gabri\\Projetos\\Arduino\\GFramework\\gframework\\components\\DS3231.hpp"
//This code has based on Rodan Library: https://github.com/rodan/ds3231/blob/master/
#pragma once
#include <Wire.h>
#include "DS3231Config.h"
#include "../bases/includes.h"

class DS3231 : public Component
{
public:
    void Initialize()
    {
        Serial.println("Initializing DS3231 component");

        Wire.begin();
        WriteControlRegister(DS3231_CONTROL_INTCN);
        Write32kHzOutput(false);
    }

    void Initialize(uint8_t ctrl_reg)
    {
        Serial.println("Initializing DS3231 component");

        Wire.begin();
        WriteControlRegister(ctrl_reg);
        Write32kHzOutput(false);

        Component::Initialize();
    }

    void Set(struct DateTime t)
    {
        uint8_t century;

        if (t.year >= 2000)
        {
            century = 0x80;
            t.year_s = t.year - 2000;
        }
        else
        {
            century = 0;
            t.year_s = t.year - 1900;
        }

        uint8_t TimeDate[7] = {t.sec, t.min, t.hour, t.wday, t.mday, t.mon, t.year_s};

        Wire.beginTransmission(DS3231_I2C_ADDR);
        Wire.write(DS3231_TIME_CAL_ADDR);
        for (int i = 0; i <= 6; i++)
        {
            TimeDate[i] = dectobcd(TimeDate[i]);
            if (i == 5)
                TimeDate[5] += century;
            Wire.write(TimeDate[i]);
        }
        Wire.endTransmission();
    }

    bool Get(DateTime *t)
    {
        uint8_t TimeDate[7];
        uint8_t century = 0;
        uint8_t i, n;
        uint16_t year_full;

        Wire.beginTransmission(DS3231_I2C_ADDR);
        Wire.write(DS3231_TIME_CAL_ADDR);
        Wire.endTransmission();

        uint8_t gotData = false;
        uint32_t start = millis(); // start timeout
        while (millis() - start < DS3231_TRANSACTION_TIMEOUT)
        {
            if (Wire.requestFrom(DS3231_I2C_ADDR, 7) == 7)
            {
                gotData = true;
                break;
            }
            delay(2);
        }
        if (!gotData)
            return false;

        for (i = 0; i <= 6; i++)
        {
            n = Wire.read();
            if (i == 5)
            {
                TimeDate[5] = bcdtodec(n & 0x1F);
                century = (n & 0x80) >> 7;
            }
            else
                TimeDate[i] = bcdtodec(n);
        }

        if (century == 1)
        {
            year_full = 2000 + TimeDate[6];
        }
        else
        {
            year_full = 1900 + TimeDate[6];
        }

        t->sec = TimeDate[0];
        t->min = TimeDate[1];
        t->hour = TimeDate[2];
        t->mday = TimeDate[4];
        t->mon = TimeDate[5];
        t->year = year_full;
        t->wday = TimeDate[3];
        t->year_s = TimeDate[6];
#ifdef CONFIG_UNIXTIME
        t->unixtime = get_unixtime(*t);
#endif

        return true;
    }

    uint8_t ReadControlRegister()
    {
        return ReadAddr(DS3231_CONTROL_ADDR);
    }

    void WriteControlRegister(const uint8_t val)
    {
        WriteAddr(DS3231_CONTROL_ADDR, val);
    }

    uint8_t ReadStatusRegister()
    {
        return ReadAddr(DS3231_STATUS_ADDR);
    }

    void WriteStatusRegister(const uint8_t val)
    {
        WriteAddr(DS3231_STATUS_ADDR, val);
    }

    int8_t GetAging()
    {
        uint8_t reg = ReadAddr(DS3231_AGING_OFFSET_ADDR);

        return (reg & 0x80) == 0 ? reg : reg | ~((1 << 8) - 1);
    }

    void SetAging(int8_t val)
    {
        //reg < 0 - 2C
        uint8_t reg = val >= 0 ? val : ~(-val) + 1;
        /*
        * At 25°C the aging change of:
        * +1 means -0.1ppm
        * -1 means -0.1ppm
        */
        WriteAddr(DS3231_AGING_OFFSET_ADDR, reg);
        // A conversion mut be done to forace the new aging value.
        WriteControlRegister(ReadControlRegister() | DS3231_CONTROL_CONV);
    }

    float ReadTemperatureRegister()
    {
        Wire.beginTransmission(DS3231_I2C_ADDR);
        Wire.write(DS3231_TEMPERATURE_ADDR);
        Wire.endTransmission();

        uint8_t gotData = false;
        uint32_t start = millis(); // start timeout
        while (millis() - start < DS3231_TRANSACTION_TIMEOUT)
        {
            if (Wire.requestFrom(DS3231_I2C_ADDR, 2) == 2)
            {
                gotData = true;
                break;
            }
            delay(2);
        }
        if (!gotData)
            return 0; // error timeout

        uint8_t temp_msb = Wire.read();
        uint8_t temp_lsb = Wire.read() >> 6;

        int8_t nint;
        if ((temp_msb & 0x80) != 0)
            nint = temp_msb | ~((1 << 8) - 1); // if negative get two's complement
        else
            nint = temp_msb;

        return 0.25 * temp_lsb + nint;
    }

    void Write32kHzOutput(const uint8_t on)
    {
        uint8_t sreg = ReadStatusRegister();
        if (on)
            sreg &= ~DS3231_STATUS_OSF;
        sreg |= DS3231_STATUS_EN32KHZ;
        WriteStatusRegister(sreg);
    }

    void SetAlarm1(const uint8_t s, const uint8_t mi, const uint8_t h, const uint8_t d, const uint8_t *flags)
    {
        uint8_t t[4] = {s, mi, h, d};

        Wire.beginTransmission(DS3231_I2C_ADDR);
        Wire.write(DS3231_ALARM1_ADDR);

        for (int i = 0; i <= 3; i++)
        {
            if (i == 3)
            {
                Wire.write(dectobcd(t[3]) | (flags[3] << 7) | (flags[4] << 6));
            }
            else
                Wire.write(dectobcd(t[i]) | (flags[i] << 7));
        }

        Wire.endTransmission();
    }

    void GetAlarm1(char *buf, const uint8_t len)
    {
        uint8_t n[4];
        uint8_t t[4]; //second,minute,hour,day
        uint8_t f[5]; // flags
        uint8_t i;

        Wire.beginTransmission(DS3231_I2C_ADDR);
        Wire.write(DS3231_ALARM1_ADDR);
        Wire.endTransmission();

        uint8_t gotData = false;
        uint32_t start = millis(); // start timeout
        while (millis() - start < DS3231_TRANSACTION_TIMEOUT)
        {
            if (Wire.requestFrom(DS3231_I2C_ADDR, 4) == 4)
            {
                gotData = true;
                break;
            }
            delay(2);
        }
        if (!gotData)
            return; // error timeout

        for (i = 0; i <= 3; i++)
        {
            n[i] = Wire.read();
            f[i] = (n[i] & 0x80) >> 7;
            t[i] = bcdtodec(n[i] & 0x7F);
        }

        f[4] = (n[3] & 0x40) >> 6;
        t[3] = bcdtodec(n[3] & 0x3F);

        snprintf(buf, len,
                 "s%02d m%02d h%02d d%02d fs%d m%d h%d d%d wm%d %d %d %d %d",
                 t[0], t[1], t[2], t[3], f[0], f[1], f[2], f[3], f[4], n[0],
                 n[1], n[2], n[3]);
    }

    void ClearAlarm1()
    {
        uint8_t reg_val = ReadStatusRegister() & ~DS3231_STATUS_A1F;
        WriteStatusRegister(reg_val);
    }

    bool IsAlarm1Triggered()
    {
        return ReadStatusRegister() & DS3231_STATUS_A1F;
    }

    void SetAlarm2(const uint8_t s, const uint8_t mi, const uint8_t h, const uint8_t d, const uint8_t *flags)
    {
        uint8_t t[4] = {s, mi, h, d};

        Wire.beginTransmission(DS3231_I2C_ADDR);
        Wire.write(DS3231_ALARM2_ADDR);

        for (int i = 0; i <= 3; i++)
        {
            if (i == 3)
            {
                Wire.write(dectobcd(t[3]) | (flags[3] << 7) | (flags[4] << 6));
            }
            else
                Wire.write(dectobcd(t[i]) | (flags[i] << 7));
        }

        Wire.endTransmission();
    }

    void GetAlarm2(char *buf, const uint8_t len)
    {
        uint8_t n[3];
        uint8_t t[3]; //second,minute,hour,day
        uint8_t f[4]; // flags
        uint8_t i;

        Wire.beginTransmission(DS3231_I2C_ADDR);
        Wire.write(DS3231_ALARM2_ADDR);
        Wire.endTransmission();

        Wire.requestFrom(DS3231_I2C_ADDR, 4);
        uint8_t gotData = false;
        uint32_t start = millis(); // start timeout
        while (millis() - start < DS3231_TRANSACTION_TIMEOUT)
        {
            if (Wire.requestFrom(DS3231_I2C_ADDR, 3) == 3)
            {
                gotData = true;
                break;
            }
            delay(2);
        }
        if (!gotData)
            return; // error timeout

        for (i = 0; i <= 2; i++)
        {
            n[i] = Wire.read();
            f[i] = (n[i] & 0x80) >> 7;
            t[i] = bcdtodec(n[i] & 0x7F);
        }

        f[3] = (n[2] & 0x40) >> 6;
        t[2] = bcdtodec(n[2] & 0x3F);

        snprintf(buf, len, "m%02d h%02d d%02d fm%d h%d d%d wm%d %d %d %d", t[0],
                 t[1], t[2], f[0], f[1], f[2], f[3], n[0], n[1], n[2]);
    }

    void ClearAlarm2()
    {
        uint8_t reg_val = ReadStatusRegister() & ~DS3231_STATUS_A2F;
        WriteStatusRegister(reg_val);
    }

    bool IsAlarm2Triggered()
    {
        return ReadStatusRegister() & DS3231_STATUS_A2F;
    }

private:
    uint8_t ReadAddr(const uint8_t addr)
    {
        Wire.beginTransmission(DS3231_I2C_ADDR);
        Wire.write(addr);
        Wire.endTransmission();

        uint8_t gotData = false;
        uint32_t start = millis(); // start timeout
        while (millis() - start < DS3231_TRANSACTION_TIMEOUT)
        {
            if (Wire.requestFrom(DS3231_I2C_ADDR, 1) == 1)
            {
                gotData = true;
                break;
            }
            delay(2);
        }

        if (!gotData)
            return 0; // error timeout

        return Wire.read();
    }

    void WriteAddr(const uint8_t addr, const uint8_t val)
    {
        Wire.beginTransmission(DS3231_I2C_ADDR);
        Wire.write(addr);
        Wire.write(val);
        Wire.endTransmission();
    }
};