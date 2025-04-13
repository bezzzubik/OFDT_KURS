#include "need_libs.hpp"
#include "class_frec.hpp"
#include "class_prt.hpp"
#include <ctime>


std::atomic<bool> readyGener(false);


void generate_bit(std::stop_token stoken, unsigned long long Hz)
{
    FrecTh HzTh(Hz);
    std::srand(std::time(NULL));
    unsigned int i = 0;
    // std::this_thread::sleep_for(0.1s);
    unsigned int startGenerate[100];
    
    for (i = 0; i < 100; ++i)
        startGenerate[i] = std::rand() | (std::rand() % 2) << 32;

    unsigned int nowBit;
    readyGener.store(true);
    while(!startWork.load());
    while (!stoken.stop_requested())
    {
        // HzTh.start_frec();
        nowBit = startGenerate[i];

        // std::this_thread::sleep_for(1s);
        while (newGener.load());

        bitGl = nowBit;

        newGener.store(true);
        i = (i + 1) % 100;

        // HzTh.end_frec();
        // std::cout << "Сгенерированные биты: " << std::bitset<N * 4>(bitGl) << std::endl;

        // print.lock();
        // prtTime.print_time('1');
        // print.unlock();
     }
}