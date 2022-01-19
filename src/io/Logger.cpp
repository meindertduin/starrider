#include "Logger.h"

#include <iostream>
#include <fstream>

#include <stdexcept>

std::unique_ptr<std::ofstream> Logger::log_output {nullptr};

void Logger::initialize(std::string output_file) {
    Logger::log_output = std::make_unique<std::ofstream>(output_file);
    *Logger::log_output << "Initializing Logging\n";
}

void Logger::log(LogLevel level, std::string value, bool log_cout) {
    std::string level_string;
    switch (level) {
        case LogLevel::Info:
            level_string = "Info: ";
            break;
        case LogLevel::Warning:
            level_string = "Warning: ";
            break;
        case LogLevel::Error:
            level_string = "Error: ";
            break;
        case LogLevel::Fatal:
            level_string = "Fatal: ";
            break;
    };

    *Logger::log_output << level_string << value << std::endl;
    std::cout << level_string << value << std::endl;
}

