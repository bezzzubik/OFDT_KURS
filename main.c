#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<cstring>
#include<iostream>
// #define PI 3.14159265358979323846
// #define DATA_LEN 1000	// Buffer
// #define M 4				// Size optim
// #define COUNT_CADR 12               // Количество кадров
// #define symbol_len (DATA_LEN / M)
// #define EbN0_dB_MAX 15
#define DINAMIK

constexpr int DATA_LEN = 1000;
constexpr int M = 4;
constexpr int COUNT_CADR = 12;
constexpr int symbol_len = DATA_LEN/M;
// constexpr int EbN0_dB_MAX = 15;

unsigned long timeStart; 


int i;
int iGl = 0;

void Data_produce(char*, int*);
void Symbol_produce(unsigned char*, char*);
void Modulation(unsigned char*, double **);
void Modulation_16PSK(unsigned char *, double**);

int test_func(double**);

int main()
{
		// Перед вводом собрать план БПФ, вывести логи для подготовки.

		printf("Введите строку\n");
		int maxLen = 35;

		unsigned char cadrs[COUNT_CADR];

		char buff[DATA_LEN];
		memset(buff, 0, sizeof(char)*DATA_LEN);
		char buff_out[COUNT_CADR];
		unsigned char cadrs_out[COUNT_CADR];
		
		double** symbolCoding = static_cast<double**>(malloc(2 * sizeof(double*)));
		for (int i = 0; i < 2; ++i) {
			symbolCoding[i] = static_cast<double*>(malloc(COUNT_CADR * sizeof(double)));
		}
		Data_produce(buff, &maxLen);

		printf("Введенная строка: %s\nЧисло символов в строке: %d\n", buff, maxLen);


		timeStart = time(NULL);
		long l = 0;
		long long glob = 0;
		for (int lss = 1; lss < 51; ++lss)
		{
			printf("Тест %d\n", lss);
			l = 0;
			timeStart = time(NULL);
			// Выравнивание времени:
			while (timeStart == time(NULL));

			timeStart = time(NULL);

			while(timeStart == time(NULL))
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

				// printf("Обработанные данные:\n");
				// for (i = 0; i < COUNT_CADR; i++)
				// 	printf("%4b\n", cadrs[i]);

				Modulation(cadrs, symbolCoding);

				test_func(symbolCoding);
				// printf("Промодулированные данные на комплексную плоскость:\n");
				// for (i = 0; i < COUNT_CADR; i++)
				// 	printf("%4b = %+f %+fj\n", cadrs[i], symbolCoding[0][i], symbolCoding[1][i]);

				// Здесь идет работа ОБПФ и дальнейшая передача.

				// Demodulation();
				// printf("Демодулированные данные:\n");
				// for (int i = 0; i < COUNT_CADR; i++)
				// 	printf("%4b\n", cadrs_out[i]);
				
				// printf("\n\nДекодирование:\n");
				// Decoding_produse();
				// printf("Декодированные данные:\n");
				// for (int i = 0; i < COUNT_CADR/2; i++)
				// 	printf("%c\n", buff_out[i]);
				++l;
			}
			printf("Число пройденных циклов: %ld\n", l);
			glob += l;
		}
		printf("Итоговое среднее число пройденных циклов: %lld\n", glob/50);
        // Здесь делаем ОБПФ для каждого кадра по отдельности


        /*
		Gaussnoise(EbN0_dB);
		Demodulation();
		Erd = Erdproduce();
		Mrd = Mrdproduce();
		printf("Eb/N0 %d dB\t Symbol_Error: %f \t Bit_Error:%f \n", i, Mrd, Erd);
        */
	for (int i = 0; i < 2; ++i) {
		free (symbolCoding[i]);
	}
	free(symbolCoding);

	return 0;
}


