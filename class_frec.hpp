#pragma once
#include <chrono>
#include <thread>

#define TAKTING


class FrecTh
{
    #ifdef TAKTING
    private:
        unsigned long long HzLong;
        std::chrono::duration<double> Hz;
        std::chrono::system_clock::time_point startTime;
    public:
        FrecTh(unsigned long long HHZ)
        {
            HzLong = HHZ;
            Hz = (std::chrono::duration<double>(1.0 / static_cast<double>(HHZ)));
            startTime = std::chrono::system_clock::now();
        }

        void change_Hz(unsigned long long HHZ)
        {
            Hz = (std::chrono::duration<double>(1.0 / static_cast<double>(HHZ)));
        }
        
        void start_frec()
        {
            startTime = std::chrono::system_clock::now();
        }

        void end_frec()
        {
            auto nextTime = Hz + startTime;
            std::this_thread::sleep_until(nextTime);
            startTime = std::chrono::system_clock::now();
        }

        ~FrecTh()
        {

        }
    #else
    public:
        FrecTh(unsigned long long HHZ)
        {
        }

        void change_Hz(unsigned long long HHZ)
        {
        }

        void start_frec()
        {
        }

        void end_frec()
        {
        }

        ~FrecTh()
        {

        }
    #endif
};
