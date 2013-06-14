#include "vaht/vaht.h"
#include <stdio.h>

#ifndef __INCLUDE_VAHT_INTERN_H__
#define __INCLUDE_VAHT_INTERN_H__

void vaht_get_endian();
uint16_t vaht_swap_uint16(uint16_t in);
uint32_t vaht_swap_uint32(uint32_t in);

#define VAHT_SWAP_U16(v) (v) = vaht_swap_uint16(v)
#define VAHT_SWAP_U32(v) (v) = vaht_swap_uint32(v)
#define VAHT_SWAP_S16(v) (v) = (int16_t)vaht_swap_uint16((uint16_t)(v))

struct vaht_mohawk_header_iff
{
	char signature[4]; /* Should be "MHWK" */
	uint32_t file_size; /* File size (- this header), or fsize - 8 */
};

struct vaht_mohawk_header_rsrc
{
	char signature[4]; /* Should be "RSRC" */
	uint32_t chunk_len; /* Should be length of header (?) (not really) */
	uint32_t file_size; /* Total file size in bytes */
	uint32_t resource_dir_offset; /* absolute offset of rsrc dir */
	uint16_t file_table_offset; /* offset in rsrc dir of file table */
	uint16_t file_table_size; /* size of the file table */
};

struct vaht_mohawk_header_type_table
{
	uint16_t name_list_offset; /* offset in rsrc dir of name list */
	uint16_t num_types; /* number of resource types in file */
};

struct vaht_mohawk_type_table
{
	char resource_type[4]; /* 4 char resource type (tWAV, NAME, etc) */
	uint16_t resource_table_offset; /* in resource dir */
	uint16_t name_table_offset; /* in resource dir */
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
	uint16_t refcount;
};

struct vaht_mohawk_resource_table
{
	uint16_t resource_id; /* res ID */
	uint16_t file_table_index; /* starting from 1 */
};

struct vaht_mohawk_name_table
{
	uint16_t name_list_offset; /* res ID */
	uint16_t file_table_index; /* starting from 1 */
};

struct vaht_mohawk_file_table
{
	uint32_t data_offset; /* absolute */
	uint16_t data_size_1; /* bits 15-0 */
	uint8_t data_size_2; /* bits 23-16 */
	uint8_t u0;
	uint16_t u1;
};

struct vaht_resource_s
{
	uint16_t id;
	uint8_t typei;
	vaht_archive* archive;
	uint16_t file_table_index; /* starting from 1 */
	struct vaht_mohawk_file_table file_table_entry;
	uint32_t size;
	uint32_t seek;
	char* name;
	uint16_t refcount;
};

struct vaht_bmp_header
{
	uint16_t width;
	uint16_t height;
	uint16_t bytes_per_row;
	uint8_t compression; /* 0 - un, 4 - compressed */
	uint8_t truecolor; /* 4 - truecolor, not 4 - indexed */
};

struct vaht_bmp_s
{
	uint16_t width;
	uint16_t height;
	uint16_t bytes_per_row;
	uint8_t* data;
	uint8_t format; /* bit 0 - compressed, 1 - truecolor */
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

struct vaht_wav_adpcm_decode_state
{
	int32_t index;
	int32_t predictor;
	uint16_t step;
};

struct vaht_wav_s
{
	vaht_resource* res;
	uint16_t sample_rate;
	uint32_t sample_count;
	enum vaht_wav_encoding_t encoding;
	uint8_t bits_per_sample;
	uint8_t channels;
	uint32_t data_seek;
	
	struct vaht_wav_adpcm_decode_state* adpcm_state[2];
};

struct vaht_name_s
{
	vaht_resource* res;
	uint16_t count;
};

struct vaht_card_s
{
	vaht_resource* res;
	char* cached_name;
	int16_t name_rec; /* index into NAME 1 if >= 0 */

    /* if non-0, this is a zip mode destination */
	uint16_t zip_mode_place;
	
	vaht_script* script;
};

struct vaht_plst_record
{
	uint16_t index;
	uint16_t bitmap_id;
	uint16_t left;
	uint16_t top;
	uint16_t right;
	uint16_t bottom;
};

struct vaht_plst_s
{
	vaht_resource* res;
	uint16_t count;
	struct vaht_plst_record* records;
};

struct vaht_command_s
{
	uint8_t is_branch;
	union
	{
		struct
		{
			uint16_t cmd;
			uint16_t arg_count;
			uint16_t* args;
		} cmd;
		
		struct
		{
			uint16_t var;
			uint16_t value_count;
			uint16_t* values;
			vaht_command*** commands;
		} branch;
	};
};

struct vaht_script_s
{
	vaht_command** handlers[EVENT_COUNT];
};

struct vaht_blst_record
{
	uint16_t index;
	uint16_t enabled;
	uint16_t hotspot_id;
};

struct vaht_blst_s
{
	vaht_resource* res;
	uint16_t count;
	struct vaht_blst_record* records;
};

struct vaht_hspt_record
{
	uint16_t blst_id;
	int16_t name_rec;
	int16_t left;
	int16_t top;
	int16_t right;
	int16_t bottom;
	uint16_t u0;
	uint16_t mouse_cursor;
	uint16_t index;
	int16_t u1;
	uint16_t zip_mode;
};

struct vaht_hspt_s
{
	vaht_resource* res;
	uint16_t count;
	struct vaht_hspt_record* records;
	vaht_script** scripts;
	char** names;
};

#endif /* __INCLUDE_VAHT_INTERN_H__ */

