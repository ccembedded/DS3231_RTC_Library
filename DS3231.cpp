/*!
    \file DS3231.cpp
    \brief DS3231 Real Time Clock Library
*/

#include "DS3231.h"
#include "Wire.h"

/*!
    \brief Reads value of specified register
    \param[in] reg Address of register to read
    \return Value of register
*/
uint8_t DS3231::readRegister(uint8_t reg)
{
    uint8_t read_val;

    Wire.beginTransmission(DS3231_ADDRESS);
    Wire.write(reg);
    Wire.endTransmission();

    Wire.requestFrom(DS3231_ADDRESS, 1);
    while(Wire.available())
    {
        read_val = Wire.read();
    }

    return (read_val);
}

/*!
    \brief Writes value of specified register
    \param[in] reg Address of register to write
    \param[in] data Data to write 
*/
void DS3231::writeRegister(uint8_t reg, uint8_t data)
{
    Wire.beginTransmission(DS3231_ADDRESS);
    Wire.write(reg);
    Wire.write(data);
    Wire.endTransmission();
}

/*!
    \brief Reads seconds register and converts to seconds
    \return Seconds 0-59
*/
uint8_t DS3231::readSeconds(void)
{
    uint8_t read_s;
    uint8_t seconds;

    read_s = readRegister(DS3231_SECONDS_REG);

    seconds = (read_s & DS3231_SECONDS_MASK);
    seconds += ((read_s & DS3231_10_SECONDS_MASK) >> DS3231_10_SECONDS_SHIFT) * 10;

    return (seconds);
}

/*!
    \brief Reads minutes register and converts to minutes
    \return Minutes 0-59
*/
uint8_t DS3231::readMinutes(void)
{
    uint8_t read_m;
    uint8_t minutes;

    read_m = readRegister(DS3231_MINUTES_REG);

    minutes = (read_m & DS3231_MINUTES_MASK);
    minutes += ((read_m & DS3231_10_MINUTES_MASK) >> DS3231_10_MINUTES_SHIFT) * 10;

    return (minutes);
}

/*!
    \brief Reads hours register and converts to hours, 12/24 hour format, AM/PM
    \param[out] hour_info Hours, 12/24 hour format, AM/PM 
*/
void DS3231::readHours(HOURS * const hour_info)
{
    uint8_t read_h;
    uint8_t hours;
    uint8_t hour_format;
    uint8_t am_pm = 0;

    read_h = readRegister(DS3231_HOURS_REG);

    hour_format = (read_h & DS3231_12_24_HOUR_MASK) >> DS3231_12_24_HOUR_SHIFT;

    hours = read_h & DS3231_HOURS_MASK;
    hours += ((read_h & DS3231_10_HOURS_MASK) >> DS3231_10_HOURS_SHIFT) * 10;
    if(hour_format == DS3231_HOURS_12_FORMAT)
    {
        am_pm = (read_h & DS3231_20_HOURS_AM_PM_MASK) >> DS3231_20_HOURS_AM_PM_SHIFT;
    }
    else
    {
        hours += ((read_h & DS3231_20_HOURS_AM_PM_MASK) >> DS3231_20_HOURS_AM_PM_SHIFT) * 20;
    }

    hour_info->hour_12_24 = hour_format;
    hour_info->hour_am_pm = am_pm;
    hour_info->hour = hours;
}

/*!
    \brief Calls time get functions to get all time data
    \param[out] time Hours, minutes, and seconds data
*/
void DS3231::readTime(TIME * const time)
{
    HOURS read_hours;

    time->seconds = readSeconds();
    time->minutes = readMinutes();
    readHours(&read_hours);
    memcpy(&(time->hour_data), &read_hours, sizeof(HOURS));
}

/*!
    \brief Converts and writes seconds value to seconds register
    \param[in] sec Seconds value to write 0-59
*/
void DS3231::writeSeconds(uint8_t sec)
{
    uint8_t write_sec = 0;

    // Get 10s position
    if(sec >= 10)
    {
        write_sec |= ((uint8_t)(sec / 10)) << DS3231_10_MINUTES_SHIFT;
    }

    write_sec |= (sec % 10);

    writeRegister(DS3231_SECONDS_REG, write_sec);
}

/*!
    \brief Converts and writes minutes value to minutes register
    \param[in] mins Minutes data to write 0-59
*/
void DS3231::writeMinutes(uint8_t mins)
{
    uint8_t write_mins = 0;

    // Get 10s position
    if(mins >= 10)
    {
        write_mins |= ((uint8_t)(mins / 10)) << DS3231_10_MINUTES_SHIFT;
    }

    write_mins |= (mins % 10);

    writeRegister(DS3231_MINUTES_REG, write_mins);
}

