#pragma once

#include <string>
#include <iostream>
#include <memory>

enum class LogLevel {
    Info,
    Warning,
    Error,
    Fatal,
};

class Logger {
public:
    /* will initialize log file and overwrite if already exists */
    static void initialize(std::string output_file);

    /*
     * Will write log output with warning statement to output file.
     * May also log to console output when log_cout set to true.
    */
    static void log(LogLevel level, std::string value, bool log_cout);
private:
    static std::unique_ptr<std::ofstream> log_output;
};
