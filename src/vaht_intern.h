#include "vaht.h"
#include <stdio.h>

#ifndef __INCLUDE_VAHT_INTERN_H__
#define __INCLUDE_VAHT_INTERN_H__

void vaht_get_endian();
uint16_t vaht_swap_uint16(uint16_t in);
uint32_t vaht_swap_uint32(uint32_t in);

#define VAHT_SWAP_U16(v) v = vaht_swap_uint16(v)
#define VAHT_SWAP_U32(v) v = vaht_swap_uint32(v)

struct vaht_mohawk_header_iff
{
	char signature[4]; // Should be "MHWK"
	uint32_t file_size; // File size (- this header), or fsize - 8
};

struct vaht_mohawk_header_rsrc
{
	char signature[4]; // Should be "RSRC"
	uint32_t chunk_len; // Should be length of header (?) (not really)
	uint32_t file_size; // Total file size in bytes
	uint32_t resource_dir_offset; // absolute offset of rsrc dir
	uint16_t file_table_offset; // offset in rsrc dir of file table
	uint16_t file_table_size; // size of the file table
};

struct vaht_mohawk_header_type_table
{
	uint16_t name_list_offset; // offset in rsrc dir of name list
	uint16_t num_types; // number of resource types in file
};

struct vaht_mohawk_type_table
{
	char resource_type[4]; // 4 char resource type (tWAV, NAME, etc)
	uint16_t resource_table_offset; // in resource dir
	uint16_t name_table_offset; // in resource dir
};

struct vaht_archive_s
{
	char* filename;
	FILE* fd;
	struct vaht_mohawk_header_iff iff;
	struct vaht_mohawk_header_rsrc rsrc;
	uint32_t file_table_entries;
	struct vaht_mohawk_header_type_table type_table_header;
	struct vaht_mohawk_type_table* type_table;
	char** resource_types;
};

struct vaht_mohawk_resource_table
{
	uint16_t resource_id; // res ID
	uint16_t file_table_index; // starting from 1
};

struct vaht_mohawk_name_table
{
	uint16_t name_list_offset; // res ID
	uint16_t file_table_index; // starting from 1
};

struct vaht_mohawk_file_table
{
	uint32_t data_offset; // absolute
	uint16_t data_size_1; // bits 15-0
	uint8_t data_size_2; // bits 23-16
	uint8_t u0;
	uint16_t u1;
};

struct vaht_resource_s
{
	uint16_t id;
	uint8_t typei;
	vaht_archive* archive;
	uint16_t file_table_index; // starting from 1
	struct vaht_mohawk_file_table file_table_entry;
	uint32_t size;
	uint32_t seek;
	char* name;
};

struct vaht_bmp_header
{
	uint16_t width;
	uint16_t height;
	uint16_t bytes_per_row;
	uint8_t compression; // 0 - un, 4 - compressed
	uint8_t truecolor; // 4 - truecolor, not 4 - indexed
};

struct vaht_bmp_s
{
	uint16_t width;
	uint16_t height;
	uint8_t* data;
	uint8_t format; // bit 0 - compressed, 1 - truecolor
	uint32_t datastart;
	vaht_resource* res;
};

struct vaht_mov_s
{
	vaht_resource* res;
	uint32_t stco_start[5];
	uint32_t stco_length[5];
	uint8_t* stco_data[5];
	uint8_t stco_count;
};

#endif // __INCLUDE_VAHT_INTERN_H__

