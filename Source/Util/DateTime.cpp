#include "DateTime.hpp"

#include <Script/ScriptExtensions.hpp>

#include <angelscript.h>

#include <cassert>
#include <ctime>

using Util::DateTime;

DateTime::DateTime(int year, int month, int day, int hour, int minute, int second)
{
    mCalendarTime.tm_year = (year <= 1900 ? year : year - 1900);
    mCalendarTime.tm_mon = month;
    mCalendarTime.tm_mday = day + 1;
    mCalendarTime.tm_hour = hour;
    mCalendarTime.tm_min = minute;
    mCalendarTime.tm_sec = second;
    
    mCalendarTime.tm_isdst = -1;

    std::mktime(&mCalendarTime);
}
DateTime::DateTime(int64_t timeVal)
{
    std::time_t time = (std::time_t)timeVal;

    mCalendarTime = *std::localtime(&time);
}
DateTime::DateTime(const Util::Timestamp& ts)
{
    std::time_t time = ClockImpl::to_time_t(ts);

    mCalendarTime = *std::localtime(&time);
}

bool DateTime::operator==(const DateTime& rhs) const
{
    std::tm tmL = mCalendarTime, tmR = rhs.mCalendarTime;

    std::time_t lht = std::mktime(&tmL),
        rht = std::mktime(&tmR);

    return (lht >= 0 && lht == rht);
}

bool DateTime::operator<(const DateTime& rhs) const
{
    std::tm tmL = mCalendarTime, tmR = rhs.mCalendarTime;

    std::time_t lht = std::mktime(&tmL),
        rht = std::mktime(&tmR);

    return (lht < rht);
}
bool DateTime::operator>(const DateTime& rhs) const
{
    std::tm tmL = mCalendarTime, tmR = rhs.mCalendarTime;

    std::time_t lht = std::mktime(&tmL),
        rht = std::mktime(&tmR);

    return (lht > rht);
}

DateTime& DateTime::operator+=(const DateTime& rhs)
{
    std::tm copy = mCalendarTime,
            rhst = rhs.mCalendarTime;

    copy.tm_year += rhst.tm_year;
    copy.tm_mon += rhst.tm_mon;
    copy.tm_mday += rhst.tm_mday;
    copy.tm_hour += rhst.tm_hour;
    copy.tm_min += rhst.tm_min;
    copy.tm_sec += rhst.tm_sec;

    if (copy.tm_isdst < 0)
        copy.tm_isdst = rhst.tm_isdst;

    std::mktime(&copy);

    mCalendarTime = copy;
    return *this;
}

DateTime& DateTime::operator+=(const Util::Timespan& rhs)
{
    if (rhs < std::chrono::seconds(1))
        return *this;

    mCalendarTime.tm_sec += (int)std::chrono::duration_cast<std::chrono::seconds>(rhs).count();

    std::mktime(&mCalendarTime);

    return *this;
}

DateTime& DateTime::operator-=(const DateTime& rhs)
{
    std::tm copy = mCalendarTime,
        rhst = rhs.mCalendarTime;

    copy.tm_year -= rhst.tm_year;
    copy.tm_mon -= rhst.tm_mon;
    copy.tm_mday -= rhst.tm_mday;
    copy.tm_hour -= rhst.tm_hour;
    copy.tm_min -= rhst.tm_min;
    copy.tm_sec -= rhst.tm_sec;

    if (copy.tm_isdst < 0)
        copy.tm_isdst = rhst.tm_isdst;

    std::mktime(&copy);

    mCalendarTime = copy;
    return *this;
}

DateTime& DateTime::operator-=(const Util::Timespan& rhs)
{
    mCalendarTime.tm_sec -= (int)std::chrono::duration_cast<std::chrono::seconds>(rhs).count();

    std::mktime(&mCalendarTime);

    return *this;
}


DateTime DateTime::operator+(const DateTime& rhs) const
{
    DateTime copy = *this;
    copy += rhs;
    return copy;
}

DateTime DateTime::operator+(const Util::Timespan& rhs) const
{
    DateTime copy = *this;
    copy += rhs;
    return copy;
}

DateTime DateTime::operator-(const DateTime& rhs) const
{
    DateTime copy = *this;
    copy -= rhs;
    return copy;
}

DateTime DateTime::operator-(const Util::Timespan& rhs) const
{
    DateTime copy = *this;
    copy -= rhs;
    return copy;
}

uint16_t DateTime::getYear() const
{
    return mCalendarTime.tm_year + 1900;
}
DateTime::Month DateTime::getMonth() const
{
    return DateTime::Month(mCalendarTime.tm_mon);
}
uint8_t DateTime::getDay() const
{
    return mCalendarTime.tm_mday - 1;
}

uint8_t DateTime::getHour() const
{
    return mCalendarTime.tm_hour;
}
uint8_t DateTime::getMinute() const
{
    return mCalendarTime.tm_min;
}
uint8_t DateTime::getSecond() const
{
    return mCalendarTime.tm_sec;
}

std::tm DateTime::getCalendarTime() const
{
    return mCalendarTime;
}

DateTime DateTime::now()
{
    return DateTime(Util::ClockImpl::now());
}

namespace
{
    void create_DateTime_clean(DateTime* memory)
    {
        new (memory)DateTime();
    }

