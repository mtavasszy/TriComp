#ifndef STOPWATCH_H_
#define STOPWATCH_H_

#include <chrono>
#include <iostream>

template <typename Clock = std::chrono::steady_clock>
class stopwatch
{
    typename Clock::time_point last_;

public:
    stopwatch()
        : last_(Clock::now())
    {}

    void reset()
    {
        *this = stopwatch();
    }

    typename Clock::duration elapsed() const
    {
        return Clock::now() - last_;
    }

    typename Clock::duration tick()
    {
        auto now = Clock::now();
        auto elapsed = now - last_;
        last_ = now;
        return elapsed;
    }
};

template <typename T, typename Rep, typename Period>
T duration_cast(const std::chrono::duration<Rep, Period>& duration)
{
    return duration.count() * static_cast<T>(Period::num) / static_cast<T>(Period::den);
}

#endif // !STOPWATCH_H_
