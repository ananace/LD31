#pragma once

#include <chrono>
#include <cstdint>

namespace Util
{

#ifdef _MSC_VER
class ClockImpl
{
public:
    typedef std::chrono::duration<uint64_t, std::nano> duration;
    typedef std::chrono::time_point<ClockImpl> time_point;

    static time_point now();
    static time_t to_time_t(const time_point& t);

private:
    static int64_t init();

    static std::chrono::system_clock::time_point sSystemStart;
    static time_point sStart;
    static int64_t sFreq;
};
#else
typedef std::chrono::high_resolution_clock ClockImpl;
#endif

typedef ClockImpl::duration Timespan;
typedef ClockImpl::time_point Timestamp;

void sleep(const Timespan& ts);
void sleep(const Timestamp& until);

}
