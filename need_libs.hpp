#pragma once
#include <iostream>
#include <thread>
#include <atomic>

std::atomic<bool> newGener(false);
std::atomic<bool> newS2p(false);
std::atomic<bool> newMap(false);

std::atomic<bool> startWork(false);
std::atomic<bool> flagSinh(false);

constexpr int N = 16; // Число комплексных чисел, которые одновременно обрабатывает одна операция ОБПФ

unsigned int bitGl;
unsigned char bit4Gl[N];
double mapDig[N][2];