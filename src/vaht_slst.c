#include "vaht_intern.h"
#include <stdlib.h>
#include <string.h>

vaht_slst* vaht_slst_open(vaht_resource* resource)
{
	if (strcmp(vaht_resource_type(resource), "SLST") != 0)
		return NULL;
	
	vaht_slst* ret = malloc(sizeof(vaht_slst));
    
    vaht_resource_seek(resource, 0);
    
    /* record count comes first */
    vaht_resource_read(resource, sizeof(uint16_t), &(ret->count));
    VAHT_SWAP_U16(ret->count);
    
    ret->records = malloc(sizeof(struct vaht_slst_record) * ret->count);

    uint16_t i, j;
    for (i = 0; i < ret->count; i++)
    {
        struct vaht_slst_record* r = &(ret->records[i]);
        
        vaht_resource_read(resource, sizeof(uint16_t), &(r->index));
        VAHT_SWAP_U16(r->index);
        vaht_resource_read(resource, sizeof(uint16_t), &(r->sound_count));
        VAHT_SWAP_U16(r->sound_count);
        
        r->sound_ids = malloc(sizeof(uint16_t) * r->sound_count);
        r->volumes = malloc(sizeof(uint16_t) * r->sound_count);
        r->balances = malloc(sizeof(int16_t) * r->sound_count);
        r->u2 = malloc(sizeof(uint16_t) * r->sound_count);
        
        for (j = 0; j < r->sound_count; j++)
        {
            vaht_resource_read(resource, sizeof(uint16_t), &(r->sound_ids[j]));
            VAHT_SWAP_U16(r->sound_ids[j]);
        }
        vaht_resource_read(resource, sizeof(uint16_t), &(r->fade_flags));
        VAHT_SWAP_U16(r->fade_flags);
        vaht_resource_read(resource, sizeof(uint16_t), &(r->loop));
        VAHT_SWAP_U16(r->loop);
        vaht_resource_read(resource, sizeof(uint16_t), &(r->global_volume));
        VAHT_SWAP_U16(r->global_volume);
        vaht_resource_read(resource, sizeof(uint16_t), &(r->u0));
        VAHT_SWAP_U16(r->u0);
        vaht_resource_read(resource, sizeof(uint16_t), &(r->u1));
        VAHT_SWAP_U16(r->u1);
        for (j = 0; j < r->sound_count; j++)
        {
            vaht_resource_read(resource, sizeof(uint16_t), &(r->volumes[j]));
            VAHT_SWAP_U16(r->volumes[j]);
        }
        for (j = 0; j < r->sound_count; j++)
        {
            vaht_resource_read(resource, sizeof(int16_t), &(r->balances[j]));
            VAHT_SWAP_S16(r->balances[j]);
        }
        for (j = 0; j < r->sound_count; j++)
        {
            vaht_resource_read(resource, sizeof(uint16_t), &(r->u2[j]));
            VAHT_SWAP_U16(r->u2[j]);
        }
    }

	ret->res = resource;
	vaht_resource_grab(ret->res);
	
	return ret;
}

void vaht_slst_close(vaht_slst* slst)
{
    uint16_t i;
	vaht_resource_close(slst->res);
    for (i = 0; i < slst->count; i++)
    {
        free(slst->records[i].sound_ids);
        free(slst->records[i].volumes);
        free(slst->records[i].balances);
        free(slst->records[i].u2);
    }
    free(slst->records);
	free(slst);
}

uint16_t vaht_slst_records(vaht_slst* slst)
{
    return slst->count;
}

/* helper to reverse-map an index into our loaded records */
static inline int32_t rmap_index(vaht_slst* slst, uint16_t index)
{
	uint16_t i;
	for (i = 0; i < slst->count; i++)
	{
		if (slst->records[i].index == index)
		{
			return i;
		}
	}
	
	return -1;
}

uint16_t vaht_slst_count(vaht_slst* slst, uint16_t i)
{
    int32_t index = rmap_index(slst, i);
    if (index < 0)
        return 0;
    return slst->records[index].sound_count;
}

uint16_t vaht_slst_sound_id(vaht_slst* slst, uint16_t i, uint16_t j)
{
    int32_t index = rmap_index(slst, i);
    if (index < 0)
        return 0;
    if (j >= slst->records[index].sound_count)
        return 0;
    return slst->records[index].sound_ids[j];
}

enum vaht_slst_fade vaht_slst_fade(vaht_slst* slst, uint16_t i)
{
    int32_t index = rmap_index(slst, i);
    if (index < 0)
        return 0;
    return slst->records[index].fade_flags & 0x3;
}

uint16_t vaht_slst_loop(vaht_slst* slst, uint16_t i)
{
    int32_t index = rmap_index(slst, i);
    if (index < 0)
        return 0;
    return slst->records[index].loop;
}

uint16_t vaht_slst_global_volume(vaht_slst* slst, uint16_t i)
{
    int32_t index = rmap_index(slst, i);
    if (index < 0)
        return 0;
    return slst->records[index].global_volume;
}

uint16_t vaht_slst_volume(vaht_slst* slst, uint16_t i, uint16_t j)
{
    int32_t index = rmap_index(slst, i);
    if (index < 0)
        return 0;
    if (j >= slst->records[index].sound_count)
        return 0;
    return slst->records[index].volumes[j];
}

int16_t vaht_slst_balance(vaht_slst* slst, uint16_t i, uint16_t j)
{
    int32_t index = rmap_index(slst, i);
    if (index < 0)
        return 0;
    if (j >= slst->records[index].sound_count)
        return 0;
    return slst->records[index].balances[j];
}
