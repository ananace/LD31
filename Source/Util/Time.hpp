#pragma once

#include <Defines.hpp>

#include <chrono>
#include <iostream>

#include <cstdint>

namespace Util
{

#ifdef LD31_WINDOWS
/// The Windows implementation of a high-precision clock
class ClockImpl
{
public:
    typedef std::chrono::duration<uint64_t, std::nano> duration;
    typedef std::chrono::time_point<ClockImpl> time_point;

    /// Get the current time
    static time_point now();
    /// Convert a time point to a time_t value
    static time_t to_time_t(const time_point& t);

private:
    /// Initializes the clock and stores the frequency
    static int64_t init();

    /// The time point when the clock initialized
    static std::chrono::system_clock::time_point sSystemStart;
    /// The cpu cycle count when the clock initialized
    static int64_t sStart,
    /// The cycle frequency of the cpu
                   sFreq;
};
#else
/// The POSIX implementation of a high-precision clock
typedef std::chrono::high_resolution_clock ClockImpl;
#endif

typedef ClockImpl::duration Timespan;
typedef ClockImpl::time_point Timestamp;

/// Sleep for the specified duration
void sleep(const Timespan& ts);
/// Sleep until the specified time point
void sleep(const Timestamp& until);

}

std::ostream& operator<<(std::ostream& os, const Util::Timespan& out);
std::ostream& operator<<(std::ostream& os, const Util::Timestamp& out);
