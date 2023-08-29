/*!
    \file DS3231.h
    \brief DS3231 Real Time Clock Library
*/

#ifndef _DS3231_H_
#define _DS3231_H_

#include <inttypes.h>

#define DS3231_ADDRESS               0x68

// Register addresses
#define DS3231_SECONDS_REG           0x0
#define DS3231_MINUTES_REG           0x1
#define DS3231_HOURS_REG             0x2
#define DS3231_DAY_REG               0x3
#define DS3231_DATE_REG              0x4
#define DS3231_CENTURY_MONTH_REG     0x5
#define DS3231_YEAR_REG              0x6

#define DS3231_ALARM1_SECONDS_REG    0x7
#define DS3231_ALARM1_MINUTES_REG    0x8
#define DS3231_ALARM1_HOURS_REG      0x9
#define DS3231_ALARM1_DAY_DATE_REG   0xA

#define DS3231_ALARM2_MINUTES_REG    0xB
#define DS3231_ALARM2_HOURS_REG      0xC
#define DS3231_ALARM2_DAY_DATE_REG   0xD

#define DS3231_CONTROL_REG           0xE
#define DS3231_STATUS_REG            0xF
#define DS3231_AGING_OFFSET_REG      0x10
#define DS3231_TEMP_MSB_REG          0x11
#define DS3231_TEMP_LSB_REG          0x12


// Masks and shifts for time
#define DS3231_SECONDS_MASK          0x0F
#define DS3231_10_SECONDS_MASK       0x70
#define DS3231_10_SECONDS_SHIFT      0x4

#define DS3231_MINUTES_MASK          0x0F
#define DS3231_10_MINUTES_MASK       0x70
#define DS3231_10_MINUTES_SHIFT      0x4

#define DS3231_HOURS_MASK            0x0F
#define DS3231_10_HOURS_MASK         0x10
#define DS3231_20_HOURS_AM_PM_MASK   0x20
#define DS3231_12_24_HOUR_MASK       0x40

#define DS3231_10_HOURS_SHIFT        0x4
#define DS3231_20_HOURS_AM_PM_SHIFT  0x5
#define DS3231_12_24_HOUR_SHIFT      0x6

#define DS3231_DAY_MASK              0x07

#define DS3231_DATE_MASK             0x0F
#define DS3231_10_DATE_MASK          0x30
#define DS3231_10_DATE_SHIFT         0x4

#define DS3231_MONTH_MASK            0x0F
#define DS3231_10_MONTH_MASK         0x10
#define DS3231_CENTURY_MASK          0x80
#define DS3231_10_MONTH_SHIFT        0x4
#define DS3231_CENTURY_SHIFT         0x7

#define DS3231_YEAR_MASK             0x0F
#define DS3231_10_YEAR_MASK          0xF0
#define DS3231_10_YEAR_SHIFT         0x4


// DS3231 Values
#define DS3231_HOURS_24_FORMAT       0x0
#define DS3231_HOURS_12_FORMAT       0x1
#define DS3231_HOURS_AM              0x0
#define DS3231_HOURS_PM              0x1

#define DS3231_DAY_MONDAY            0x0
#define DS3231_DAY_TUESDAY           0x1
#define DS3231_DAY_WEDNESDAY         0x2
#define DS3231_DAY_THURSDAY          0x3
#define DS3231_DAY_FRIDAY            0x4
#define DS3231_DAY_SATURDAY          0x5
#define DS3231_DAY_SUNDAY            0x6
#define DS3231_NUM_DAYS              0x7
#define DS3231_DAY_CHAR_LENGTH       0xA

#define DS3231_10_POSITION           10
#define DS3231_20_POSITION           20


typedef struct HOURS_TAG
{
    uint8_t hour;          //!< Hour. 1-12 for 12 hour format. 0-23 for 24 hour format.
    uint8_t hour_am_pm;    //!< AM/PM for 12 hour format DS3231_HOURS_AM, DS3231_HOURS_PM
    uint8_t hour_12_24;    //!< 12 or 24 hour format DS3231_HOURS_12_FORMAT, DS3231_HOURS_24_FORMAT
} HOURS;

typedef struct TIME_TAG
{
    uint8_t seconds;    //!< Seconds 0-59
    uint8_t minutes;    //!< Minutes 0-59
    HOURS hour_data;    //!< Hours, AM/PM, 12/24 hour format
} TIME;

typedef struct DATE_TAG
{
    uint8_t day;        //!< Day of the week 0-7
    uint8_t date;       //!< Date 1-31
    uint8_t month;      //!< Month 1-12
    uint8_t year;       //!< Year 0-99
    uint8_t century;    //!< Century 0-1
} DATE;

class DS3231
{
    public:
        const char dayIndexToString[DS3231_NUM_DAYS][DS3231_DAY_CHAR_LENGTH] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};

        uint8_t readRegister(uint8_t reg);
        void writeRegister(uint8_t reg, uint8_t data);

        uint8_t readSeconds(void);
        uint8_t readMinutes(void);
        void readHours(HOURS * const hour_info);
        void readTime(TIME * const time);

        void writeSeconds(uint8_t sec);
        void writeMinutes(uint8_t min);
        void writeHours(const HOURS * const hour_data);
        void writeTime(const TIME * const time_data);

        uint8_t readDay(void);
        uint8_t readDate(void);
        uint8_t readMonth(void);
        uint8_t readYear(void);
        uint8_t readCentury(void);
        void readFullDate(DATE * const date);

        void writeDay(uint8_t day);
        void writeDate(uint8_t date);
        void writeMonth(uint8_t month);
        void writeYear(uint8_t year);
        void writeFullDate(const DATE * const date);
};

#endif
