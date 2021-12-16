#pragma once

#include <chrono>

auto start_time = std::chrono::system_clock::now();

void delay(int ms);

double get_program_ticks_ms();
