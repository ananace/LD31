#pragma once

#include "Time.hpp"


namespace Util
{

class DateTime
{
public:
    enum Month
    {
        January, February,
        March, April,
        May, June,
        July, August,
        September, October,
        November, December
    };

    DateTime(int year = 0, int month = 0, int day = 0, int hour = 0, int minute = 0, int second = 0);
    DateTime(int64_t timeVal);
    DateTime(const Timestamp& ts);
    DateTime(const DateTime&) = default;
    ~DateTime() = default;

    DateTime& operator=(const DateTime&) = default;

    bool operator==(const DateTime&) const;

    bool operator<(const DateTime&) const;
    bool operator>(const DateTime&) const;
    
    DateTime& operator+=(const DateTime&);
    DateTime& operator+=(const Timespan&);
    DateTime& operator-=(const DateTime&);
    DateTime& operator-=(const Timespan&);

    DateTime operator+(const DateTime&) const;
    DateTime operator+(const Timespan&) const;
    DateTime operator-(const DateTime&) const;
    DateTime operator-(const Timespan&) const;

    uint16_t getYear() const;
    void setYear(uint16_t);
    Month getMonth() const;
    void setMonth(Month);
    uint8_t getDay() const;
    void setDay(uint8_t);

    uint8_t getHour() const;
    void setHour(uint8_t);
    uint8_t getMinute() const;
    void setMinute(uint8_t);
    uint8_t getSecond() const;
    void setSecond(uint8_t);

    std::tm getCalendarTime() const;

    static DateTime now(); // const Timestamp& when = ClockImpl::now()

private:
    std::tm mCalendarTime;
};

}
