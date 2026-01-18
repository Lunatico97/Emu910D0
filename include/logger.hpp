#include <types.hpp>

#ifndef __LOGGER_H__
#define __LOGGER_H__

struct Logger
{
    const char* logpath;

    int init(const char* filepath)
    {
        logpath = filepath;
        std::ofstream filelogger(filepath, std::ios_base::out | std::ios_base::trunc);
        if (filelogger.is_open()) 
        {
            filelogger.close();
            return 1;
        } 
        else 
        {
            std::cerr << "Error: Could not open file '" << filepath << "'\n";
            return -1;
        }
    }

    void log(const std::string& msg)
    {
        std::ofstream filelogger(logpath, std::ios_base::out | std::ios_base::app);
        filelogger << msg;
        filelogger.close();
    }

    void log(const std::string& msg, const u8& value)
    {
        std::ofstream filelogger(logpath, std::ios_base::out | std::ios_base::app);
        filelogger << std::uppercase << msg << std::hex << std::setfill('0') << std::setw(2) << (value & 0xFF) << " ";
        filelogger.close();
    }

    void log(const std::string& msg, const u16 value, bool newline)
    {
        std::ofstream filelogger(logpath, std::ios_base::out | std::ios_base::app);
        filelogger << "CYC:" << value << "\n";
        filelogger.close();
    }

    void log(const std::string& msg, const u16& value)
    {
        std::ofstream filelogger(logpath, std::ios_base::out | std::ios_base::app);
        filelogger << std::uppercase << msg << std::hex << std::setfill('0') << std::setw(4) << (value & 0xFFFF) << " ";
        filelogger.close();
    }

    void log(const HEX& value)
    {
        log("", value.h8[0]);
        log("", value.h8[1]);
        log("", value.h8[2]);
    }
};

extern Logger logger;

#endif