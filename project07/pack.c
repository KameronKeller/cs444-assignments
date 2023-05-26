#include "pack.h"

unsigned int read_u32(void *addr)
{
    unsigned char *bytes = addr;

    return (bytes[0] << 24) |
        (bytes[1] << 16) |
        (bytes[2] << 8) |
        (bytes[3] << 0);
}

unsigned short read_u16(void *addr)
{
    unsigned char *bytes = addr;

    return (bytes[0] << 8) | (bytes[1] << 0);
}

unsigned char read_u8(void *addr)
{
    unsigned char *bytes = addr;

    return bytes[0];
}

void write_u32(void *addr, unsigned long value)
{
    unsigned char *bytes = addr;

    bytes[0] = (value >> 24) & 0xff;
    bytes[1] = (value >> 16) & 0xff;
    bytes[2] = (value >> 8) & 0xff;
    bytes[3] = value & 0xff;
}

void write_u16(void *addr, unsigned int value)
{
    unsigned char *bytes = addr;

    bytes[0] = (value >> 8) & 0xff;
    bytes[1] = value & 0xff;
}

void write_u8(void *addr, unsigned char value)
{
    unsigned char *bytes = addr;

    bytes[0] = value;
}

