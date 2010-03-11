#ifndef __INCLUDE_VAHT_RESOURCE_H__
#define __INCLUDE_VAHT_RESOURCE_H__

#include <stdint.h>
#include "vaht_archive.h"

struct vaht_resource_s;
typedef struct vaht_resource_s vaht_resource;

vaht_resource* vaht_resource_open(vaht_archive* archive, const char* type, uint16_t id);
uint16_t vaht_resource_close(vaht_resource* resource);
uint16_t vaht_resource_grab(vaht_resource* resource);
vaht_resource** vaht_resources_open(vaht_archive* archive, const char* type);
void vaht_resources_close(vaht_resource** resources);
uint32_t vaht_resource_size(vaht_resource* resource);
uint32_t vaht_resource_read(vaht_resource* resource, uint32_t size, void* buffer);
char* vaht_resource_name(vaht_resource* resource);
char* vaht_resource_type(vaht_resource* resource);
uint16_t vaht_resource_id(vaht_resource* resource);
void vaht_resource_seek(vaht_resource* resource, uint32_t seek);
uint32_t vaht_resource_tell(vaht_resource* resource);

#endif // __INCLUDE_VAHT_RESOURCE_H__
