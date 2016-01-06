#include "vaht_intern.h"
#include <stdlib.h>
#include <string.h>

vaht_name* vaht_name_open(vaht_resource* resource)
{
    if (strcmp(vaht_resource_type(resource), "NAME") != 0)
        return NULL;
    
    vaht_name* ret = malloc(sizeof(vaht_name));
    
    vaht_resource_seek(resource, 0);
    vaht_resource_read(resource, sizeof(uint16_t), &(ret->count));
    VAHT_SWAP_U16(ret->count);
    
    ret->res = resource;
    vaht_resource_grab(ret->res);
    
    return ret;
}

void vaht_name_close(vaht_name* name)
{
    vaht_resource_close(name->res);
    free(name);
}

uint16_t vaht_name_count(vaht_name* name)
{
    return name->count;
}

char* vaht_name_get(vaht_name* name, uint16_t i)
{
    if (i >= name->count)
        return NULL;
    
    /* seek to the string offset location */
    vaht_resource_seek(name->res, sizeof(uint16_t) * (1 + i));
    
    uint16_t string_offset;
    vaht_resource_read(name->res, sizeof(uint16_t), &string_offset);
    VAHT_SWAP_U16(string_offset);
    
    /* now seek to the string */
    vaht_resource_seek(name->res, sizeof(uint16_t) * (1 + (2 * name->count)) + string_offset);
    
    /* we'll overestimate string size, and force a string end */
    char* ret = malloc(sizeof(char) * 256);
    vaht_resource_read(name->res, sizeof(char) * 256, ret);
    ret[255] = 0;
    
    return ret;
}
