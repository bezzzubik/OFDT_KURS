/*
Обязательные флаги при компиляции: -lfftw3 -lm
*/
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<cstring>
#include<iostream>
#include<math.h>
#include <bitset>
#include <vector>
#include <complex>
#include <chrono>


constexpr int N = 21; // Число комплексных чисел, которые одновременно обрабатывает одна операция ОБПФ
					 // В будущем сюда добавятся комплексные числа для поддержания точности.
constexpr int DATA_LEN = 14191;
constexpr int COUNT_CADR = 2;		// Количество кадров
constexpr int COUNT_TESTS = 100;
constexpr double normalizeCoef = 1/sqrt(N);	// Нормализующий коэффициент0
constexpr int COUNT_SR_TIME = 100;

// #define RECOWER	// Раскомментировать если нужно провести обратный процесс

unsigned long timeStart; 


int i;
int iGl = 0;

void Data_produce(char*, int*);
void Symbol_produce(unsigned char*, char*);
std::complex<double> short_modulation(unsigned char);

int main()
{
	// запишем комплексные числа как двумерный массив float.
	std::vector<std::vector<std::vector<std::complex<double>>>> offtDataIn(17, std::vector<std::vector<std::complex<double>>>(N, std::vector<std::complex<double>>(N)));
	std::vector<std::complex<double>> offtDataOut(N);

	// Инициализируем начальный массив данных:
	for (i = 0; i < 17; ++i)
	{
		for (unsigned int n = 0; n < N; ++n)
		{

			for (unsigned int k = 0; k < N; ++k)
			{
				offtDataIn[i][n][k] = short_modulation(i) * std::exp(std::complex<double>(0, 2 * M_PI * k * n / static_cast<double>(N)));
			}
		}
	}

		unsigned char cadrs[COUNT_CADR];

		char buff[DATA_LEN] = "asdefrga";
		int maxLen = strlen(buff);
		// memset(buff, 0, sizeof(char)*DATA_LEN);

		printf("Введите строку\n");

		// Data_produce(buff, &maxLen);

		printf("Введенная строка: %s\nЧисло символов в строке: %d\n", buff, maxLen);

		// timeStart = time(NULL);
		long l = 0;
		long long glob = 0;
		int lss = 1;

		std::chrono::system_clock::time_point startTime;
		std::chrono::duration<double> srTime[COUNT_TESTS];
		std::chrono::duration<double> nowSrTime;

		for (lss = 0; lss < COUNT_TESTS; ++lss)
		{
			printf("Тест %d\n", lss);
			l = 0;
			startTime = std::chrono::system_clock::now();
			for(unsigned long long kk = 0; kk < COUNT_SR_TIME; ++kk, iGl = 0)
			while (iGl < maxLen)
			{
				if (iGl >= maxLen)
					iGl = 0;
				// printf("\nКадр: %ld\n", iGl/6+1);
				// printf("Строка обработки:\n");
				// for (i = (iGl/6) * 6; i < (iGl/6 + 1) * 6; i++)
				// 	printf("%c", buff[i]);
				// printf("\n");

				// printf("Данные, обработанные на этом этапе:\n");
				// for (i = (iGl/6) * 6; i < (iGl/6 + 1) * 6; i++)
				// 	printf("%8b\n", buff[i]);

				Symbol_produce(cadrs, buff);

				// for (i = 0; i < COUNT_CADR; i++)
				// 	std::cout << i << " "<< std::bitset<4>(cadrs[i]) << std::endl;

				for (i = 0; i < N; ++i)
					offtDataOut[i] = offtDataIn[cadrs[0]][i][0];

				for (unsigned int l = 0; l < N; ++l)
				{
					for (i = 1; i < COUNT_CADR; ++i)
					{
						offtDataOut[l] += offtDataIn[cadrs[i]][l][i];
					}
					for (; i < N; ++i)
					{
						offtDataOut[l] += offtDataIn[16][l][i];
					}
					offtDataOut[l] *= normalizeCoef;
				}

				// std::cout << "Rezult OFFT: \n ";
				// for (i = 0; i < N; ++i)
				// 	std::cout << "Real = " << offtDataOut[i].real() << " Mnim = " << offtDataOut[i].imag() << std::endl;

				++l;
			}
			printf("Число пройденных циклов: %ld\n", l);
			glob += l;
			srTime[lss] = std::chrono::duration<double>(std::chrono::system_clock::now() - startTime);
			nowSrTime += srTime[lss];
		}
		printf("Итоговое среднее число пройденных циклов: %lld\n", glob/COUNT_TESTS);
		std::cout << "Время прохождения:" << std::endl;

		for (i = 0; i < COUNT_TESTS; ++i)
			std::cout << srTime[i].count() << std::endl;
		std::cout << "Среднее время прохождения = " << nowSrTime.count() / COUNT_TESTS << std::endl;

        // Здесь делаем ОБПФ для каждого кадра по отдельности

	return 0;
}



// Тут просто будет ввод текста и ожидание подтверждения отправки.
void Data_produce(char* buff, int* maxLen)
{
	char c;
	int i = -1;
	while((buff[++i] = getchar()) !='\n');
	buff[i] = 0;
	*maxLen = i;
}


// Преобразование символов в кадры по 4 бита. (Проверено)
inline void Symbol_produce(unsigned char* cadrs, char* buff)
{
    for (i = 0; i < COUNT_CADR; ++i, ++iGl)
    {
        cadrs[i] = buff[iGl] & 0b1111;
        cadrs[++i] = buff[iGl] >> 4 & 0b1111;
    }
}




std::complex<double> short_modulation(unsigned char cadrs)
{
	std::complex<double> symbolCoding;
	switch (cadrs) {
		case 0b0000: symbolCoding.real(-3.0); symbolCoding.imag(3.0); break; // 0
		case 0b0001: symbolCoding.real(-1.0); symbolCoding.imag(3.0); break; // 1
		case 0b0010: symbolCoding.real(-3.0); symbolCoding.imag(1.0); break; // 2
		case 0b0011: symbolCoding.real(-1.0); symbolCoding.imag(1.0); break; // 3
		case 0b0100: symbolCoding.real(-3.0); symbolCoding.imag(3.0); break; // 4
		case 0b0101: symbolCoding.real(-1.0); symbolCoding.imag(3.0); break; // 5
		case 0b0110: symbolCoding.real(-3.0); symbolCoding.imag(1.0); break; // 6
		case 0b0111: symbolCoding.real(-1.0); symbolCoding.imag(1.0); break; // 7
		case 0b1000: symbolCoding.real( 3.0); symbolCoding.imag(3.0); break; // 8
		case 0b1001: symbolCoding.real( 1.0); symbolCoding.imag(3.0); break; // 9
		case 0b1010: symbolCoding.real( 3.0); symbolCoding.imag(1.0); break; // 10
		case 0b1011: symbolCoding.real( 1.0); symbolCoding.imag(1.0); break; // 11
		case 0b1100: symbolCoding.real( 3.0); symbolCoding.imag(3.0); break; // 12
		case 0b1101: symbolCoding.real( 1.0); symbolCoding.imag(3.0); break; // 13
		case 0b1110: symbolCoding.real( 3.0); symbolCoding.imag(1.0); break; // 14
		case 0b1111: symbolCoding.real( 1.0); symbolCoding.imag(1.0); break; // 15
		default: symbolCoding.real(0.0); symbolCoding.imag(0.0);
	}
	return symbolCoding;
}
