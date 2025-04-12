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
#include<fftw3.h>
#include<math.h>
#define PRINT_TIME
// #define TAKTING



constexpr int COUNT_THREAD = 16;
constexpr int N = 16; // Число комплексных чисел, которые одновременно обрабатывает одна операция ОБПФ
constexpr double normalizeCoef = 1.0/sqrt(N);	// Нормализующий коэффициент
using namespace std::chrono_literals;
unsigned int bitGl;
unsigned char bit4Gl[N];
std::mutex print;
std::atomic<bool> newGener(false);
std::atomic<bool> newS2p(false);
std::atomic<bool> newMap(false);

std::atomic<bool> readyGener(false);
std::atomic<bool> readyS2p(false);
std::atomic<bool> readyMap(false);
std::atomic<bool> readyIFFT(false);

std::atomic<bool> startWork(false);
std::atomic<bool> flagSinh(false);


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
            std::this_thread::sleep_until(Hz + std::chrono::duration<double>(startTime));
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


constexpr int PERIOD_PRINT = 500000;
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
        }

        void start_time()
        {
            startTime = std::chrono::system_clock::now();
        }

        void start_time_all_work()
        {
            allStartTime = std::chrono::system_clock::now();
        }

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
        }

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

        ~TimePrt()
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
    unsigned int startGenerate[100];
    
    for (i = 0; i < 100; ++i)
        startGenerate[i] = std::rand() | (std::rand() % 2) << 32;

    unsigned int nowBit;
    readyGener.store(true);
    while (!stoken.stop_requested())
    {
        while(!startWork.load());
        HzTh.start_frec();
        nowBit = startGenerate[i];

        // std::this_thread::sleep_for(1s);
        while (newGener.load());

        bitGl = nowBit;

        newGener.store(true);
        i = (i + 1) % 100;

        HzTh.end_frec();
        // std::cout << "Сгенерированные биты: " << std::bitset<N * 4>(bitGl) << std::endl;

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
    // добавить какой-нибудь поток, который бы запускал работу всех потоков
    readyS2p.store(true);
    while (!stoken.stop_requested())
    {
        while(!startWork.load());

        HzTh.start_frec();
        prtTime.start_time_all_work();
        while (!newGener.load());
        
        bitLoc = bitGl;
        newGener.store(false);

        while (newS2p.load());
        prtTime.start_time();


        for (i = 0; i < N; ++i)
        {
            bit4Gl[i] = bitLoc & 0b1111;
            bitLoc = bitLoc >> 4;
        }
        newS2p.store(true);
        prtTime.print_time('2');
        HzTh.end_frec();
        // Передача в следующий блок.

        // std::cout << "Преобразованные биты в s2p:" << std::endl;
        // for (i = 0; i < N; ++i)
        //     std::cout << std::bitset<4>(bit4[i]) << std::endl;
    }
}


double mapDig[N][2];

void mapper(std::stop_token stoken, unsigned long long Hz)
{
    FrecTh HzTh(Hz);
    unsigned char bit4Loc[N];

    unsigned char i = 0;
    TimePrt prtTime;
    readyMap.store(true);
    while (!stoken.stop_requested())
    {
        while(!startWork.load());
        prtTime.start_time_all_work();
        HzTh.start_frec();
        while (!newS2p.load());
        memcpy(bit4Loc, bit4Gl, sizeof(unsigned char)*N);
        // for (i = 0; i < N; ++i)
        //     bit4Loc[i] = bit4Gl[i];
        newS2p.store(false);

        while (newMap.load());
        prtTime.start_time();

        // std::this_thread::sleep_for(1s);
        for (i = 0; i < N; ++i)
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
        newMap.store(true);
        prtTime.print_time('3');

        HzTh.end_frec();
        // std::cout << "Данные после модуляции:" << std::endl;
        // for (i = 0; i < N; ++i)
        //     std::cout << "I = " << mapDig[i][0] << " Q = " << mapDig[i][1] << std::endl;

    }
}



