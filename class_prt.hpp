#pragma once
#include <chrono>
#include <mutex>
#include <iostream>

#define PRINT_TIME
std::mutex print;
constexpr int PERIOD_PRINT = 50000;
constexpr int COUNT_SR_TIME = 100;


class TimePrt
{
    #ifdef PRINT_TIME
    private:
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point allStartTime;

        std::chrono::duration<double> srTime[COUNT_SR_TIME];
        std::chrono::duration<double> nowSrTime;

        std::chrono::duration<double> allSrTime[COUNT_SR_TIME];
        std::chrono::duration<double> allNowSrTime;

        std::chrono::duration<double> deleteTime;

        unsigned int period = 0;
        unsigned int indGl = 0;
        bool first = true;
    public:
        TimePrt()
        {
            startTime = std::chrono::system_clock::now();
            int i;
            for (i = 0; i < COUNT_SR_TIME; ++i)
                srTime[i] = std::chrono::duration<double>(0);
            for (i = 0; i < COUNT_SR_TIME; ++i)
                allSrTime[i] = std::chrono::duration<double>(0);
        };

        void start_time()
        {
            startTime = std::chrono::system_clock::now();
        };

        void start_time_all_work()
        {
            allStartTime = std::chrono::system_clock::now();
        };
        
        void clear_time()
        {
            if (first)
                return;
        
            for (int i = 0; i < COUNT_SR_TIME; ++i)
            {
                srTime[i] = std::chrono::duration<double>(0);
                allSrTime[i] = std::chrono::duration<double>(0);
            }
            indGl = 0;
            first = true;
        };

        void print_time(char k)
        {
            if (first)
            {
                first = false;
                return;
            }
            srTime[indGl] = std::chrono::duration<double>(std::chrono::system_clock::now() - startTime);
            allSrTime[indGl] = std::chrono::duration<double>(std::chrono::system_clock::now() - allStartTime);
            // Начальное заполнение массива
            if (srTime[COUNT_SR_TIME - 1].count() == 0)
            {
                    indGl = (indGl + 1) % COUNT_SR_TIME;
                    return ;
            }
        
            // Цикл чтобы всегда иметь среднее число в массиве.
            if ( ++period < PERIOD_PRINT )
                return;
        
            int i;
        
            nowSrTime = std::chrono::duration<double>(0);
        
            for (i = 0; i < COUNT_SR_TIME; ++i)
                nowSrTime += srTime[i];
        
        
            allNowSrTime = allSrTime[0];
        
            for (i = 1; i < COUNT_SR_TIME; ++i)
                allNowSrTime += allSrTime[i];
                
            indGl = (indGl + 1) % COUNT_SR_TIME;
        
            print.lock();
        
            period = 0;
        
            std::cout << "\033[" << k << ";1H" << "\033[2K" << "\033[" << k << ";1H";
            switch (k)
            {
            case '1':
                std::cout << "Generate_bit";
                break;
            case '2':
                std::cout << "s2p";
                break;
            case '3':
                std::cout << "mapper";
                break;
            case '4':
                std::cout << "ifft";
                break;
            case '5':
                std::cout << "p2s";
                break;
            default:
                break;
            }
        
            std::cout << "\033[" << k << ";8H|all time cycle: " << allNowSrTime.count()/static_cast<double>(COUNT_SR_TIME) 
            << ";\033[" << k << ";40H| work cycle: " << nowSrTime.count()/static_cast<double>(COUNT_SR_TIME)
            << ";\033[" << k << ";70H| cycle work " << (nowSrTime.count() / allNowSrTime.count()) * 100 << "%" << std::endl;
        
            std::cout << "\033[0m" << std::endl;
        
            print.unlock();
            return;
        };

        ~TimePrt(){
        };
    #else
    public:
    TimePrt(){};

    void start_time(){};
    
    void start_time_all_work(){};
    
    void clear_time(){};
    
    void print_time(char k){};
    
    ~TimePrt(){};
    #endif
};
