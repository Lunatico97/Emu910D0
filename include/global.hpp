#include <iostream>
#include <iomanip>
#include <cassert>
#include <string>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <map>
#include <types.hpp>
#include <logger.hpp>
#include <SDL2/SDL.h>

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

namespace Global
{
    static bool debug = false;

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
            std::cerr << "Error: Could not open file '" << filepath << "'\n";
            return "";
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
};

#endif