#include "vaht_intern.h"
#include <stdlib.h>
#include <string.h>

vaht_plst* vaht_plst_open(vaht_resource* resource)
{
    if (strcmp(vaht_resource_type(resource), "PLST") != 0)
        return NULL;
    
    vaht_plst* ret = malloc(sizeof(vaht_plst));
    
    vaht_resource_seek(resource, 0);
    
    /* record count comes first */
    vaht_resource_read(resource, sizeof(uint16_t), &(ret->count));
    VAHT_SWAP_U16(ret->count);
    
    ret->records = malloc(sizeof(struct vaht_plst_record) * ret->count);
    
    uint16_t i;
    for (i = 0; i < ret->count; i++)
    {
        vaht_resource_read(resource, sizeof(struct vaht_plst_record), &(ret->records[i]));
        VAHT_SWAP_U16(ret->records[i].index);
        VAHT_SWAP_U16(ret->records[i].bitmap_id);
        VAHT_SWAP_U16(ret->records[i].left);
        VAHT_SWAP_U16(ret->records[i].top);
        VAHT_SWAP_U16(ret->records[i].right);
        VAHT_SWAP_U16(ret->records[i].bottom);
    }
    
    ret->res = resource;
    vaht_resource_grab(ret->res);
    
    return ret;
}

void vaht_plst_close(vaht_plst* plst)
{
    vaht_resource_close(plst->res);
    free(plst->records);
    free(plst);
}

uint16_t vaht_plst_records(vaht_plst* plst)
{
    return plst->count;
}

/* helper to reverse-map an index into our loaded records */
static inline int32_t rmap_index(vaht_plst* plst, uint16_t index)
{
    uint16_t i;
    for (i = 0; i < plst->count; i++)
    {
        if (plst->records[i].index == index)
        {
            return i;
        }
    }
    
    return -1;
}

int32_t vaht_plst_bitmap_id(vaht_plst* plst, uint16_t i)
{
    int32_t index = rmap_index(plst, i);
    if (index < 0)
        return -1;
    return plst->records[index].bitmap_id;
}

vaht_bmp* vaht_plst_bitmap_open(vaht_plst* plst, uint16_t i)
{
    uint16_t bid = vaht_plst_bitmap_id(plst, i);
    vaht_resource* bmp_res = vaht_resource_open(plst->res->archive, "tBMP", bid);
    if (!bmp_res)
        return NULL;
    
    vaht_bmp* bmp = vaht_bmp_open(bmp_res);
    vaht_resource_close(bmp_res);
    
    return bmp;
}

void vaht_plst_rect(vaht_plst* plst, uint16_t i, uint16_t* left, uint16_t* right, uint16_t* top, uint16_t* bottom)
{
    int32_t index = rmap_index(plst, i);
    if (index < 0)
        return;
    
    if (left)
        *left = plst->records[index].left;
    if (right)
        *right = plst->records[index].right;
    if (top)
        *top = plst->records[index].top;
    if (bottom)
        *bottom = plst->records[index].bottom;
}
