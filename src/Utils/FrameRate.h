#pragma once

#include <chrono>
#include <iostream>
#include <ratio>
#include <thread>

namespace Luntik::Utils {
    template<std::intmax_t FPS>
    class frame_rater {
    public:
        frame_rater() :                 // initialize the object keeping the pace
            time_between_frames{1},     // std::ratio<1, FPS> seconds
            tp{std::chrono::steady_clock::now()}
        {}

        void sleep() {
            // add to time point
            tp += time_between_frames;

            // and sleep until that time point
            std::this_thread::sleep_until(tp);
        }

    private:
        // a duration with a length of 1/FPS seconds
        std::chrono::duration<double, std::ratio<1, FPS>> time_between_frames;

        // the time point we'll add to in every loop
        std::chrono::time_point<std::chrono::steady_clock, decltype(time_between_frames)> tp;
    };

    template <int FPS>
    class no_sleep_timer {
    public:
        no_sleep_timer() : m_TimeStep(1.D / double(FPS))
        {}

        bool should_run_code(double delta) {
            m_Count += delta;

            if (m_Count >= m_TimeStep) {
                m_Count -= m_TimeStep;
                return true;
            }

            return false;
        }

    private:
        double m_TimeStep;
        double m_Count = 0.0;
    };
}