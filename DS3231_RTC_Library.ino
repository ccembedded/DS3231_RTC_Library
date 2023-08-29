#include "DS3231.h"
#include "Wire.h"

static DS3231 rtc;

void setup()
{
    Serial.begin(9600);
    Wire.begin();

    initDateTime(&rtc);
}

void loop()
{
    TIME time;
    DATE date;
    char string_data[100];
    char am_pm[] = "XM";

    rtc.readTime(&time);
    rtc.readFullDate(&date);

    if(time.hour_data.hour_12_24)
    {
        if(time.hour_data.hour_am_pm)
        {
            am_pm[0] = 'P';
        }
        else
        {
            am_pm[0] = 'A';
        }
    }
    else
    {
        am_pm[0] = ' ';
        am_pm[1] = ' ';
    }

    snprintf(string_data, 100, "%s %02u/%02u/%02u\n%02u:%02u:%02u %s\n\n", rtc.dayIndexToString[date.day], date.month, date.date, date.year, time.hour_data.hour, time.minutes, time.seconds, am_pm);

    Serial.print(string_data);
    delay(1000);
}

void initDateTime(const DS3231 * const rtc)
{
    TIME time_init;
    DATE date_init;

    time_init.seconds = 20;
    time_init.minutes = 20;
    time_init.hour_data.hour = 12;
    time_init.hour_data.hour_12_24 = DS3231_HOURS_24_FORMAT;
    time_init.hour_data.hour_am_pm = DS3231_HOURS_AM;

    date_init.day = DS3231_DAY_WEDNESDAY;
    date_init.date = 25;
    date_init.month = 12;
    date_init.year = 98;

    rtc->writeTime(&time_init);
    rtc->writeFullDate(&date_init);
}

void runTests(const DS3231 * const rtc)
{
    testTime(rtc);
    testDate(rtc);
}

void testTime(const DS3231 * const rtc)
{
    TIME write_time, read_time;
    char buff[100];
    char string_am_pm[] = "XM";
    bool test_failure = false;

    // Test 12 hour format
    for(uint8_t am_pm = 0; am_pm < 2; am_pm++)
    {
        for(uint8_t hours = 1; hours <= 12; hours++)
        {
            if(am_pm == DS3231_HOURS_AM)
            {
                string_am_pm[0] = 'A';
            }
            else
            {
                string_am_pm[0] = 'P';
            }

            snprintf(buff, 100, "Testing hour: %u %s", hours, string_am_pm);
            Serial.println(buff);

            for(uint8_t minutes = 0; minutes < 60; minutes++)
            {
                for(uint8_t seconds = 0; seconds < 60; seconds++)
                {
                    memset(&read_time, 0, sizeof(TIME));

                    // Use memcmp?
                    write_time.seconds = seconds;
                    write_time.minutes = minutes;
                    write_time.hour_data.hour = hours;
                    write_time.hour_data.hour_12_24 = DS3231_HOURS_12_FORMAT;
                    write_time.hour_data.hour_am_pm = am_pm;

                    rtc->writeTime(&write_time);

                    delay(1);

                    rtc->readTime(&read_time);

                    if(read_time.seconds != write_time.seconds)
                    {
                        Serial.println("SECONDS ERROR");
                        test_failure = true;
                    }

                    if(read_time.minutes != write_time.minutes)
                    {
                        Serial.println("MINUTES ERROR");
                        test_failure = true;
                    }
                    
                    if(read_time.hour_data.hour != write_time.hour_data.hour)
                    {
                        Serial.println("HOUR ERROR");
                        test_failure = true;
                    }

                    if(read_time.hour_data.hour_12_24 != write_time.hour_data.hour_12_24)
                    {
                        Serial.println("HOUR FORMAT ERROR");
                        test_failure = true;
                    }

                    if(read_time.hour_data.hour_am_pm != write_time.hour_data.hour_am_pm)
                    {
                        Serial.println("AM PM ERROR");
                        test_failure = true;
                    }
                }
            }
        }
    }

    // Test 24 hour format
    for(uint8_t hours = 0; hours < 24; hours++)
    {
        snprintf(buff, 100, "Testing hour: %u", hours);
        Serial.println(buff);

        for (uint8_t minutes = 0; minutes < 60; minutes++)
        {
            for (uint8_t seconds = 0; seconds < 60; seconds++)
            {
                memset(&read_time, 0, sizeof(TIME));

                // Use memcmp?
                write_time.seconds = seconds;
                write_time.minutes = minutes;
                write_time.hour_data.hour = hours;
                write_time.hour_data.hour_12_24 = DS3231_HOURS_24_FORMAT;
                write_time.hour_data.hour_am_pm = 0;

                rtc->writeTime(&write_time);

                delay(1);

                rtc->readTime(&read_time);

                if (read_time.seconds != write_time.seconds)
                {
                    Serial.println("Seconds Error");
                    test_failure = true;
                }

                if (read_time.minutes != write_time.minutes)
                {
                    Serial.println("Minutes Error");
                    test_failure = true;
                }

                if (read_time.hour_data.hour != write_time.hour_data.hour)
                {
                    Serial.println("Hour Error");
                    test_failure = true;

                    Serial.println(read_time.hour_data.hour);
                    Serial.println(write_time.hour_data.hour);
                    Serial.println(rtc->readRegister(DS3231_HOURS_REG));
                }
            }
        }
    }

    Serial.println("Time Test Completed");

    if(test_failure)
    {
        Serial.println("Test failure");
    }
    else
    {
        Serial.println("All tests passing");
    }
}

void testDate(const DS3231 * const rtc)
{
    DATE write_date, read_date;
    char buff[100];
    bool test_failure = false;

    // Reset time so day and date don't change to next day during test
    initDateTime(rtc);

    for(uint8_t year = 0; year < 100; year++)
    {
        for(uint8_t month = 1; month <= 12; month++)
        {
            snprintf(buff, 100, "Testing Year: %u    Month: %u\n", year, month);
            Serial.print(buff);

            for(uint8_t date = 1; date <= 31; date++)
            {
                for(uint8_t day = 0; day < 7; day++)
                {
                    write_date.day = day;
                    write_date.date = date;
                    write_date.month = month;
                    write_date.year = year;

                    rtc->writeFullDate(&write_date);

                    delay(1);

                    rtc->readFullDate(&read_date);

                    if(read_date.day != write_date.day)
                    {
                        Serial.println("Day Error");
                        Serial.println(read_date.day);
                        Serial.println(write_date.day);
                        Serial.println(rtc->readRegister(DS3231_DAY_REG));
                        test_failure = true;
                    }

                    if(read_date.date != write_date.date)
                    {
                        Serial.println("Date Error");
                        test_failure = true;
                    }

                    if(read_date.month != write_date.month)
                    {
                        Serial.println("Month Error");
                        test_failure = true;
                    }

                    if(read_date.year != write_date.year)
                    {
                        Serial.println("Year Error");
                        test_failure = true;
                    }
                }
            }
        }
    }

    Serial.println("Date test completed");

    if(test_failure)
    {
        Serial.println("Test failure");
    }
    else
    {
        Serial.println("All tests passing");
    }
}