void ifft(std::stop_token stoken, unsigned long long Hz)
{
    FrecTh HzTh(Hz);
    TimePrt prtTime;
    int i;
    fftw_complex *offtDataIn = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    fftw_complex *offtDataOut = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);

    memset(offtDataIn, 0, sizeof(fftw_complex) * N);
    memset(offtDataOut, 0, sizeof(fftw_complex) * N);

    fftw_plan planBack;
    // Заранее создаем план для ОБПФ. 
    planBack = fftw_plan_dft_1d(N, offtDataIn, offtDataOut, FFTW_BACKWARD, FFTW_EXHAUSTIVE | FFTW_DESTROY_INPUT);

    readyIFFT.store(true);
    while (!stoken.stop_requested())
    {
        while(!startWork.load());
        HzTh.start_frec();
        prtTime.start_time_all_work();
        while (!newMap.load());
        for (i = 0; i < N; ++i)
        {
            offtDataIn[i][0] = mapDig[i][0];
            offtDataIn[i][1] = mapDig[i][1];
        }
        newMap.store(false);

        prtTime.start_time();

        // ОБПФ
        fftw_execute(planBack);
        // Нормализация. В будущем заменить в зависимости от параметров приемника-передатчика
        for (i = 0; i < N; ++i)
        {
            offtDataOut[i][0] *= normalizeCoef;
            offtDataOut[i][1] *= normalizeCoef;
        }
        prtTime.print_time('4');
        HzTh.end_frec();
    }
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
    std::cout << "\033[1;1H\033[2J";

    setNonCanonicalMode();

    std::jthread tgener(generate_bit, 2000000);
    std::jthread ts2p(s2p, 20000);
    std::jthread tmap(mapper, 10000000);
    std::jthread tifft(ifft, 10000000);

    // Дожидаемся инициализации каждого потока
    while(!readyGener.load()) std::this_thread::sleep_for(0.001s);
    std::cout << "Thread Generator is ready" << std::endl;

    while(!readyS2p.load()) std::this_thread::sleep_for(0.001s);
    std::cout << "Thread S2p is ready" << std::endl;

    while(!readyMap.load()) std::this_thread::sleep_for(0.001s);
    std::cout << "Thread Mapper is ready" << std::endl;

    while(!readyIFFT.load()) std::this_thread::sleep_for(0.001s);
    std::cout << "Thread IFFT is ready" << std::endl;

    std::cout << "\n\nAll thread is ready" << std::endl;
    startWork.store(true);

    bool flagWork = true;
    // std::this_thread::sleep_for(0.2s);
    while (flagWork)
    {
        char c;
        if (read(STDIN_FILENO, &c, 1) > 0)
        {
            switch (c)
            {
                case 'h':
                    flagSinh.store(!flagSinh.load());
                    print.lock();
                    std::cout << "\033[10;1HНажат флаг синхронизации" << std::endl;
                    print.unlock();
                    break;
                case 's':
                    startWork.store(!startWork.load());
                    break;

                case 'q':       // Завершение работы программы
                    tgener.request_stop();
                    ts2p.request_stop();
                    tmap.request_stop();
                    tifft.request_stop();
                    // отпарвить всем запросы на остановки

                    // обнулить все атомики
                    startWork.store(!startWork.load());
                    newGener.store(!newGener.load());
                    newS2p.store(!newS2p.load());
                    newMap.store(!newMap.load());

                    // потом все остальные
                    std::this_thread::sleep_for(0.01s);

                    startWork.store(!startWork.load());
                    newGener.store(!newGener.load());
                    newS2p.store(!newS2p.load());
                    newMap.store(!newMap.load());

                    // дождаться конца работы потоков
                    tgener.join();
                    ts2p.join();
                    tmap.join();
                    tifft.join();
                    flagWork = false;
                    std::cout << "\033[10;1H" << std::endl;
                    break;
                                
                default:
                    break;
            }
        }
    }

    restoreCanonicalMode();
    return 0;
}