    void create_DateTime(int year, int month, int day, int hour, int minute, int second, DateTime* memory)
    {
        new (memory)DateTime(year, month, day, hour, minute, second);
    }

    void destruct_DateTime(DateTime* memory)
    {
        memory->~DateTime();
    }

    std::string tostring_DateTime(const DateTime& dt)
    {
        char buf[256];
        std::tm tm = dt.getCalendarTime();
        size_t len = std::strftime(buf, 256, "%c", &tm);

        return std::string(buf, len);
    }

    int cmp_DateTime(const DateTime& rhs, const DateTime& lhs)
    {
        return (lhs < rhs ? -1 : (lhs == rhs ? 0 : 1));
    }


    bool Reg()
    {
        Script::ScriptExtensions::AddExtension([](asIScriptEngine* eng) {
            int r = 0;

            r = eng->SetDefaultNamespace("DateTime"); assert(r >= 0);

            r = eng->RegisterEnum("Month"); assert(r >= 0);
            r = eng->RegisterEnumValue("Month", "January", DateTime::January); assert(r >= 0);
            r = eng->RegisterEnumValue("Month", "February", DateTime::February); assert(r >= 0);
            r = eng->RegisterEnumValue("Month", "March", DateTime::March); assert(r >= 0);
            r = eng->RegisterEnumValue("Month", "April", DateTime::April); assert(r >= 0);
            r = eng->RegisterEnumValue("Month", "May", DateTime::May); assert(r >= 0);
            r = eng->RegisterEnumValue("Month", "June", DateTime::June); assert(r >= 0);
            r = eng->RegisterEnumValue("Month", "July", DateTime::July); assert(r >= 0);
            r = eng->RegisterEnumValue("Month", "August", DateTime::August); assert(r >= 0);
            r = eng->RegisterEnumValue("Month", "September", DateTime::September); assert(r >= 0);
            r = eng->RegisterEnumValue("Month", "October", DateTime::October); assert(r >= 0);
            r = eng->RegisterEnumValue("Month", "November", DateTime::November); assert(r >= 0);
            r = eng->RegisterEnumValue("Month", "December", DateTime::December); assert(r >= 0);

            r = eng->SetDefaultNamespace(""); assert(r >= 0);


            r = eng->RegisterObjectType("DateTime", sizeof(DateTime), asOBJ_VALUE | asGetTypeTraits<DateTime>()); assert(r >= 0);

            r = eng->RegisterObjectBehaviour("DateTime", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(create_DateTime_clean), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectBehaviour("DateTime", asBEHAVE_CONSTRUCT, "void f(int year=0,int month=0,int day=0, int hour=0, int minute=0, int second=0)", asFUNCTION(create_DateTime), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectBehaviour("DateTime", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(destruct_DateTime), asCALL_CDECL_OBJLAST); assert(r >= 0);

            r = eng->RegisterObjectMethod("DateTime", "DateTime& opAssign(DateTime&in)", asMETHOD(DateTime, operator=), asCALL_THISCALL); assert(r >= 0);

            r = eng->RegisterObjectMethod("DateTime", "bool opEquals(DateTime&in)", asMETHOD(DateTime, operator==), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("DateTime", "int opCmp(DateTime&in)", asFUNCTION(cmp_DateTime), asCALL_CDECL_OBJLAST); assert(r >= 0);

            r = eng->RegisterObjectMethod("DateTime", "DateTime& opAddAssign(DateTime&in)", asMETHODPR(DateTime, operator+=, (const DateTime&), DateTime&), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("DateTime", "DateTime& opSubAssign(DateTime&in)", asMETHODPR(DateTime, operator-=, (const DateTime&), DateTime&), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("DateTime", "DateTime opAdd(DateTime&in) const", asMETHODPR(DateTime, operator+, (const DateTime&) const, DateTime), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("DateTime", "DateTime opSub(DateTime&in) const", asMETHODPR(DateTime, operator-, (const DateTime&) const, DateTime), asCALL_THISCALL); assert(r >= 0);

            r = eng->RegisterObjectMethod("DateTime", "uint16 get_Year() const", asMETHOD(DateTime, getYear), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("DateTime", "DateTime::Month get_Month() const", asMETHOD(DateTime, getMonth), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("DateTime", "uint8 get_Day() const", asMETHOD(DateTime, getDay), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("DateTime", "uint8 get_Hour() const", asMETHOD(DateTime, getHour), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("DateTime", "uint8 get_Minute() const", asMETHOD(DateTime, getMinute), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("DateTime", "uint8 get_Second() const", asMETHOD(DateTime, getSecond), asCALL_THISCALL); assert(r >= 0);

            r = eng->RegisterObjectMethod("DateTime", "string ToString() const", asFUNCTION(tostring_DateTime), asCALL_CDECL_OBJLAST); assert(r >= 0);

            r = eng->SetDefaultNamespace("DateTime"); assert(r >= 0);

            r = eng->RegisterGlobalFunction("DateTime get_Now()", asFUNCTION(DateTime::now), asCALL_CDECL); assert(r >= 0);

            r = eng->SetDefaultNamespace(""); assert(r >= 0);

            /*

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


            */
        });

        return true;
    }
}

bool Script::ScriptExtensions::DateTimeExtensions = Reg();
