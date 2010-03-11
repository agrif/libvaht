#ifndef __INCLUDE_VAHT_BMP_H__
#define __INCLUDE_VAHT_BMP_H__

#include <stdint.h>
#include "vaht_resource.h"

struct vaht_bmp_s;
typedef struct vaht_bmp_s vaht_bmp;

vaht_bmp* vaht_bmp_open(vaht_resource* resource);
void vaht_bmp_close(vaht_bmp* bmp);
uint16_t vaht_bmp_width(vaht_bmp* bmp);
uint16_t vaht_bmp_height(vaht_bmp* bmp);
uint8_t* vaht_bmp_data(vaht_bmp* bmp);
uint8_t vaht_bmp_compressed(vaht_bmp* bmp);
uint8_t vaht_bmp_truecolor(vaht_bmp* bmp);

#endif // __INCLUDE_VAHT_BMP_H__
