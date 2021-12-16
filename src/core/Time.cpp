#include "Time.h"
#include <thread>

time_point start_time = std::chrono::system_clock::now();

void delay(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

double get_program_ticks_ms() {
    std::chrono::duration<double, std::milli> timespan =  std::chrono::system_clock::now() - start_time;
    return timespan.count();
}
