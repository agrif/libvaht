#include "vaht_intern.h"
#include <stdlib.h>
#include <string.h>

vaht_rmap* vaht_rmap_open(vaht_resource* resource)
{
	if (strcmp(vaht_resource_type(resource), "RMAP") != 0)
		return NULL;
	
	/* I know, this seems like a good idea. But check out jspit RMAP 1:
	   it has 1 byte leftover! whyyyyyyyyyyyyyy?!?!?!?!?!?
	
	if (vaht_resource_size(resource) % sizeof(uint32_t) != 0)
		return NULL;
	*/
	
	vaht_rmap* ret = malloc(sizeof(vaht_rmap));
	ret->count = vaht_resource_size(resource) / sizeof(uint32_t);	
	ret->res = resource;
	vaht_resource_grab(ret->res);
	
	return ret;
}

void vaht_rmap_close(vaht_rmap* rmap)
{
	vaht_resource_close(rmap->res);
	free(rmap);
}

uint16_t vaht_rmap_count(vaht_rmap* rmap)
{
	return rmap->count;
}

uint32_t vaht_rmap_get(vaht_rmap* rmap, uint16_t i)
{
	if (i >= rmap->count)
		return 0;
	
	/* seek to the code offset location */
	vaht_resource_seek(rmap->res, sizeof(uint32_t) * i);
	
	uint32_t code;
	vaht_resource_read(rmap->res, sizeof(uint32_t), &code);
	VAHT_SWAP_U32(code);
	
	return code;
}
