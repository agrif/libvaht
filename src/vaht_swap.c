#include "vaht_intern.h"

#define UNKNOWN_ENDIAN 0
#define BIG_ENDIAN 1
#define LITTLE_ENDIAN 2

static int vaht_swap_endian = UNKNOWN_ENDIAN;

void vaht_get_endian()
{
	int16_t word = 0x0001;
	char* byte = (char*)(&word);
	vaht_swap_endian = byte[0] ? LITTLE_ENDIAN : BIG_ENDIAN;
}

uint16_t vaht_swap_uint16(uint16_t in)
{
	return (vaht_swap_endian == LITTLE_ENDIAN) ? ((in >> 8) | (in << 8)) : in;
}

uint32_t vaht_swap_uint32(uint32_t in)
{
	return (vaht_swap_endian == LITTLE_ENDIAN) ? (((in & 0x000000FF) << 24) + ((in & 0x0000FF00) << 8) + ((in & 0x00FF0000) >> 8) + ((in & 0xFF000000) >> 24)) : in;
}

