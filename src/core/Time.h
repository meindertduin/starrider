#pragma once

#include <chrono>

typedef std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<long, std::ratio<1, 1000000000>>> time_point;

extern time_point start_time;

void delay(int ms);

double get_program_ticks_ms();