inline int test_func(float ** sumbCod)
{
	int a =	sumbCod[0][5] + 2;
	std::cout << a;
	return a;
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

// Преобразование кадра в символы. (Проверено)
void Decoding_produse(unsigned char* cadrs_out, char* buff_out)
{
    int i, ss;
    for (i = 0, ss = 0; i < COUNT_CADR; i+=2, ++ss)
    {
		buff_out[ss] = cadrs_out[i] | (cadrs_out[i+1] << 4);
    }
}



// Модуляция. Реализованный 16-QAM алгоритм.
// Возможно требуется нормализация I и Q
// Попробовать преобразовать if-else во что-то более стабильное, либо в switch, либо в выражение
/* Таблица соответствия
Биты  | I  | Q  | Комплексное число (I + jQ)
------|----|----|---------------------------
0000  | -3 |  3 | -3 + 3j
0001  | -1 |  3 | -1 + 3j
0010  | -3 |  1 | -3 + 1j
0011  | -1 |  1 | -1 + 1j
0100  | -3 | -3 | -3 - 3j
0101  | -1 | -3 | -1 - 3j
0110  | -3 | -1 | -3 - 1j
0111  | -1 | -1 | -1 - 1j
1000  |  3 |  3 |  3 + 3j
1001  |  1 |  3 |  1 + 3j
1010  |  3 |  1 |  3 + 1j
1011  |  1 |  1 |  1 + 1j
1100  |  3 | -3 |  3 - 3j
1101  |  1 | -3 |  1 - 3j
1110  |  3 | -1 |  3 - 1j
1111  |  1 | -1 |  1 - 1j
 */
inline void Modulation(unsigned char * cadrs, double **symbolCoding)
{
    // Вариант 1: swithc-case.
    for (i = 0; i < COUNT_CADR; ++i)
    {
        switch (cadrs[i]) {
            case 0b0000: symbolCoding[0][i] = -3.0; symbolCoding[1][i] =  3.0; break; // 0
            case 0b0001: symbolCoding[0][i] = -1.0; symbolCoding[1][i] =  3.0; break; // 1
            case 0b0010: symbolCoding[0][i] = -3.0; symbolCoding[1][i] =  1.0; break; // 2
            case 0b0011: symbolCoding[0][i] = -1.0; symbolCoding[1][i] =  1.0; break; // 3
            case 0b0100: symbolCoding[0][i] = -3.0; symbolCoding[1][i] = -3.0; break; // 4
            case 0b0101: symbolCoding[0][i] = -1.0; symbolCoding[1][i] = -3.0; break; // 5
            case 0b0110: symbolCoding[0][i] = -3.0; symbolCoding[1][i] = -1.0; break; // 6
            case 0b0111: symbolCoding[0][i] = -1.0; symbolCoding[1][i] = -1.0; break; // 7
            case 0b1000: symbolCoding[0][i] =  3.0; symbolCoding[1][i] =  3.0; break; // 8
            case 0b1001: symbolCoding[0][i] =  1.0; symbolCoding[1][i] =  3.0; break; // 9
            case 0b1010: symbolCoding[0][i] =  3.0; symbolCoding[1][i] =  1.0; break; // 10
            case 0b1011: symbolCoding[0][i] =  1.0; symbolCoding[1][i] =  1.0; break; // 11
            case 0b1100: symbolCoding[0][i] =  3.0; symbolCoding[1][i] = -3.0; break; // 12
            case 0b1101: symbolCoding[0][i] =  1.0; symbolCoding[1][i] = -3.0; break; // 13
            case 0b1110: symbolCoding[0][i] =  3.0; symbolCoding[1][i] = -1.0; break; // 14
            case 0b1111: symbolCoding[0][i] =  1.0; symbolCoding[1][i] = -1.0; break; // 15
            default:   symbolCoding[0][i] = 0.0; symbolCoding[1][i] = 0.0; // Обработка неожиданной ситуации (опционально)
        }
    }

    /*Вариант 2: Использование выражения*/
    /*
    for (i = 0; i < COUNT_CADR; i++)
    {
        symbolCoding[0][i] = (cadrs[i] >> 3) * 2 - 1;
        symbolCoding[1][i] = ((cadrs[i] & 0b0010) >> 1) * 2 - 1;

        if ((cadrs[i] & 0b0100)) symbolCoding[0][i] = symbolCoding[0][i] * 3;

        if ((cadrs[i] & 0b0001)) symbolCoding[1][i] = symbolCoding[1][i] * 3;
    }
    */
}


/* Таблица соответствия для 16-PSK
Биты  | I  | Q  
------|----|----
0000  | -3 |  3 	
0001  | -3 |  2.25 	
0010  | -3 |  1.5 	
0011  | -3 |  0.75 	
0100  | -3 | -3 	
0101  | -3 | -2.25  
0110  | -3 | -1.5 	
0111  | -3 | -0.75  
1000  |  3 |  3 	
1001  |  3 |  2.25  
1010  |  3 |  1.5 	
1011  |  3 |  0.75  
1100  |  3 | -3 	
1101  |  3 | -2.25  
1110  |  3 | -1.5 	
1111  |  3 | -0.75 
*/
void Modulation_16PSK(unsigned char * cadrs, float** symbolCoding)
{
    // Вариант 1: swithc-case.
    for (i = 0; i < COUNT_CADR; ++i)
    {
        switch (cadrs[i]) {
            case 0b0000: symbolCoding[0][i] = -3.0; symbolCoding[1][i] =  3.0; break; // 0
            case 0b0001: symbolCoding[0][i] = -3.0; symbolCoding[1][i] =  2.25; break; // 1
            case 0b0010: symbolCoding[0][i] = -3.0; symbolCoding[1][i] =  1.5; break; // 2
            case 0b0011: symbolCoding[0][i] = -3.0; symbolCoding[1][i] =  0.75; break; // 3
            case 0b0100: symbolCoding[0][i] = -3.0; symbolCoding[1][i] = -3.0; break; // 4
            case 0b0101: symbolCoding[0][i] = -3.0; symbolCoding[1][i] = -2.25; break; // 5
            case 0b0110: symbolCoding[0][i] = -3.0; symbolCoding[1][i] = -1.5; break; // 6
            case 0b0111: symbolCoding[0][i] = -3.0; symbolCoding[1][i] = -0.75; break; // 7
            case 0b1000: symbolCoding[0][i] =  3.0; symbolCoding[1][i] =  3.0; break; // 8
            case 0b1001: symbolCoding[0][i] =  3.0; symbolCoding[1][i] =  2.25; break; // 9
            case 0b1010: symbolCoding[0][i] =  3.0; symbolCoding[1][i] =  1.5; break; // 10
            case 0b1011: symbolCoding[0][i] =  3.0; symbolCoding[1][i] =  0.75; break; // 11
            case 0b1100: symbolCoding[0][i] =  3.0; symbolCoding[1][i] = -3.0; break; // 12
            case 0b1101: symbolCoding[0][i] =  3.0; symbolCoding[1][i] = -2.25; break; // 13
            case 0b1110: symbolCoding[0][i] =  3.0; symbolCoding[1][i] = -1.5; break; // 14
            case 0b1111: symbolCoding[0][i] =  3.0; symbolCoding[1][i] = -0.75; break; // 15
            default:   symbolCoding[0][i] = 0.0; symbolCoding[1][i] = 0.0; // Обработка неожиданной ситуации (опционально)
        }
    }
}



void Demodulation(unsigned char * cadrs_out, double** symbolCoding)
{
	for (int i = 0; i < COUNT_CADR; ++i)
	{
		if (symbolCoding[0][i] < 0)
		{
			cadrs_out[i] = 0b0000;
		}
		else 
		{
			cadrs_out[i] = 0b1000;
		}


		if (symbolCoding[1][i] < -2.625)
		{
			cadrs_out[i] |= 0b0100;
		}
		else if (symbolCoding[1][i] > 2.625)
		{
			cadrs_out[i] |= 0b0000;
		}
		else if (symbolCoding[1][i] > 1.875)
		{
			cadrs_out[i] |= 0b0001;
		}
		else if (symbolCoding[1][i] > 1.125)
		{
			cadrs_out[i] |= 0b0010;
		}
		else if (symbolCoding[1][i] > 0)
		{
			cadrs_out[i] |= 0b0010;
		}
		else if (symbolCoding[1][i] < -1.875)
		{
			cadrs_out[i] |= 0b0101;
		}
		else if (symbolCoding[1][i] < -1.125)
		{
			cadrs_out[i] |= 0b0110;
		}
		else
		{
			cadrs_out[i] |= 0b0111;
		}
	}
}


void Demodulation_16PSK(unsigned char * cadrs_out, double** symbolCoding)
{
	for (int i = 0; i < COUNT_CADR; ++i)
	{
		if (symbolCoding[1][i] < -2.0)
		{
			cadrs_out[i] = 0b0000;
		}
		else if (symbolCoding[0][i] > 2.0)
		{
			cadrs_out[i] = 0b1000;
		}
		else if (symbolCoding[0][i] < 2.0 && symbolCoding[0][i] > 0.0)
		{
			cadrs_out[i] = 0b1001;
		}
		else
		{
			cadrs_out[i] = 0b0001;
		}


		if (symbolCoding[1][i] < -2.0)
		{
			cadrs_out[i] |= 0b0100;
		}
		else if (symbolCoding[1][i] > 2.0)
		{
			cadrs_out[i] |= 0b0000;
		}
		else if (symbolCoding[1][i] < 2.0 && symbolCoding[1][i] > 0.0)
		{
			cadrs_out[i] |= 0b0010;
		}
		else
		{
			cadrs_out[i] |= 0b0110;
		}
	}
}







/*
double Gauss()
{
	double U, V, Z;

	U = (double)(rand() + 1.0) / (double)(RAND_MAX + 1.0);
	V = (double)(rand() + 1.0) / (double)(RAND_MAX + 1.0);
	Z = sqrt(-2 * log(U)) * sin(2 * PI * V);

	return Z;
}

void Gaussnoise(double EbN0_dB)
{
	double EbN0 = pow(10, EbN0_dB / 10);
	double N0 = Eb / EbN0;
	double sigma = sqrt(N0 / 2);

	for (int i = 0; i < symbol_len; i++)
	{
		symbolCoding[0][i] = symbolCoding[0][i] + Gauss() * sigma;
		symbolCoding[1][i] = symbolCoding[1][i] + Gauss() * sigma;
	}
}


double Erdproduce()
{
	double Erd;
	int Nrd = 0;
	for (int i = 0; i < data_len; i++)
	{
		if (data_decoding[i] != data_bfcoding[i])
			Nrd++;
	}
	Erd = (double)Nrd / (double)data_len;
	return Erd;
}

double Mrdproduce()
{
	double Mrd;
	int Srd = 0;
	int p[M];
	int pt;
	for (int i = 0; i < symbol_len; i++)
	{
		int pt = 0;
		for (int j = 0; j < M; j++)
		{
			if (symbol_decoding[j][i] == symbol_bfcoding[j][i])
				p[j] = 0;
			else
				p[j] = 1;
		}
		for (int j = 0; j < M; j++)
		{
			pt = pt + p[j];
		}

		if (pt > 0)
			Srd++;
	}
	Mrd = (double)Srd / (double)(symbol_len);
	return Mrd;
}
*/
