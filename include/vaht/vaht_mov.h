#ifndef __INCLUDE_VAHT_MOV_H__
#define __INCLUDE_VAHT_MOV_H__

#include <stdint.h>
#include "vaht_resource.h"

struct vaht_mov_s;
typedef struct vaht_mov_s vaht_mov;

vaht_mov* vaht_mov_open(vaht_resource* resource);
void vaht_mov_close(vaht_mov* mov);
uint32_t vaht_mov_read(vaht_mov* mov, uint32_t size, void* buffer);
void vaht_mov_seek(vaht_mov* mov, uint32_t seek);
uint32_t vaht_mov_tell(vaht_mov* mov);

#endif // __INCLUDE_VAHT_MOV_H__
