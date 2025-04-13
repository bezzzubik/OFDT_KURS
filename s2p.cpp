#include "need_libs.hpp"
#include "class_frec.hpp"
#include "class_prt.hpp"

std::atomic<bool> readyS2p(false);


inline void read_chanel(unsigned int* bitLoc)
{
    *bitLoc = bitGl;
}

inline void main_calculate(unsigned int bitLoc)
{
    for (int i = 0; i < N; ++i)
    {
        bit4Gl[i] = bitLoc & 0b1111;
        bitLoc = bitLoc >> 4;
    }
}



void s2p(std::stop_token stoken, unsigned long long Hz)
{
    FrecTh HzTh(Hz);
    unsigned int bitLoc;
    unsigned char i = 0;
    TimePrt prtTime;
    // добавить какой-нибудь поток, который бы запускал работу всех потоков
    readyS2p.store(true);
    while(!startWork.load());
    while (!stoken.stop_requested())
    {

        HzTh.start_frec();
        prtTime.start_time_all_work();
        while (!newGener.load());
        
        read_chanel(&bitLoc);

        newGener.store(false);

        while (newS2p.load());
        prtTime.start_time();

        main_calculate(bitLoc);

        newS2p.store(true);
        HzTh.end_frec();
        prtTime.print_time('2');
        // Передача в следующий блок.

        // std::cout << "Преобразованные биты в s2p:" << std::endl;
        // for (i = 0; i < N; ++i)
        //     std::cout << std::bitset<4>(bit4[i]) << std::endl;
    }
}
