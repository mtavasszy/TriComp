#ifndef STOPWATCH_H_
#define STOPWATCH_H_

#include <chrono>

class Stopwatch
{
public:

    Stopwatch() {
        m_start = std::chrono::steady_clock::now();
    }

    long long reset()
    {
        auto stop = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - m_start);
        m_start = std::chrono::steady_clock::now();
        return duration.count();
    }

private:
    std::chrono::steady_clock::time_point m_start;
};

#endif // !STOPWATCH_H_
