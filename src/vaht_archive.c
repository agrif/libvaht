#include "vaht_intern.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

vaht_archive* vaht_archive_open(const char* filename)
{
	unsigned int i, j;
	
	FILE* fd = fopen(filename, "rb");
	if (fd == NULL)
		return NULL;
	
	vaht_archive* ret = malloc(sizeof(vaht_archive));
	ret->type_table = NULL;
	ret->resource_types = NULL;
	ret->fd = fd;
	
	ret->filename = malloc(strlen(filename) + 1);
	strcpy(ret->filename, filename);
	
	vaht_get_endian();
	
	fread(&(ret->iff), 1, sizeof(struct vaht_mohawk_header_iff), ret->fd);
	if (ret->iff.signature[0] != 'M' ||
	    ret->iff.signature[1] != 'H' ||
	    ret->iff.signature[2] != 'W' ||
	    ret->iff.signature[3] != 'K')
	{
		vaht_archive_close(ret);
		return NULL;
	}
	VAHT_SWAP_U32(ret->iff.file_size);
	
	fread(&(ret->rsrc), 1, sizeof(struct vaht_mohawk_header_rsrc), ret->fd);
	if (ret->rsrc.signature[0] != 'R' ||
	    ret->rsrc.signature[1] != 'S' ||
	    ret->rsrc.signature[2] != 'R' ||
	    ret->rsrc.signature[3] != 'C')
	{
		vaht_archive_close(ret);
		return NULL;
	}
	VAHT_SWAP_U32(ret->rsrc.chunk_len);
	VAHT_SWAP_U32(ret->rsrc.file_size);
	VAHT_SWAP_U32(ret->rsrc.resource_dir_offset);
	VAHT_SWAP_U16(ret->rsrc.file_table_offset);
	VAHT_SWAP_U16(ret->rsrc.file_table_size);
	
	if (ret->rsrc.file_size - 8 != ret->iff.file_size)
	{
		vaht_archive_close(ret);
		return NULL;
	}
	
	fseek(ret->fd, ret->rsrc.resource_dir_offset + ret->rsrc.file_table_offset, SEEK_SET);
	fread(&(ret->file_table_entries), 1, sizeof(uint32_t), ret->fd);
	VAHT_SWAP_U32(ret->file_table_entries);
	
	if (ret->file_table_entries != (ret->rsrc.file_table_size - 4) / sizeof(struct vaht_mohawk_file_table))
	{
		vaht_archive_close(ret);
		return NULL;
	}
	
	fseek(ret->fd, ret->rsrc.resource_dir_offset, SEEK_SET);
	fread(&(ret->type_table_header), 1, sizeof(struct vaht_mohawk_header_type_table), ret->fd);
	VAHT_SWAP_U16(ret->type_table_header.name_list_offset);
	VAHT_SWAP_U16(ret->type_table_header.num_types);
	
	ret->type_table = malloc(sizeof(struct vaht_mohawk_type_table) * ret->type_table_header.num_types);
	fread(ret->type_table, 1, sizeof(struct vaht_mohawk_type_table) * ret->type_table_header.num_types, ret->fd);
	ret->resource_types = malloc(sizeof(char*) * ret->type_table_header.num_types);
	for (i = 0; i < ret->type_table_header.num_types; ++i)
	{
		VAHT_SWAP_U16(ret->type_table[i].resource_table_offset);
		VAHT_SWAP_U16(ret->type_table[i].name_table_offset);
		ret->resource_types[i] = malloc(sizeof(char) * 5);
		for (j = 0; j < 4; ++j)
			ret->resource_types[i][j] = ret->type_table[i].resource_type[j];
		ret->resource_types[i][4] = 0;
	}

	ret->refcount = 1;
		
	return ret;
}

uint16_t vaht_archive_close(vaht_archive* archive)
{
	archive->refcount--;
	printf("archive: %i\n", archive->refcount);
	if (archive->refcount > 0)
		return archive->refcount;
	
	unsigned int i;
	if (archive->resource_types)
	{
		for (i = 0; i < archive->type_table_header.num_types; ++i)
		{
			free(archive->resource_types[i]);
		}
		free(archive->resource_types);
	}
	if (archive->type_table)
		free(archive->type_table);
	free(archive->filename);
	fclose(archive->fd);
	free(archive);

	return 0;
}

uint16_t vaht_archive_grab(vaht_archive* archive)
{
	archive->refcount++;
	printf("archive: %i\n", archive->refcount);
	return archive->refcount;
}

uint16_t vaht_archive_get_resource_types(vaht_archive* archive)
{
	return archive->type_table_header.num_types;
}

char* vaht_archive_get_resource_type(vaht_archive* archive, uint16_t i)
{
	return archive->resource_types[i];
}

