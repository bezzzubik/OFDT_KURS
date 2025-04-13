#include <termios.h>
#include <unistd.h>
// #include <bitset>
// #include <climits>
#include "class_frec.hpp"
#include "generator.cpp"
#include "s2p.cpp"
#include "mapper.cpp"
#include "ifft.cpp"


using namespace std::chrono_literals;


constexpr int COUNT_THREAD = 16;



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



void p2s(std::stop_token stoken)
{

}



// main работает с терминальным выводом, обработкой статистики, управлением потоками
int main()
{
    std::cout << "\033[1;1H\033[2J";

    setNonCanonicalMode();

    std::jthread tgener(generate_bit, 2000000);
    std::jthread ts2p(s2p, 1000000);
    std::jthread tmap(mapper, 10000000);
    std::jthread tifft(ifft, 10000000);

    // Дожидаемся инициализации каждого потока
    while(!readyGener.load());
    std::cout << "Thread Generator is ready" << std::endl;

    while(!readyS2p.load());
    std::cout << "Thread S2p is ready" << std::endl;

    while(!readyMap.load());
    std::cout << "Thread Mapper is ready" << std::endl;

    while(!readyIFFT.load());
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
