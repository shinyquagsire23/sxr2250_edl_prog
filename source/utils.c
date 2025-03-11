#include "utils.h"
#include "types.h"

#include <string.h>

u64 getle64(const u8* p)
{
    u64 n = p[0];

    n |= (u64)p[1] << 8;
    n |= (u64)p[2] << 16;
    n |= (u64)p[3] << 24;
    n |= (u64)p[4] << 32;
    n |= (u64)p[5] << 40;
    n |= (u64)p[6] << 48;
    n |= (u64)p[7] << 56;
    return n;
}

u64 getbe64(const u8* p)
{
    u64 n = 0;

    n |= (u64)p[0] << 56;
    n |= (u64)p[1] << 48;
    n |= (u64)p[2] << 40;
    n |= (u64)p[3] << 32;
    n |= (u64)p[4] << 24;
    n |= (u64)p[5] << 16;
    n |= (u64)p[6] << 8;
    n |= (u64)p[7] << 0;
    return n;
}

u64 getbe48(const u8* p)
{
    u64 n = 0;

    n |= (u64)p[0] << 40;
    n |= (u64)p[1] << 32;
    n |= (u64)p[2] << 24;
    n |= (u64)p[3] << 16;
    n |= (u64)p[4] << 8;
    n |= (u64)p[5] << 0;
    return n;
}

u64 getle48(const u8* p)
{
    u64 n = p[0];

    n |= (u64)p[1] << 8;
    n |= (u64)p[2] << 16;
    n |= (u64)p[3] << 24;
    n |= (u64)p[4] << 32;
    n |= (u64)p[5] << 40;
    return n;
}

u32 getle32(const u8* p)
{
    return (p[0]<<0) | (p[1]<<8) | (p[2]<<16) | (p[3]<<24);
}

u32 getbe32(const u8* p)
{
    return (p[0]<<24) | (p[1]<<16) | (p[2]<<8) | (p[3]<<0);
}

u32 getle16(const u8* p)
{
    return (p[0]<<0) | (p[1]<<8);
}

u32 getbe16(const u8* p)
{
    return (p[0]<<8) | (p[1]<<0);
}

void putle16(u8* p, u16 n)
{
    p[0] = (u8) n;
    p[1] = (u8) (n>>8);
}

void putle32(u8* p, u32 n)
{
    p[0] = (u8) n;
    p[1] = (u8) (n>>8);
    p[2] = (u8) (n>>16);
    p[3] = (u8) (n>>24);
}

void putle48(u8* p, u64 n)
{
    p[0] = (u8)n;
    p[1] = (u8)(n >> 8);
    p[2] = (u8)(n >> 16);
    p[3] = (u8)(n >> 24);
    p[4] = (u8)(n >> 32);
    p[5] = (u8)(n >> 40);
}

void putle64(u8* p, u64 n)
{
    p[0] = (u8) n;
    p[1] = (u8) (n >> 8);
    p[2] = (u8) (n >> 16);
    p[3] = (u8) (n >> 24);
    p[4] = (u8) (n >> 32);
    p[5] = (u8) (n >> 40);
    p[6] = (u8) (n >> 48);
    p[7] = (u8) (n >> 56);
}

void putbe16(u8* p, u16 n)
{
    p[1] = (u8) n;
    p[0] = (u8) (n >> 8);
}

void putbe32(u8* p, u32 n)
{
    p[3] = (u8) n;
    p[2] = (u8) (n >> 8);
    p[1] = (u8) (n >> 16);
    p[0] = (u8) (n >> 24);
}

void putbe48(u8* p, u64 n)
{
    p[5] = (u8)n;
    p[4] = (u8)(n >> 8);
    p[3] = (u8)(n >> 16);
    p[2] = (u8)(n >> 24);
    p[1] = (u8)(n >> 32);
    p[0] = (u8)(n >> 40);
}

void putbe64(u8* p, u64 n)
{
    p[7] = (u8) n;
    p[6] = (u8) (n >> 8);
    p[5] = (u8) (n >> 16);
    p[4] = (u8) (n >> 24);
    p[3] = (u8) (n >> 32);
    p[2] = (u8) (n >> 40);
    p[1] = (u8) (n >> 48);
    p[0] = (u8) (n >> 56);
}

void* memcpy32(void* dst, const void* src, size_t size)
{
    u32* dst32 = dst;
    const u32* src32 = src;
    int words = size / sizeof(u32);

    for(int i = 0; i < words; i++)
        dst32[i] = src32[i];

    return dst;
}

void hexdump_f(const char* prefix, const u8* data, u32 size, int (*f)(const char* fmt, ...))
{
    u32 i;
    u32 prefixlen = strlen(prefix);
    u32 offs = 0;
    u32 line = 0;
    while(size)
    {
        u32 max = 16;

        if (max > size)
            max = size;

        if (line==0)
            f("%s", prefix);
        else
            f("%*s", prefixlen, "");


        for(i=0; i<max; i++)
            f(" %02X", data[offs+i]);
        f("\r\n");
        line++;
        size -= max;
        offs += max;
    }
}

void hexdump(const char* prefix, const u8* data, u32 size)
{
	//hexdump_f(prefix, data, size, printf);
}

bool is_set(u8* buffer, size_t size)
{
	if (!buffer || size == 0) return false;

	for (int i = 0; i < size; i++)
		if (buffer[i] != 0) return true;

	return false;
}
