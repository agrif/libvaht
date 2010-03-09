#ifndef __INCLUDE_VAHT_H__
#define __INCLUDE_VAHT_H__

#include <stdint.h>

struct vaht_archive_s;
typedef struct vaht_archive_s vaht_archive;

vaht_archive* vaht_archive_open(const char* filename);
void vaht_archive_close(vaht_archive* archive);
uint16_t vaht_archive_get_resource_types(vaht_archive* archive);
char* vaht_archive_get_resource_type(vaht_archive* archive, uint16_t i);

struct vaht_resource_s;
typedef struct vaht_resource_s vaht_resource;

vaht_resource* vaht_resource_open(vaht_archive* archive, const char* type, uint16_t id);
void vaht_resource_close(vaht_resource* resource);
vaht_resource** vaht_resources_open(vaht_archive* archive, const char* type);
void vaht_resources_close(vaht_resource** resources);
uint32_t vaht_resource_size(vaht_resource* resource);
uint32_t vaht_resource_read(vaht_resource* resource, uint32_t size, void* buffer);
char* vaht_resource_name(vaht_resource* resource);
char* vaht_resource_type(vaht_resource* resource);
uint16_t vaht_resource_id(vaht_resource* resource);
void vaht_resource_seek(vaht_resource* resource, uint32_t seek);
uint32_t vaht_resource_tell(vaht_resource* resource);

struct vaht_bmp_s;
typedef struct vaht_bmp_s vaht_bmp;

vaht_bmp* vaht_bmp_open(vaht_resource* resource);
void vaht_bmp_close(vaht_bmp* bmp);
uint16_t vaht_bmp_width(vaht_bmp* bmp);
uint16_t vaht_bmp_height(vaht_bmp* bmp);
uint8_t* vaht_bmp_data(vaht_bmp* bmp);
uint8_t vaht_bmp_compressed(vaht_bmp* bmp);
uint8_t vaht_bmp_truecolor(vaht_bmp* bmp);

struct vaht_mov_s;
typedef struct vaht_mov_s vaht_mov;

vaht_mov* vaht_mov_open(vaht_resource* resource);
void vaht_mov_close(vaht_mov* mov);
uint32_t vaht_mov_read(vaht_mov* mov, uint32_t size, void* buffer);
void vaht_mov_seek(vaht_mov* mov, uint32_t seek);
uint32_t vaht_mov_tell(vaht_mov* mov);

#endif // __INCLUDE_VAHT_H__

