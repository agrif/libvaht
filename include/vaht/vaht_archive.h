#ifndef __INCLUDE_VAHT_ARCHIVE_H__
#define __INCLUDE_VAHT_ARCHIVE_H__

#include <stdint.h>

struct vaht_archive_s;
typedef struct vaht_archive_s vaht_archive;

vaht_archive* vaht_archive_open(const char* filename);
uint16_t vaht_archive_close(vaht_archive* archive);
uint16_t vaht_archive_grab(vaht_archive* archive);
uint16_t vaht_archive_get_resource_types(vaht_archive* archive);
char* vaht_archive_get_resource_type(vaht_archive* archive, uint16_t i);

#endif // __INCLUDE_VAHT_ARCHIVE_H__
