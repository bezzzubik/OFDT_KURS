#include "need_libs.hpp"
#include "class_frec.hpp"
#include "class_prt.hpp"
#include <cstring>

std::atomic<bool> readyMap(false);

inline void read_chanel(unsigned char* bit4Loc)
{
    memcpy(bit4Loc, bit4Gl, sizeof(unsigned char)*N);
    // for (i = 0; i < N; ++i)
    //     bit4Loc[i] = bit4Gl[i];
}

inline void main_calculate(unsigned char* bit4Loc)
{
    // std::this_thread::sleep_for(1s);
    for (int i = 0; i < N; ++i)
    {
        switch (bit4Loc[i]) {
            case 0b0000: mapDig[i][0] = -3.0; mapDig[i][1] =  3.0; break; // 0
            case 0b0001: mapDig[i][0] = -1.0; mapDig[i][1] =  3.0; break; // 1
            case 0b0010: mapDig[i][0] = -3.0; mapDig[i][1] =  1.0; break; // 2
            case 0b0011: mapDig[i][0] = -1.0; mapDig[i][1] =  1.0; break; // 3
            case 0b0100: mapDig[i][0] = -3.0; mapDig[i][1] = -3.0; break; // 4
            case 0b0101: mapDig[i][0] = -1.0; mapDig[i][1] = -3.0; break; // 5
            case 0b0110: mapDig[i][0] = -3.0; mapDig[i][1] = -1.0; break; // 6
            case 0b0111: mapDig[i][0] = -1.0; mapDig[i][1] = -1.0; break; // 7
            case 0b1000: mapDig[i][0] =  3.0; mapDig[i][1] =  3.0; break; // 8
            case 0b1001: mapDig[i][0] =  1.0; mapDig[i][1] =  3.0; break; // 9
            case 0b1010: mapDig[i][0] =  3.0; mapDig[i][1] =  1.0; break; // 10
            case 0b1011: mapDig[i][0] =  1.0; mapDig[i][1] =  1.0; break; // 11
            case 0b1100: mapDig[i][0] =  3.0; mapDig[i][1] = -3.0; break; // 12
            case 0b1101: mapDig[i][0] =  1.0; mapDig[i][1] = -3.0; break; // 13
            case 0b1110: mapDig[i][0] =  3.0; mapDig[i][1] = -1.0; break; // 14
            case 0b1111: mapDig[i][0] =  1.0; mapDig[i][1] = -1.0; break; // 15
            default:   mapDig[i][0] = 0.0; mapDig[i][1] = 0.0; // Обработка неожиданной ситуации (опционально)
        }
    }
}


void mapper(std::stop_token stoken, unsigned long long Hz)
{
    FrecTh HzTh(Hz);
    unsigned char bit4Loc[N];

    unsigned char i = 0;
    TimePrt prtTime;
    readyMap.store(true);
    while(!startWork.load());
    while (!stoken.stop_requested())
    {
        prtTime.start_time_all_work();
        HzTh.start_frec();
        while (!newS2p.load());

        read_chanel(bit4Loc);

        newS2p.store(false);

        while (newMap.load());
        prtTime.start_time();

        main_calculate(bit4Loc);

        newMap.store(true);

        HzTh.end_frec();
        prtTime.print_time('3');

        // std::cout << "Данные после модуляции:" << std::endl;
        // for (i = 0; i < N; ++i)
        //     std::cout << "I = " << mapDig[i][0] << " Q = " << mapDig[i][1] << std::endl;

    }
}