/*!
    \brief Converts and writes hour data to hours register
    \param[in] hour_data Hour data to write 1-12AM/PM or 0-23
*/
void DS3231::writeHours(const HOURS * const hour_data)
{
    uint8_t write_hours = 0;

    // Set 12/24 hour format
    write_hours |= hour_data->hour_12_24 << DS3231_12_24_HOUR_SHIFT;

    if(hour_data->hour_12_24 == DS3231_HOURS_12_FORMAT)
    {
        // Set AM/PM
        write_hours |= hour_data->hour_am_pm << DS3231_20_HOURS_AM_PM_SHIFT;

        if(hour_data->hour >= 10)
        {
            // Set 10s position
            write_hours |= (hour_data->hour / 10) << DS3231_10_HOURS_SHIFT;

            // Set 1s position
            write_hours |= (hour_data->hour - 10);
        }
        else
        {
            // Set 1s position
            write_hours |= hour_data->hour;
        }
    }
    else
    {
        // 24 hour format
        if(hour_data->hour >= 20)
        {
            // Set 20s position
            write_hours |= (hour_data->hour / 20) << DS3231_20_HOURS_AM_PM_SHIFT;

            // Set 1s position
            write_hours |= (hour_data->hour - 20);
        }
        else if(hour_data->hour >= 10)
        {
            // Set 10s position
            write_hours |= (hour_data->hour / 10) << DS3231_10_HOURS_SHIFT;

            // Set 1s position
            write_hours |= (hour_data->hour - 10);
        }
        else
        {
            // Set 1s position
            write_hours |= hour_data->hour;
        }
    }

    writeRegister(DS3231_HOURS_REG, write_hours);
}

/*!
    \brief Calls time set functions to set all time data
    \param[in] time_data Time data to write
*/
void DS3231::writeTime(const TIME * const time_data)
{
    writeSeconds(time_data->seconds);
    writeMinutes(time_data->minutes);
    writeHours(&(time_data->hour_data));
}

/*!
    \brief Reads day register
    \return Day of week 0-6
*/
uint8_t DS3231::readDay(void)
{
    uint8_t read_d;
    uint8_t day;

    read_d = readRegister(DS3231_DAY_REG);

    day = read_d & DS3231_DAY_MASK;

    return (day);
}

/*!
    \brief Reads and converts date register
    \return Date 1-31
*/
uint8_t DS3231::readDate(void)
{
    uint8_t read_d;
    uint8_t date;

    read_d = readRegister(DS3231_DATE_REG);

    date = read_d & DS3231_DATE_MASK;
    date += ((read_d & DS3231_10_DATE_MASK) >> DS3231_10_DATE_SHIFT) * 10;

    return (date);
}

/*!
    \brief Reads and converts month register
    \return Month 1-12
*/
uint8_t DS3231::readMonth(void)
{
    uint8_t read_m;
    uint8_t month;

    read_m = readRegister(DS3231_CENTURY_MONTH_REG);

    month = read_m & DS3231_MONTH_MASK;
    month += ((read_m & DS3231_10_MONTH_MASK) >> DS3231_10_MONTH_SHIFT) * 10;

    return (month);
}

/*!
    \brief Reads and converts year register
    \return Year 0-99
*/
uint8_t DS3231::readYear(void)
{
    uint8_t read_y;
    uint8_t year;

    read_y = readRegister(DS3231_YEAR_REG);

    year = read_y & DS3231_YEAR_MASK;
    year += ((read_y & DS3231_10_YEAR_MASK) >> DS3231_10_YEAR_SHIFT) * 10;

    return (year);
}

/*!
    \brief Reads and converts century value
    \return Century 0-1.  Bit toggles when year register overflows from 99 to 0.
*/
uint8_t DS3231::readCentury(void)
{
    uint8_t read_c;
    uint8_t century;

    read_c = readRegister(DS3231_CENTURY_MONTH_REG);

    century = (read_c & DS3231_CENTURY_MASK) >> DS3231_CENTURY_SHIFT;

    return (century);
}

/*!
    \brief Calls date get functions to get all date data
    \param[out] date Contains Year, Month, Date, and Day data
*/
void DS3231::readFullDate(DATE * const date)
{
    date->day = readDay();
    date->date = readDate();
    date->month = readMonth();
    date->year = readYear();
    date->century = readCentury();
}

/*!
    \brief Converts and writes day register
    \param[in] day Day of the week 0-6
*/
void DS3231::writeDay(uint8_t day)
{
    uint8_t write_day = 0;

    write_day = day & DS3231_DAY_MASK;

    writeRegister(DS3231_DAY_REG, write_day);
}

/*!
    \brief Converts and writes date register
    \param[in] date Date 1-31
*/
void DS3231::writeDate(uint8_t date)
{
    uint8_t write_date = 0;

    if(date >= 10)
    {
        write_date |= ((uint8_t)(date / 10)) << DS3231_10_DATE_SHIFT;
    }

    write_date |= date % 10;

    writeRegister(DS3231_DATE_REG, write_date);
}

/*!
    \brief Converts and writes month register
    \param[in] month Month 1-12
*/
void DS3231::writeMonth(uint8_t month)
{
    uint8_t write_month = 0;

    if(month >= 10)
    {
        write_month |= ((uint8_t)(month / 10)) << DS3231_10_MONTH_SHIFT;
    }

    write_month |= month % 10;

    writeRegister(DS3231_CENTURY_MONTH_REG, write_month);
}

/*!
    \brief Converts and writes year register
    \param[in] year Year 0-99
*/
void DS3231::writeYear(uint8_t year)
{
    uint8_t write_year = 0;

    if(year >= 10)
    {
        write_year |= ((uint8_t)(year / 10)) << DS3231_10_YEAR_SHIFT;
    }

    write_year |= year % 10;

    writeRegister(DS3231_YEAR_REG, write_year);
}

/*!
    \brief Calls all date put functions to write date data
    \param[in] date Year, month, date, and day data
*/
void DS3231::writeFullDate(const DATE * const date)
{
    writeDay(date->day);
    writeDate(date->date);
    writeMonth(date->month);
    writeYear(date->year);
}
