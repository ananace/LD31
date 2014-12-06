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

    DateTime();
    DateTime(int year, int month, int day, int hour, int minute, int second);
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

    void justify();

    int getYear() const;
    void setYear(int);
    int getMonth() const;
    void setMonth(int);
    int getDay() const;
    void setDay(int);

    int getHour() const;
    void setHour(int);
    int getMinute() const;
    void setMinute(int);
    int getSecond() const;
    void setSecond(int);

    std::tm getCalendarTime() const;

    static DateTime now(); // const Timestamp& when = ClockImpl::now()

private:
    std::tm mCalendarTime;
};

}
