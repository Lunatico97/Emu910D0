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
    inline int scr_w;
    inline int scr_h;
    static bool debug = false;
    static const char* rom_path = "roms/";

    static std::string readTextFromFile(const char* filepath) 
    {
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

    static void readDataFromFile(const char *filepath, u8* data, u16 size)
    {
        std::ifstream inputFile(filepath, std::ios::binary);
        if (inputFile.is_open()) 
        {
            inputFile.read(reinterpret_cast<char*>(data), size);
        } 
        else 
        {
            std::cerr << "Error: Could not open file: '" << filepath << "'\n";
        }
    }

    static void writeDataToFile(const char *filepath, u8* data, u16 size)
    {
        std::ofstream outputFile(filepath, std::ios::binary | std::ios::trunc);
        if (outputFile.is_open()) 
        {
            outputFile.write(reinterpret_cast<const char*>(data), size);
        } 
        else 
        {
            std::cerr << "Error: Could not create file: '" << filepath << "'\n";
        }
    }

    static std::string trim(const std::string& str) 
    {
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

    static std::vector<std::string> scan_files(const char *directory, const char *ext)
    {
        std::vector<std::string> files;
        DIR* dptr = opendir(directory);
        if(dptr) 
        {
            dirent* dirent = readdir(dptr);
            while(dirent != NULL) 
            {
                std::string filename = dirent->d_name;
                if(filename.find(ext, (get_length(dirent->d_name)-get_length(ext))) != std::string::npos)
                {
                    files.push_back(dirent->d_name);
                }
                dirent = readdir(dptr);
            }
            closedir(dptr);
        }
        return files;
    }

    static u8 lateral_invert(u8 byte_in)
    {
        return ((byte_in & D0) << 7) | ((byte_in & D1) << 5) | ((byte_in & D2) << 3) |
            ((byte_in & D3) << 1) | ((byte_in & D4) >> 1) | ((byte_in & D5) >> 3) |
            ((byte_in & D6) >> 5) | ((byte_in & D7) >> 7);
    }

    static u8* generate_sliver(u8 p1, u8 p0)
    {
        static u8 sliver[8];
        for(u8 i=0; i<8; i++)
        {
            sliver[i] = (p1 & D7) >> 6 | (p0 & D7) >> 7; 
            p1 <<= 1;
            p0 <<= 1;
        }
        return sliver;
    }
};

#endif
