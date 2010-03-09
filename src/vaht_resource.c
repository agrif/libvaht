#include "vaht_intern.h"
#include <string.h>

vaht_resource* vaht_resource_open(vaht_archive* archive, const char* type, uint16_t id)
{
	uint16_t i, ti, ro = 0, no = 0;
	for (i = 0; i < archive->type_table_header.num_types; ++i)
	{
		if (strcmp(type, archive->resource_types[i]) == 0)
		{
			ti = i;
			ro = archive->type_table[i].resource_table_offset;
			no = archive->type_table[i].name_table_offset;
			break;
		}
	}
	if (ro == 0)
		return NULL;
	
	uint16_t res_num = 0;
	fseek(archive->fd, archive->rsrc.resource_dir_offset + ro, SEEK_SET);
	fread(&res_num, 1, sizeof(uint16_t), archive->fd);
	VAHT_SWAP_U16(res_num);
	
	struct vaht_mohawk_resource_table rest;
	int32_t file_table_index = -1;
	for (i = 0; i < res_num; ++i)
	{
		fread(&rest, 1, sizeof(struct vaht_mohawk_resource_table), archive->fd);
		VAHT_SWAP_U16(rest.resource_id);
		if (rest.resource_id == id)
		{
			VAHT_SWAP_U16(rest.file_table_index);
			file_table_index = rest.file_table_index;
			break;
		}
	}
	if (file_table_index == -1)
		return NULL;
	if (file_table_index > archive->file_table_entries)
		return NULL;
	
	vaht_resource* ret = malloc(sizeof(vaht_resource));
	ret->id = id;
	ret->typei = ti;
	ret->file_table_index = file_table_index;
	
	struct vaht_mohawk_file_table file_table_entry;
	fseek(archive->fd, archive->rsrc.resource_dir_offset + archive->rsrc.file_table_offset + sizeof(uint32_t), SEEK_SET);
	for (i = 0; i < file_table_index; ++i)
	{
		fread(&file_table_entry, 1, sizeof(struct vaht_mohawk_file_table), archive->fd);
	}
	VAHT_SWAP_U32(file_table_entry.data_offset);
	VAHT_SWAP_U16(file_table_entry.data_size_1);
	VAHT_SWAP_U16(file_table_entry.u1);
	ret->file_table_entry = file_table_entry;
	
	fseek(archive->fd, archive->rsrc.resource_dir_offset + archive->rsrc.file_table_offset + sizeof(uint32_t), SEEK_SET);
	uint32_t next_offset = archive->rsrc.file_size;
	for (i = 0; i < archive->file_table_entries; ++i)
	{
		fread(&file_table_entry, 1, sizeof(struct vaht_mohawk_file_table), archive->fd);
		VAHT_SWAP_U32(file_table_entry.data_offset);
		if (file_table_entry.data_offset > ret->file_table_entry.data_offset && file_table_entry.data_offset < next_offset)
			next_offset = file_table_entry.data_offset;
	}
	ret->size = next_offset - ret->file_table_entry.data_offset;
	
	ret->seek = 0;
	ret->archive = archive;
	
	fseek(archive->fd, archive->rsrc.resource_dir_offset + no, SEEK_SET);
	fread(&res_num, 1, sizeof(uint16_t), archive->fd);
	VAHT_SWAP_U16(res_num);
	
	struct vaht_mohawk_name_table name_table_entry;
	int32_t nlo = -1;
	for (i = 0; i < res_num; ++i)
	{
		fread(&name_table_entry, 1, sizeof(struct vaht_mohawk_name_table), archive->fd);
		VAHT_SWAP_U16(name_table_entry.file_table_index);
		if (ret->file_table_index == name_table_entry.file_table_index)
		{
			VAHT_SWAP_U16(name_table_entry.name_list_offset);
			nlo = name_table_entry.name_list_offset;
			break;
		}
	}
	
	if (nlo == -1)
	{
		ret->name = malloc(1);
		ret->name[0] = 0;
	} else {
		fseek(archive->fd, archive->rsrc.resource_dir_offset + archive->type_table_header.name_list_offset + nlo, SEEK_SET);
		char a;
		unsigned int len = 0;
		while (fread(&a, 1, sizeof(char), archive->fd))
		{
			++len;
			if (a == 0)
			{
				break;
			}
		}
		
		fseek(archive->fd, archive->rsrc.resource_dir_offset + archive->type_table_header.name_list_offset + nlo, SEEK_SET);
		ret->name = malloc(len);
		fread(ret->name, 1, len, archive->fd);
	}
	
	vaht_archive_grab(archive);
	ret->refcount = 1;
	
	return ret;
}

uint16_t vaht_resource_close(vaht_resource* resource)
{
	resource->refcount--;
	if (resource->refcount > 0)
		return resource->refcount;
	
	vaht_archive_close(resource->archive);
	free(resource->name);
	free(resource);
	return 0;
}

uint16_t vaht_resource_grab(vaht_resource* resource)
{
	resource->refcount++;
	return resource->refcount;
}

vaht_resource** vaht_resources_open(vaht_archive* archive, const char* type)
{
	uint16_t i, ti, ro = 0, no = 0;
	for (i = 0; i < archive->type_table_header.num_types; ++i)
	{
		if (strcmp(type, archive->resource_types[i]) == 0)
		{
			ti = i;
			ro = archive->type_table[i].resource_table_offset;
			no = archive->type_table[i].name_table_offset;
			break;
		}
	}
	if (ro == 0)
		return NULL;
	
	uint16_t res_num = 0;
	fseek(archive->fd, archive->rsrc.resource_dir_offset + ro, SEEK_SET);
	fread(&res_num, 1, sizeof(uint16_t), archive->fd);
	VAHT_SWAP_U16(res_num);
	
	struct vaht_mohawk_resource_table rest;
	vaht_resource** ret = malloc((sizeof(vaht_resource*) * res_num) + 1);
	for (i = 0; i < res_num; ++i)
	{
		fread(&rest, 1, sizeof(struct vaht_mohawk_resource_table), archive->fd);
		VAHT_SWAP_U16(rest.resource_id);
		size_t savepos = ftell(archive->fd);
		ret[i] = vaht_resource_open(archive, type, rest.resource_id);
		fseek(archive->fd, savepos, SEEK_SET);
	}
	ret[res_num] = NULL;
	
	return ret;
}

void vaht_resources_close(vaht_resource** resources)
{
	unsigned int i = 0;
	while (1)
	{
		if (resources[i] == NULL)
			break;
		vaht_resource_close(resources[i]);
		++i;
	}
	free(resources);
}

uint32_t vaht_resource_size(vaht_resource* resource)
{
	return resource->size;
}

uint32_t vaht_resource_read(vaht_resource* resource, uint32_t size, void* buffer)
{
	if (resource->seek + size > resource->size)
		size = resource->size - resource->seek;
	fseek(resource->archive->fd, resource->file_table_entry.data_offset + resource->seek, SEEK_SET);
	size_t read = size;
	if (buffer != NULL)
		read = fread(buffer, 1, size, resource->archive->fd);
	resource->seek += read;
	return read;
}

char* vaht_resource_name(vaht_resource* resource)
{
	return resource->name;
}

char* vaht_resource_type(vaht_resource* resource)
{
	return resource->archive->resource_types[resource->typei];
}

uint16_t vaht_resource_id(vaht_resource* resource)
{
	return resource->id;
}

void vaht_resource_seek(vaht_resource* resource, uint32_t seek)
{
	if (seek > resource->size)
		return;
	resource->seek = seek;
}

uint32_t vaht_resource_tell(vaht_resource* resource)
{
	return resource->seek;
}


