#include "vaht_intern.h"
#include <stdlib.h>
#include <string.h>

vaht_blst* vaht_blst_open(vaht_resource* resource)
{
    if (strcmp(vaht_resource_type(resource), "BLST") != 0)
        return NULL;
    
    vaht_blst* ret = malloc(sizeof(vaht_blst));
    
    vaht_resource_seek(resource, 0);
    
    /* record count comes first */
    vaht_resource_read(resource, sizeof(uint16_t), &(ret->count));
    VAHT_SWAP_U16(ret->count);
    
    ret->records = malloc(sizeof(struct vaht_blst_record) * ret->count);
    
    uint16_t i;
    for (i = 0; i < ret->count; i++)
    {
        vaht_resource_read(resource, sizeof(struct vaht_blst_record), &(ret->records[i]));
        VAHT_SWAP_U16(ret->records[i].index);
        VAHT_SWAP_U16(ret->records[i].enabled);
        VAHT_SWAP_U16(ret->records[i].hotspot_id);
    }
    
    ret->res = resource;
    vaht_resource_grab(ret->res);
    
    return ret;
}

void vaht_blst_close(vaht_blst* blst)
{
    vaht_resource_close(blst->res);
    free(blst->records);
    free(blst);
}

uint16_t vaht_blst_records(vaht_blst* blst)
{
    return blst->count;
}

/* helper to reverse-map an index into our loaded records */
static inline int32_t rmap_index(vaht_blst* blst, uint16_t index)
{
    uint16_t i;
    for (i = 0; i < blst->count; i++)
    {
        if (blst->records[i].index == index)
        {
            return i;
        }
    }
    
    return -1;
}

uint16_t vaht_blst_enabled(vaht_blst* blst, uint16_t i)
{
    int32_t index = rmap_index(blst, i);
    if (index < 0)
        return 0;
    return blst->records[index].enabled;
}

int32_t vaht_blst_hotspot_id(vaht_blst* blst, uint16_t i)
{
    int32_t index = rmap_index(blst, i);
    if (index < 0)
        return -1;
    return blst->records[index].hotspot_id;
}
