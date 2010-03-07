#include "vaht_intern.h"
#include <stdio.h>
#include <string.h>

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
		
		printf("stco chunk: %i entries\n", entries);
		
		// size:4 type:4 version:1 flags:3 #entries:4;
		mov->stco_start = start + 16;
		mov->stco_length = entries * 4;
		mov->stco_data = malloc(mov->stco_length);
		
		uint32_t file_offset = mov->res->file_table_entry.data_offset;
		printf("file offset: %i\n", file_offset);
		
		unsigned int i;
		for (i = 0; i < entries; i++)
		{
			uint32_t entry;
			if (vaht_resource_read(mov->res, 4, &entry) != 4)
				return 0;
			
			VAHT_SWAP_U32(entry);
			printf(" - old: %i\n", entry);
			entry -= file_offset;
			printf(" - new: %i\n", entry);
			VAHT_SWAP_U32(entry);
			
			memcpy(&(mov->stco_data[4 * i]), &entry, 4);
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
	ret->stco_start = 0;
	ret->stco_length = 0;
	ret->stco_data = NULL;
	
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
	if (mov->stco_data)
		free(mov->stco_data);
	free(mov);
}
