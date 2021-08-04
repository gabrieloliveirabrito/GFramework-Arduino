#line 1 "c:\\Users\\gabri\\Projetos\\Arduino\\GFramework\\gframework\\components\\DS3231Config.h"
#pragma once

#define DS3231_TRANSACTION_TIMEOUT 100 // Wire NAK/Busy timeout in ms

#define SECONDS_FROM_1970_TO_2000 946684800

// i2c slave address of the DS3231 chip
#define DS3231_I2C_ADDR 0x68

// timekeeping registers
#define DS3231_TIME_CAL_ADDR 0x00
#define DS3231_ALARM1_ADDR 0x07
#define DS3231_ALARM2_ADDR 0x0B
#define DS3231_CONTROL_ADDR 0x0E
#define DS3231_STATUS_ADDR 0x0F
#define DS3231_AGING_OFFSET_ADDR 0x10
#define DS3231_TEMPERATURE_ADDR 0x11

// control register bits
#define DS3231_CONTROL_A1IE 0x1   /* Alarm 2 Interrupt Enable */
#define DS3231_CONTROL_A2IE 0x2   /* Alarm 2 Interrupt Enable */
#define DS3231_CONTROL_INTCN 0x4  /* Interrupt Control */
#define DS3231_CONTROL_RS1 0x8    /* square-wave rate select 2 */
#define DS3231_CONTROL_RS2 0x10   /* square-wave rate select 2 */
#define DS3231_CONTROL_CONV 0x20  /* Convert Temperature */
#define DS3231_CONTROL_BBSQW 0x40 /* Battery-Backed Square-Wave Enable */
#define DS3231_CONTROL_EOSC 0x80  /* not Enable Oscillator, 0 equal on */

// status register bits
#define DS3231_STATUS_A1F 0x01     /* Alarm 1 Flag */
#define DS3231_STATUS_A2F 0x02     /* Alarm 2 Flag */
#define DS3231_STATUS_BUSY 0x04    /* device is busy executing TCXO */
#define DS3231_STATUS_EN32KHZ 0x08 /* Enable 32KHz Output  */
#define DS3231_STATUS_OSF 0x80     /* Oscillator Stop Flag */

struct DateTime
{
    uint8_t sec;    /* seconds */
    uint8_t min;    /* minutes */
    uint8_t hour;   /* hours */
    uint8_t mday;   /* day of the month */
    uint8_t mon;    /* month */
    int16_t year;   /* year */
    uint8_t wday;   /* day of the week */
    uint8_t yday;   /* day in the year */
    uint8_t isdst;  /* daylight saving time */
    uint8_t year_s; /* year in short notation*/
#ifdef CONFIG_UNIXTIME
    uint32_t unixtime; /* seconds since 01.01.1970 00:00:00 UTC*/
#endif
};

#ifdef CONFIG_UNIXTIME
const uint8_t days_in_month[12] PROGMEM = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

// returns the number of seconds since 01.01.1970 00:00:00 UTC, valid for 2000..FIXME
uint32_t get_unixtime(struct ts t)
{
    uint8_t i;
    uint16_t d;
    int16_t y;
    uint32_t rv;

    if (t.year >= 2000)
    {
        y = t.year - 2000;
    }
    else
    {
        return 0;
    }

    d = t.mday - 1;
    for (i = 1; i < t.mon; i++)
    {
        d += xpgm_read_byte(days_in_month + i - 1);
    }
    if (t.mon > 2 && y % 4 == 0)
    {
        d++;
    }
    // count leap days
    d += (365 * y + (y + 3) / 4);
    rv = ((d * 24UL + t.hour) * 60 + t.min) * 60 + t.sec + SECONDS_FROM_1970_TO_2000;
    return rv;
}
#endif

uint8_t dectobcd(const uint8_t val)
{
    return ((val / 10 * 16) + (val % 10));
}

uint8_t bcdtodec(const uint8_t val)
{
    return ((val / 16 * 10) + (val % 16));
}

uint8_t inp2toi(char *cmd, const uint16_t seek)
{
    uint8_t rv;
    rv = (cmd[seek] - 48) * 10 + cmd[seek + 1] - 48;
    return rv;
}