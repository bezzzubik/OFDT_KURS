#include <iostream>
#include <thread>
#include <chrono>
#include <termios.h>
#include <unistd.h>
#include <atomic>
#include <ctime>
#include <bitset>
#include <mutex>
#include <cstring>
#include <climits>
// #define PRINT_TIME



constexpr int COUNT_THREAD = 16;
constexpr int PERIOD_PRINT = 500000;
using namespace std::chrono_literals;
unsigned long bitGl;
unsigned char bit4Gl[COUNT_THREAD];
std::mutex print;
std::atomic<bool> newGener(false);
std::atomic<bool> newS2p(false);
std::atomic<bool> newMap(false);


class FrecTh
{
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

        void end_frec()
        {
            if (Hz.count() - std::chrono::duration<double>(std::chrono::system_clock::now() - startTime).count() > 0)
                std::this_thread::sleep_for(Hz - std::chrono::duration<double>(std::chrono::system_clock::now() - startTime));
            startTime = std::chrono::system_clock::now();
        }

        ~FrecTh()
        {

        }

};




class TimePrt
{
    #ifdef PRINT_TIME
    private:
        std::chrono::system_clock::time_point startTime;
        std::chrono::duration<double> srTime;
        unsigned int period = 0;
    public:
        TimePrt()
        {
            startTime = std::chrono::system_clock::now();
            srTime = std::chrono::duration<double>(0);
        }

        void start_time()
        {
            startTime = std::chrono::system_clock::now();
        }

        void print_time(char k)
        {
            srTime = std::chrono::duration<double>(std::chrono::system_clock::now() - startTime);
            print.lock();
            if ( ++period < PERIOD_PRINT )
            {
                print.unlock();
                return;
            }
            period = 0;

            std::cout << "\033[" << k << ";1H" << "\033[2K" << "\033[" << k << ";1H";
            switch (k)
            {
            case '1':
                std::cout << "Generate_bit = ";
                break;
            case '2':
                std::cout << "s2p = ";
                break;
            case '3':
                std::cout << "mapper = ";
                break;
            case '4':
                std::cout << "ifft = ";
                break;
            case '5':
                std::cout << "p2s = ";
                break;
            default:
                break;
            }

            std::cout << srTime.count();

            std::cout << "\033[0m" << std::endl;

            print.unlock();
            return;
        };

        ~TimePrt()  // k - номер блока
        {
        };
    #else
    public:
        TimePrt(){};
        ~TimePrt(){};
        void print_time(char k){};
        void start_time(){};
    #endif
};


// Установка неканониеского ввода в терминал (работает только для Linux!!!!!!!!!!!!!)
void setNonCanonicalMode()
{
    termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

// Восстановка канонического режима ввода
void restoreCanonicalMode()
{
    termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}


// false
void generate_bit(std::stop_token stoken, unsigned long long Hz)
{
    FrecTh HzTh(Hz);
    std::srand(std::time(NULL));
    unsigned int i = 0;
    // std::this_thread::sleep_for(0.1s);
    while (!stoken.stop_requested())
    {
        // std::this_thread::sleep_for(1s);
        while (newGener.load());
        bitGl = std::rand() % 2;
        for (i = 1; i < COUNT_THREAD * 4; ++i)
            bitGl |= std::rand() % 2 << i;
        newGener.store(true);
        HzTh.end_frec();
        // std::cout << "Сгенерированные биты: " << std::bitset<COUNT_THREAD * 4>(bitGl) << std::endl;

        // print.lock();
        // prtTime.print_time('1');
        // print.unlock();
     }
}

// true
void s2p(std::stop_token stoken, unsigned long long Hz)
{
    FrecTh HzTh(Hz);
    unsigned int bitLoc;
    unsigned char i = 0;
    TimePrt prtTime;
    // std::this_thread::sleep_for(0.1s);
    while (!stoken.stop_requested())
    {
        while (!newGener.load());
        while (newS2p.load());

        // std::this_thread::sleep_for(1s);
        prtTime.start_time();
        bitLoc = bitGl;
        newGener.store(false);
        for (i = 0; i < COUNT_THREAD; ++i)
        {
            bit4Gl[i] = bitLoc & 0b1111;
            bitLoc = bitLoc >> 4;
        }
        newS2p.store(true);
        HzTh.end_frec();
        prtTime.print_time('2');

        // Передача в следующий блок.

        // std::cout << "Преобразованные биты в s2p:" << std::endl;
        // for (i = 0; i < COUNT_THREAD; ++i)
        //     std::cout << std::bitset<4>(bit4[i]) << std::endl;
    }
}



void mapper(std::stop_token stoken, unsigned long long Hz)
{
    FrecTh HzTh(Hz);
    unsigned char bit4Loc[COUNT_THREAD];

    unsigned char i = 0;
    double mapDig[COUNT_THREAD][2];
    TimePrt prtTime;
    // std::this_thread::sleep_for(0.1s);
    while (!stoken.stop_requested())
    {
        while (!newS2p.load());
        prtTime.start_time();
        memcpy(bit4Loc, bit4Gl, sizeof(unsigned char)*COUNT_THREAD);
        // for (i = 0; i < COUNT_THREAD; ++i)
        //     bit4Loc[i] = bit4Gl[i];
        newS2p.store(false);
        // std::this_thread::sleep_for(1s);
        for (i = 0; i < COUNT_THREAD; ++i)
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

        HzTh.end_frec();
        prtTime.print_time('3');

        // std::cout << "Данные после модуляции:" << std::endl;
        // for (i = 0; i < COUNT_THREAD; ++i)
        //     std::cout << "I = " << mapDig[i][0] << " Q = " << mapDig[i][1] << std::endl;

    }
}


void ifft(std::stop_token stoken, int k)
{

}


void p2s(std::stop_token stoken)
{

}


void func1(std::stop_token stoken)
{
    char c;
    while (!stoken.stop_requested())
    {
        std::cout << "work..." << std::endl;
        std::this_thread::sleep_for(0.2s);
    }
    std::cout << "End work" << std::endl;
}


// main работает с терминальным выводом, обработкой статистики, управлением потоками
int main()
{
    std::cout << "\033[2J";
    setNonCanonicalMode();

    std::jthread tgener(generate_bit, 2000000);
    std::jthread ts2p(s2p, 20000);
    std::jthread tmap(mapper, 10000000);

    bool flagWork = true;
    // std::this_thread::sleep_for(0.2s);
    while (flagWork)
    {
        char c;
        if (read(STDIN_FILENO, &c, 1) > 0)
        {
            switch (c)
            {
                case 'q':
                    tgener.request_stop();
                    ts2p.request_stop();
                    tmap.request_stop();
                    // отпарвить всем запросы на остановки

                    // обнулить все атомики
                    newGener.store(!newGener.load());
                    newS2p.store(!newS2p.load());
                    newMap.store(!newMap.load());

                    // потом все остальные
                    std::this_thread::sleep_for(0.01s);
                    newGener.store(!newGener.load());
                    newS2p.store(!newS2p.load());
                    newMap.store(!newMap.load());

                    // дождаться конца работы потоков
                    tgener.join();
                    ts2p.join();
                    tmap.join();
                    flagWork = false;
                    break;
                                
                default:
                    break;
            }
        }
    }

    restoreCanonicalMode();
    return 0;
}
