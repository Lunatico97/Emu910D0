#ifndef _TYPES_H_
#define _TYPES_H_

#define D7 0b10000000
#define D6 0b01000000
#define D5 0b00100000
#define D4 0b00010000
#define D3 0b00001000
#define D2 0b00000100
#define D1 0b00000010
#define D0 0b00000001

#define SCRW 640
#define SCRH 480
#define PPFW 640
#define PPFH 480

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

struct OPCODE
{
    u8 value;
    u8 bytes;
};

struct HEX
{
    u8 h8[3];
};

constexpr u8 IL_MAP[256] = {
       1,    2,    1,    1,    1,    2,    2,    1,    1,    2,    1,    1,    1,    3,    3,    1,
       2,    2,    1,    1,    1,    2,    2,    1,    1,    3,    1,    1,    1,    3,    3,    1,
       3,    2,    1,    1,    2,    2,    2,    1,    1,    2,    1,    1,    3,    3,    3,    1,
       2,    2,    1,    1,    1,    2,    2,    1,    1,    3,    1,    1,    1,    3,    3,    1,
       1,    2,    1,    1,    1,    2,    2,    1,    1,    2,    1,    1,    3,    3,    3,    1,
       2,    2,    1,    1,    1,    2,    2,    1,    1,    3,    1,    1,    1,    3,    3,    1,
       1,    2,    1,    1,    1,    2,    2,    1,    1,    2,    1,    1,    3,    3,    3,    1,
       2,    2,    1,    1,    1,    2,    2,    1,    1,    3,    1,    1,    1,    3,    3,    1,
       1,    2,    1,    1,    2,    2,    2,    1,    1,    1,    1,    1,    3,    3,    3,    1,
       2,    2,    1,    1,    2,    2,    2,    1,    1,    3,    1,    1,    1,    3,    1,    1,
       2,    2,    2,    1,    2,    2,    2,    1,    1,    2,    1,    1,    3,    3,    3,    1,
       2,    2,    1,    1,    2,    2,    2,    1,    1,    3,    1,    1,    3,    3,    3,    1,
       2,    2,    1,    1,    2,    2,    2,    1,    1,    2,    1,    1,    3,    3,    3,    1,
       2,    2,    1,    1,    1,    2,    2,    1,    1,    3,    1,    1,    1,    3,    3,    1,
       2,    2,    1,    1,    2,    2,    2,    1,    1,    2,    1,    1,    3,    3,    3,    1,
       2,    2,    1,    1,    1,    2,    2,    1,    1,    3,    1,    1,    1,    3,    3,    1
};

constexpr u8 CYCLE_MAP[256] = {
    7, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 4, 4, 6, 6,
    2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 5, 5, 7, 7,
    6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 4, 4, 6, 6,
    2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 5, 5, 7, 7,
    6, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 4, 4, 6, 6,
    2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 5, 5, 7, 7,
    6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 4, 4, 6, 6,
    2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 5, 5, 7, 7,
    2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
    2, 6, 2, 6, 4, 4, 4, 4, 2, 5, 2, 5, 5, 5, 5, 5,
    2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
    2, 5, 2, 5, 4, 4, 4, 4, 2, 4, 2, 4, 5, 5, 5, 5,
    2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
    2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 5, 5, 7, 7,
    2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
    2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 5, 5, 7, 7
};

// RGB Approximation for NES Master Pallette
constexpr u32 RGB_PAL[] = {
    // Index 0x00-0x0F
    0x666666FF, 0x002A88FF, 0x1412A7FF, 0x3B00A4FF, 
    0x5F0051FF, 0x6E000CFF, 0x6D0800FF, 0x5D1B00FF, 
    0x3F3000FF, 0x0E4400FF, 0x004E00FF, 0x004824FF, 
    0x003A5CFF, 0x000000FF, 0x000000FF, 0x000000FF, 
    // Index 0x10-0x1F
    0xADADADFF, 0x155FD9FF, 0x3C40FFFF, 0x6A2CFFFF, 
    0x9726B3FF, 0xB1205FFF, 0xB03000FF, 0x9F4800FF, 
    0x7C6000FF, 0x457A00FF, 0x138300FF, 0x007C40FF, 
    0x006B8DFF, 0x000000FF, 0x000000FF, 0x000000FF, 
    // Index 0x20-0x2F
    0xFFFFFFFF, 0x4CABFFFF, 0x7D98FFFF, 0xAC88FFFF, 
    0xD77BFFFF, 0xF173CEFF, 0xF08360FF, 0xF09B2FFF, 
    0xD8B020FF, 0xA0CB00FF, 0x6ED600FF, 0x4CD16CFF, 
    0x4CC4A0FF, 0x000000FF, 0x000000FF, 0x000000FF, 
    // Index 0x30-0x3F
    0xFFFFFFFF, 0x96CDFFFF, 0xBCC4FFFF, 0xD9B8FFFF, 
    0xF2B0FFFF, 0xFFA4CCFF, 0xFFB8A8FF, 0xFFD088FF, 
    0xFCE0A8FF, 0xC8EFA8FF, 0x9CF2B0FF, 0x88F0D0FF, 
    0x96E8E8FF, 0x000000FF, 0x000000FF, 0x000000FF
};

// APU Length Counter Lookup
constexpr u16 APU_LC[] = {
    // Index 0x00 - 0x0F
    0x0A, 0xFE, 0x14, 0x02, 0x28, 0x04, 0x50, 0x06, 0xA0, 0x08, 0x3C, 0x0A, 0x0E, 0x0C, 0x1A, 0x0E,
    // Index 0x10 - 0x1F
    0x0C, 0x10, 0x18, 0x12, 0x30, 0x14, 0x60, 0x16, 0xC0, 0x18, 0x48, 0x1A, 0x10, 0x1C, 0x20, 0x1E
};

// APU Noise Period Lookup
constexpr u16 APU_TM[] = {
    // Index 0x00 - 0x0F
    0x0004, 0x0008, 0x0010, 0x0020, 0x0030, 0x0060, 0x0080, 0x00A0, 0x00CA, 0x00FE, 0x017C, 0x01FC, 0x02FA, 0x03F8, 0x07F2, 0x0FE4
};

#endif