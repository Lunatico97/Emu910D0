#include <iostream>
#include <iomanip>
#include <cassert>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <dirent.h>
#include <types.hpp>
#include <logger.hpp>
#include <SDL2/SDL.h>

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

namespace Global
{
    static bool debug = false;
    static const char* rom_path = "./roms/";

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

    static int get_length(const char *str)
    {
        int length = 0;
        while (str[length] != '\0') length++;
        return length;
    }

    static std::vector<const char*> scan_files(const char *directory, const char *ext)
    {
        std::vector<const char*> files;
        DIR* dptr = opendir(directory);
        if(dptr) 
        {
            dirent* dirent = readdir(dptr);
            while(dirent) 
            {
                if(dirent->d_type != DT_DIR)
                {
                    std::string filename = dirent->d_name;
                    if(filename.find(ext, (get_length(dirent->d_name)-get_length(ext))) != std::string::npos)
                    {
                        files.push_back(dirent->d_name);
                    }
                }
                dirent = readdir(dptr);
            }
            closedir(dptr);
        }
        return files;
    }
};

#endif