#ifndef TYPES_H
#define TYPES_H

#include <cstdint>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef float f32;
typedef double f64;

inline s32
random_int(s32 min, s32 max)
{
	s32 range = max - min;
	return min + rand() % range;
}

inline s32
min(s32 x, s32 y)
{
	return x < y ? x : y;
}
inline s32
max(s32 x, s32 y)
{
	return x > y ? x : y;
}

#endif // TYPES_H
