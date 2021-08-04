#line 1 "c:\\Users\\gabri\\Projetos\\Arduino\\GFramework\\gframework\\components\\DHT.hpp"
//Based on https://github.com/adafruit/DHT-sensor-library/
#pragma once
#include "Pin.hpp"

#define MIN_INTERVAL 2000 /**< min interval value */
#define TIMEOUT UINT32_MAX

class DHT : public Pin
{
private:
    const uint8_t usec = 55;
    DHTType type;
    DHTMode mode;

    uint8_t data[5], pullTime;
#ifdef __AVR
    uint8_t _bit, _port;
#endif
    bool _lastresult = false;
    uint32_t _lastreadtime, _maxcycles;

public:
    DHT(int pin, DHTType type) : Pin(pin, PinType::ANALOG, PinMode::IN_PULLUP)
    {
        this->type = type;
        this->mode = DHTMode::Celcius;
    }

    DHT(int pin, DHTType type, DHTMode mode) : Pin(pin, PinType::ANALOG, PinMode::IN_PULLUP)
    {
        this->type = type;
        this->mode = mode;
    }

    void Initialize()
    {
#ifdef __AVR
        _bit = digitalPinToBitMask(GetNumber());
        _port = digitalPinToPort(GetNumber());
#endif
        _maxcycles = microsecondsToClockCycles(1000);
        _lastreadtime = millis() - MIN_INTERVAL;
        pullTime = usec;

        Pin::Initialize();
    }

    float ReadTemperature(bool force)
    {
        float f = NAN;

        if (Read(force))
        {
            switch (type)
            {
            case DHT11:
                f = data[2];
                if (data[3] & 0x80)
                {
                    f = -1 - f;
                }
                f += (data[3] & 0x0f) * 0.1;
                if (mode == Farenheit)
                {
                    f = CelciusToFarenheit(f);
                }
                break;
            case DHT12:
                f = data[2];
                f += (data[3] & 0x0f) * 0.1;
                if (data[2] & 0x80)
                {
                    f *= -1;
                }

                if (mode == Farenheit)
                {
                    f = CelciusToFarenheit(f);
                }
                break;
            case DHT22:
            case DHT21:
                f = ((word)(data[2] & 0x7F)) << 8 | data[3];
                f *= 0.1;
                if (data[2] & 0x80)
                {
                    f *= -1;
                }
                if (mode == Farenheit)
                {
                    f = CelciusToFarenheit(f);
                }
                break;
            }
        }
        return f;
    }

    float ReadHumidity(bool force)
    {
        float f = NAN;
        if (Read(force))
        {
            switch (type)
            {
            case DHT11:
            case DHT12:
                f = data[0] + data[1] * 0.1;
                break;
            case DHT22:
            case DHT21:
                f = ((word)data[0]) << 8 | data[1];
                f *= 0.1;
                break;
            }
        }
        return f;
    }

private:
    uint32_t ExpectPulse(bool level)
    {
#if (F_CPU > 16000000L)
        uint32_t count = 0;
#else
        uint16_t count = 0;
#endif

#ifdef __AVR
        uint8_t portState = level ? _bit : 0;
        while ((*portInputRegister(_port) & _bit) == portState)
        {
            if (count++ >= _maxcycles)
            {
                return TIMEOUT;
            }
        }

#else
        while (Read() == level)
        {
            if (count++ >= _maxcycles)
            {
                return TIMEOUT; // Exceeded timeout, fail.
            }
        }
#endif

        return count;
    }

    float Read(bool force)
    {
        uint32_t currenttime = millis();
        if (!force && ((currenttime - _lastreadtime) < MIN_INTERVAL))
        {
            return _lastresult; // return last correct measurement
        }
        _lastreadtime = currenttime;

        data[0] = data[1] = data[2] = data[3] = data[4] = 0;

#if defined(ESP8266)
        yield();
#endif
        SetMode(PinMode::IN_PULLUP);
        delay(1);

        SetMode(PinMode::OUT);
        Write(LOW);

        switch (type)
        {
        case DHTType::DHT22:
        case DHTType::DHT21:
            delayMicroseconds(1100);
            break;
        case DHTType::DHT11:
        default:
            delay(20);
            break;
        }

        uint32_t cycles[80];
        SetMode(PinMode::IN_PULLUP);
        delayMicroseconds(pullTime);

        InterruptLock lock;

        // First expect a low signal for ~80 microseconds followed by a high signal
        // for ~80 microseconds again.
        if (ExpectPulse(LOW) == TIMEOUT)
        {
            _lastresult = false;
            return _lastresult;
        }

        if (ExpectPulse(HIGH) == TIMEOUT)
        {
            _lastresult = false;
            return _lastresult;
        }

        for (int i = 0; i < 80; i += 2)
        {
            cycles[i] = ExpectPulse(LOW);
            cycles[i + 1] = ExpectPulse(HIGH);
        }

        for (int i = 0; i < 40; ++i)
        {
            uint32_t lowCycles = cycles[2 * i];
            uint32_t highCycles = cycles[2 * i + 1];
            if ((lowCycles == TIMEOUT) || (highCycles == TIMEOUT))
            {
                _lastresult = false;
                return _lastresult;
            }
            data[i / 8] <<= 1;

            if (highCycles > lowCycles)
            {
                data[i / 8] |= 1;
            }
        }

        if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF))
        {
            _lastresult = true;
            return _lastresult;
        }
        else
        {
            _lastresult = false;
            return _lastresult;
        }
    }

    float ComputeHeadIndex()
    {
        float hi = ComputeHeadIndex(ReadTemperature(mode == Farenheit), ReadHumidity(false));
        return hi;
    }

    float ComputeHeadIndex(float temperature, float percentHumidity)
    {
        float hi;

        if (mode == DHTMode::Farenheit)
            temperature = CelciusToFarenheit(temperature);

        hi = 0.5 * (temperature + 61.0 + ((temperature - 68.0) * 1.2) +
                    (percentHumidity * 0.094));

        if (hi > 79)
        {
            hi = -42.379 + 2.04901523 * temperature + 10.14333127 * percentHumidity +
                 -0.22475541 * temperature * percentHumidity +
                 -0.00683783 * pow(temperature, 2) +
                 -0.05481717 * pow(percentHumidity, 2) +
                 0.00122874 * pow(temperature, 2) * percentHumidity +
                 0.00085282 * temperature * pow(percentHumidity, 2) +
                 -0.00000199 * pow(temperature, 2) * pow(percentHumidity, 2);

            if ((percentHumidity < 13) && (temperature >= 80.0) &&
                (temperature <= 112.0))
                hi -= ((13.0 - percentHumidity) * 0.25) *
                      sqrt((17.0 - abs(temperature - 95.0)) * 0.05882);

            else if ((percentHumidity > 85.0) && (temperature >= 80.0) &&
                     (temperature <= 87.0))
                hi += ((percentHumidity - 85.0) * 0.1) * ((87.0 - temperature) * 0.2);
        }

        return mode == DHTMode::Farenheit ? hi : FarenheitToCelcius(hi);
    }

    float FarenheitToCelcius(float value) { return value * 1.8 + 32; }
    float CelciusToFarenheit(float value) { return (value - 32) * 0.55555; }
};