#include "vaht_intern.h"
#include <stdlib.h>
#include <string.h>

vaht_hspt* vaht_hspt_open(vaht_resource* resource)
{
    if (strcmp(vaht_resource_type(resource), "HSPT") != 0)
        return NULL;
    
    vaht_hspt* ret = malloc(sizeof(vaht_hspt));
    
    vaht_resource_seek(resource, 0);
    
    /* record count comes first */
    vaht_resource_read(resource, sizeof(uint16_t), &(ret->count));
    VAHT_SWAP_U16(ret->count);
    
    ret->records = malloc(ret->count * sizeof(struct vaht_hspt_record));
    ret->scripts = calloc(ret->count, sizeof(vaht_script*));
    ret->names = calloc(ret->count, sizeof(char*));

    ret->res = resource;
    vaht_resource_grab(ret->res);
    
    uint16_t i;
    for (i = 0; i < ret->count; i++)
    {
        vaht_resource_read(resource, sizeof(struct vaht_hspt_record), &(ret->records[i]));
        VAHT_SWAP_U16(ret->records[i].blst_id);
        VAHT_SWAP_S16(ret->records[i].name_rec);
        VAHT_SWAP_S16(ret->records[i].left);
        VAHT_SWAP_S16(ret->records[i].top);
        VAHT_SWAP_S16(ret->records[i].right);
        VAHT_SWAP_S16(ret->records[i].bottom);
        VAHT_SWAP_U16(ret->records[i].u0);
        VAHT_SWAP_U16(ret->records[i].mouse_cursor);
        VAHT_SWAP_U16(ret->records[i].index);
        VAHT_SWAP_S16(ret->records[i].u1);
        VAHT_SWAP_U16(ret->records[i].zip_mode);
        ret->scripts[i] = vaht_script_read(resource);
        if (ret->scripts[i] == NULL)
        {
            vaht_hspt_close(ret);
            return NULL;
        }
    }
    
    return ret;
}

void vaht_hspt_close(vaht_hspt* hspt)
{
    uint16_t i;
    vaht_resource_close(hspt->res);
    for (i = 0; i < hspt->count; i++)
    {
        if (hspt->scripts[i])
            vaht_script_free(hspt->scripts[i]);
        if (hspt->names[i])
            free(hspt->names[i]);
    }
    free(hspt->scripts);
    free(hspt->names);
    free(hspt->records);
    free(hspt);
}

uint16_t vaht_hspt_records(vaht_hspt* hspt)
{
    return hspt->count;
}

/* helper to reverse-map an index into our loaded records */
static inline int32_t rmap_index(vaht_hspt* hspt, uint16_t index)
{
    uint16_t i;
    for (i = 0; i < hspt->count; i++)
    {
        if (hspt->records[i].index == index)
        {
            return i;
        }
    }
    
    return -1;
}

uint16_t vaht_hspt_blst_id(vaht_hspt* hspt, uint16_t i)
{
    int32_t index = rmap_index(hspt, i);
    if (index < 0)
        return 0;
    return hspt->records[index].blst_id;
}

int16_t vaht_hspt_name_record(vaht_hspt* hspt, uint16_t i)
{
    int32_t index = rmap_index(hspt, i);
    if (index < 0)
        return -1;
    return hspt->records[index].name_rec;
}

const char* vaht_hspt_name(vaht_hspt* hspt, uint16_t i)
{
    int32_t index = rmap_index(hspt, i);
    if (index < 0)
        return NULL;
    
    if (hspt->names[index])
        return hspt->names[index];
    if (hspt->records[index].name_rec < 0)
        return NULL;
    
    /* open name resource 2 */
    vaht_resource* name_res = vaht_resource_open(hspt->res->archive, "NAME", 2);
    if (!name_res)
        return NULL;
    
    vaht_name* name = vaht_name_open(name_res);
    vaht_resource_close(name_res);
    if (!name)
        return NULL;
    
    /* we got this far, cache the name */
    hspt->names[index] = vaht_name_get(name, hspt->records[index].name_rec);
    
    vaht_name_close(name);
    return hspt->names[index];
}

void vaht_hspt_rect(vaht_hspt* hspt, uint16_t i, int16_t* left, int16_t* right, int16_t* top, int16_t* bottom)
{
    int32_t index = rmap_index(hspt, i);
    if (index < 0)
        return;
    if (left)
        *left = hspt->records[index].left;
    if (right)
        *right = hspt->records[index].right;
    if (top)
        *top = hspt->records[index].top;
    if (bottom)
        *bottom = hspt->records[index].bottom;
}

uint16_t vaht_hspt_cursor(vaht_hspt* hspt, uint16_t i)
{
    int32_t index = rmap_index(hspt, i);
    if (index < 0)
        return 0;
    return hspt->records[index].mouse_cursor;
}

uint16_t vaht_hspt_zip_mode(vaht_hspt* hspt, uint16_t i)
{
    int32_t index = rmap_index(hspt, i);
    if (index < 0)
        return 0;
    return hspt->records[index].zip_mode;
}

vaht_script* vaht_hspt_script(vaht_hspt* hspt, uint16_t i)
{
    int32_t index = rmap_index(hspt, i);
    if (index < 0)
        return NULL;
    return hspt->scripts[index];
}
