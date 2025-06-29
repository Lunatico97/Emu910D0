#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <unistd.h>
#include <map>
#include <types.hpp>
#include <SDL2/SDL.h>

#ifndef _UTILS_H_
#define _UTILS_H_

namespace Utils
{
    static std::string readTextFromFile(const char* filepath) {
        std::ifstream inputFile(filepath);
        if (inputFile.is_open()) 
        {
            std::ostringstream ss;
            ss << inputFile.rdbuf();
            inputFile.close();
            return ss.str();
        } 
        else 
        {
            std::cerr << "Error: Could not open file '" << filepath << "'" << std::endl;
            return "";
        }
    }

    static int writeTextToFile(const char* filepath, const std::string& input_str) {
        std::ofstream outputFile(filepath);
        if (outputFile.is_open()) 
        {
            outputFile << input_str;
            outputFile.close();
            return 1;
        } 
        else 
        {
            std::cerr << "Error: Could not open file '" << filepath << "'" << std::endl;
            return -1;
        }
    }

    static std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\n\r\f\v");
        if (std::string::npos == first) 
        {
            return str;
        }
        size_t last = str.find_last_not_of(" \t\n\r\f\v");
        return str.substr(first, (last - first + 1));
    }

    static void logU8(const char* r, u8 value)
    {
        std::cout << std::uppercase << r << "[" << std::hex << std::setfill('0') << std::setw(2) << (value & 0xFF) << "] ";
    }

    static void logU16(const char* r, u16 value)
    {
        std::cout << std::uppercase << r << "[" << std::hex << std::setfill('0') << std::setw(4) << (value & 0xFFFF) << "] ";
    }

    static void logHEX(HEX value)
    {
        std::cout << "< ";
        logU8("OP", value.h8[0]);
        logU8("LN8", value.h8[1]);
        logU8("HN8", value.h8[2]);
        std::cout << ">" << std::endl;
    }
};

#endif