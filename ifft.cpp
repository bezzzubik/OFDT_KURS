#include "need_libs.hpp"
#include "class_frec.hpp"
#include "class_prt.hpp"
#include <fftw3.h>
#include <math.h>

std::atomic<bool> readyIFFT(false);

constexpr double normalizeCoef = 1.0/sqrt(N);	// Нормализующий коэффициент

inline void read_chanel(fftw_complex* offtDataIn)
{
    int i;
    for (i = 0; i < N; ++i)
    {
        offtDataIn[i][0] = mapDig[i][0];
        offtDataIn[i][1] = mapDig[i][1];
    }
}

inline void main_calculate(fftw_complex* offtDataOut, fftw_plan* planBack)
{
    int i;
    // ОБПФ
    fftw_execute(*planBack);
    // Нормализация. В будущем заменить в зависимости от параметров приемника-передатчика
    for (i = 0; i < N; ++i)
    {
        offtDataOut[i][0] *= normalizeCoef;
        offtDataOut[i][1] *= normalizeCoef;
    }
}

void ifft(std::stop_token stoken, unsigned long long Hz)
{
    FrecTh HzTh(Hz);
    TimePrt prtTime;
    fftw_complex *offtDataIn = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    fftw_complex *offtDataOut = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);

    fftw_plan planBack;
    // Заранее создаем план для ОБПФ. 
    planBack = fftw_plan_dft_1d(N, offtDataIn, offtDataOut, FFTW_BACKWARD, FFTW_EXHAUSTIVE | FFTW_DESTROY_INPUT);

    readyIFFT.store(true);
    while(!startWork.load());
    while (!stoken.stop_requested())
    {
        HzTh.start_frec();
        prtTime.start_time_all_work();
        while (!newMap.load());

        read_chanel(offtDataIn);

        newMap.store(false);

        prtTime.start_time();

        main_calculate(offtDataOut, &planBack);

        HzTh.end_frec();
        prtTime.print_time('4');
    }
}