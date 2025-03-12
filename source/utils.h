#ifndef _UTILS_H_
#define _UTILS_H_

#include "types.h"


#define BIT_MASK(nr)    (UL(1) << ((nr) % BITS_PER_LONG))
#define BIT_WORD(nr)    ((nr) / BITS_PER_LONG)
#define BIT_ULL_MASK(nr)   (ULL(1) << ((nr) % BITS_PER_LONG_LONG))
#define BIT_ULL_WORD(nr)   ((nr) / BITS_PER_LONG_LONG)
#define BITS_PER_BYTE      8

#define _UL(v) ((u32)v)

#define __BITS_PER_LONG (32)
#define __BITS_PER_LONG_LONG (64)

#define __GENMASK(h, l) \
        (((~_UL(0)) - (_UL(1) << (l)) + 1) & \
         (~_UL(0) >> (__BITS_PER_LONG - 1 - (h))))

#define __GENMASK_ULL(h, l) \
        (((~_ULL(0)) - (_ULL(1) << (l)) + 1) & \
         (~_ULL(0) >> (__BITS_PER_LONG_LONG - 1 - (h))))

#define GENMASK(h, l) \
   (__GENMASK(h, l))
#define GENMASK_ULL(h, l) \
   (__GENMASK_ULL(h, l))

#define BIT(x) (1 << (x))

#define max(a, b) \
    ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
       _a > _b ? _a : _b; })

#define min(a, b) \
    ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
       _a < _b ? _a : _b; })

#define clamp(x, low, high) \
    ({ __typeof__(x) __x = (x); \
       __typeof__(low) __low = (low); \
       __typeof__(high) __high = (high); \
       __x > __high ? __high : (__x < __low ? __low : __x); })

u64 getle64(const u8* p);
u64 getle48(const u8* p);
u32 getle32(const u8* p);
u32 getle16(const u8* p);
u64 getbe64(const u8* p);
u64 getbe48(const u8* p);
u32 getbe32(const u8* p);
u32 getbe16(const u8* p);
void putle16(u8* p, u16 n);
void putle32(u8* p, u32 n);
void putle48(u8* p, u64 n);
void putle64(u8* p, u64 n);
void putbe16(u8* p, u16 n);
void putbe32(u8* p, u32 n);
void putbe48(u8* p, u64 n);
void putbe64(u8* p, u64 n);

void* memcpy32(void* dst, const void* src, size_t size);

void hexdump(const char* prefix, const u8* data, u32 size);
void hexdump_f(const char* prefix, const u8* data, u32 size, void (*f)(char* fmt, ...));

bool is_set(u8* buffer, size_t size);

static void writel(u32 val, intptr_t addr) {
   *(vu32*)addr = val;
}

static u32 readl(intptr_t addr) {
   return *(vu32*)addr;
}

#endif // _UTILS_H_
