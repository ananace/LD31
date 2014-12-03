#include "Time.hpp"

#include <chrono>
#include <iostream>
#include <iomanip>
#include <ratio>
#include <sstream>

#ifdef LD31_WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <cassert>

using namespace Util;

#ifdef LD31_WINDOWS
std::chrono::system_clock::time_point ClockImpl::sSystemStart;
int64_t ClockImpl::sStart;
int64_t ClockImpl::sFreq = ClockImpl::init();

Timestamp ClockImpl::now()
{
    int64_t count = 0;
    HANDLE currentThread = GetCurrentThread();
    DWORD_PTR previousMask = SetThreadAffinityMask(currentThread, 1);

    QueryPerformanceCounter((LARGE_INTEGER*)&count);

    SetThreadAffinityMask(currentThread, previousMask);

    return time_point(duration((count * std::nano::den) / sFreq));
}

time_t ClockImpl::to_time_t(const time_point& tp)
{
    using std::chrono::system_clock;
    using std::chrono::duration_cast;

    return system_clock::to_time_t(system_clock::time_point(sSystemStart + duration_cast<system_clock::duration>(tp - time_point(duration((sStart * std::nano::den) / sFreq)))));
}

int64_t ClockImpl::init()
{
    HANDLE currentThread = GetCurrentThread();
    DWORD_PTR previousMask = SetThreadAffinityMask(currentThread, 1);

    QueryPerformanceFrequency((LARGE_INTEGER*)&sFreq);
    QueryPerformanceCounter((LARGE_INTEGER*)&sStart);

    SetThreadAffinityMask(currentThread, previousMask);
    
    sSystemStart = std::chrono::system_clock::now();

    return sFreq;
}

#endif


std::ostream& operator<<(std::ostream& os, const Timestamp& out)
{
    std::time_t time = ClockImpl::to_time_t(out);

#ifdef LD31_WINDOWS
    tm tm_t;
    assert(localtime_s(&tm_t, &time) == 0);

    char temp[256];
    auto len = strftime(temp, 256, "%Y-%m-%d %H:%M:%S", &tm_t);

    os << std::string(temp, len);
#else
    typedef std::ostreambuf_iterator<char, std::char_traits<char>> Iter;
    typedef std::time_put<char, Iter> TimePut;
    Iter begin = Iter(os.rdbuf());
    Iter end = std::use_facet<TimePut>(os.getloc()).put(begin, os, os.fill(), std::localtime(&time), "%F %T", &"%F %T"[std::char_traits<char>::length("%F %T")]);
    if (end.failed())
        os.setstate(std::ios_base::badbit);
#endif

    return os;
}
std::ostream& operator<<(std::ostream& os, const Timespan& out)
{
    os << std::setprecision(2) << std::fixed;

    if (out > std::chrono::seconds(1))
        os << std::chrono::duration<float>(out).count() << "s";
    else if (out > std::chrono::milliseconds(1))
        os << std::chrono::duration<float, std::milli>(out).count() << "ms";
    else if (out > std::chrono::microseconds(1))
        os << std::chrono::duration<float, std::micro>(out).count() << "us";
    else
        os << std::chrono::duration<uint32_t, std::nano>(out).count() << "ns";

    return os;
}

void sleep(const Timestamp& ts)
{
    auto now = ClockImpl::now();
    assert(ts > now);
    auto length = ts - now;

#ifdef LD31_WINDOWS
    Sleep((DWORD)std::chrono::duration_cast<std::chrono::milliseconds>(length).count());
#else
    usleep(std::chrono::duration_cast<std::chrono::microseconds>(length).count());
#endif
}

void sleep(const Timespan& ts)
{
#ifdef LD31_WINDOWS
    Sleep((DWORD)std::chrono::duration_cast<std::chrono::milliseconds>(ts).count());
#else
    usleep(std::chrono::duration_cast<std::chrono::microseconds>(ts).count());
#endif
}
