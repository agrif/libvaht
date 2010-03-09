#include "vaht_intern.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN(_a, _b) ((_a) > (_b) ? (_b) : (_a))

// returns how many bytes it read...
static uint32_t read_atom(vaht_mov* mov, uint32_t start)
{
	vaht_resource_seek(mov->res, start);
	
	uint32_t atom_size;
	char atom_name[5];
	if (vaht_resource_read(mov->res, 4, &atom_size) != 4)
		return 0;
	if (vaht_resource_read(mov->res, 4, atom_name) != 4)
		return 0;
	atom_name[4] = 0;
	VAHT_SWAP_U32(atom_size);
	
	//printf("[atom] name: %s size: %i\n", atom_name, atom_size);
	
	// atom heirarchy that we want to look at:
	// moov >> trak >> mdia >> minf >> stbl >> stco
	// (all stco entries are relative to MHK, we want relative to resource!)
	if (strcmp(atom_name, "moov") == 0 || strcmp(atom_name, "trak") == 0 ||
		strcmp(atom_name, "mdia") == 0 || strcmp(atom_name, "minf") == 0 ||
		strcmp(atom_name, "stbl") == 0 )
	{
		// we want subatoms!
		uint32_t cur_pos = 8;
		
		while (cur_pos < atom_size)
		{
			uint32_t add_pos = read_atom(mov, start + cur_pos);
			if (add_pos == 0)
				return 0;
			cur_pos += add_pos;
		}
	}
	
	// if this is our chunk...
	if (strcmp(atom_name, "stco") == 0)
	{
		// here's the structure...
		// 1 byte - version (0)
		// 3 bytes - flags (0)
		// 4 bytes - uint32_t: number of entries
		// followed by many uint32_t entries...
		vaht_resource_seek(mov->res, start + 8);
		
		uint8_t version;
		uint8_t flags[3];
		uint32_t entries;
		
		if (vaht_resource_read(mov->res, 1, &version) != 1)
			return 0;
		
		// sanity check
		if (version != 0)
			return 0;
		
		if (vaht_resource_read(mov->res, 3, flags) != 3)
			return 0;
		if (vaht_resource_read(mov->res, 4, &entries) != 4)
			return 0;
		
		VAHT_SWAP_U32(entries);
		
		//printf("stco chunk: %i entries\n", entries);
		
		// size:4 type:4 version:1 flags:3 #entries:4;
		mov->stco_start[mov->stco_count] = start + 16;
		mov->stco_length[mov->stco_count] = entries * 4;
		mov->stco_data[mov->stco_count] = malloc(mov->stco_length);
		mov->stco_count += 1;
		
		uint32_t file_offset = mov->res->file_table_entry.data_offset;
		//printf("file offset: %i\n", file_offset);
		
		unsigned int i;
		for (i = 0; i < entries; i++)
		{
			uint32_t entry;
			if (vaht_resource_read(mov->res, 4, &entry) != 4)
				return 0;
			
			VAHT_SWAP_U32(entry);
			//printf(" - old: %i\n", entry);
			entry -= file_offset;
			//printf(" - new: %i\n", entry);
			VAHT_SWAP_U32(entry);
			
			memcpy(&(mov->stco_data[mov->stco_count - 1][4 * i]), &entry, 4);
		}
	}
	
	return atom_size;
}

vaht_mov* vaht_mov_open(vaht_resource* resource)
{
	if (strcmp(vaht_resource_type(resource), "tMOV") != 0)
		return NULL;
	
	vaht_mov* ret = malloc(sizeof(vaht_mov));
	
	ret->res = resource;
	ret->stco_count = 0;
	
	uint32_t position = 0;
	while (position < vaht_resource_size(resource))
	{
		uint32_t add_pos = read_atom(ret, position);
		if (add_pos == 0)
		{
			vaht_mov_close(ret);
			return NULL;
		}
		position += add_pos;
	}
	
	return ret;
}

void vaht_mov_close(vaht_mov* mov)
{
	unsigned int i;
	for (i = 0; i < mov->stco_count; i++)
	{
		free(mov->stco_data[i]);
	}

	free(mov);
}

uint32_t vaht_mov_read(vaht_mov* mov, uint32_t size, void* buffer)
{
	uint32_t seek = vaht_resource_tell(mov->res);
	
	uint32_t read = vaht_resource_read(mov->res, size, buffer);
	
	unsigned int i;
	for (i = 0; i < mov->stco_count; i++)
	{
		if (mov->stco_start[i] < seek + read && seek < mov->stco_start[i] + mov->stco_length[i])
		{
			// we have overlap! no to find out how...
			// src is mov->stco_data, dest is buffer
			uint32_t src_start;
			uint32_t dest_start;
			
			if (mov->stco_start[i] >= seek)
			{
				// stco begins within buffer
				src_start = 0;
				dest_start = mov->stco_start[i] - seek;
			} else {
				// stco begins before buffer
				src_start = seek - mov->stco_start[i];
				dest_start = 0;
			}
			
			// most bytes to read from (mov->stco_length - src_start)
			// most bytes to write to (read - dest_start)
			
			uint32_t length = MIN(mov->stco_length[i] - src_start, read - dest_start);
			
			//printf("src: %i dest: %i length: %i\n", src_start, dest_start, length);
			
			if (mov->stco_data)
				memcpy(&(buffer[dest_start]), &(mov->stco_data[i][src_start]), length);
		}
	}
	
	return read;
}

void vaht_mov_seek(vaht_mov* mov, uint32_t seek)
{
	vaht_resource_seek(mov->res, seek);
}

uint32_t vaht_mov_tell(vaht_mov* mov)
{
	return vaht_resource_tell(mov->res);
}
