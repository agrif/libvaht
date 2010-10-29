#include "vaht_intern.h"
#include <stdlib.h>
#include <string.h>

vaht_card* vaht_card_open(vaht_resource* resource)
{
	if (strcmp(vaht_resource_type(resource), "CARD") != 0)
		return NULL;
	
	vaht_card* ret = malloc(sizeof(vaht_card));
	
	vaht_resource_seek(resource, 0);
	
	vaht_resource_read(resource, sizeof(int16_t), &(ret->name_rec));
	VAHT_SWAP_S16(ret->name_rec);
	vaht_resource_read(resource, sizeof(uint16_t), &(ret->zip_mode_place));
	VAHT_SWAP_U16(ret->zip_mode_place);
	
	ret->res = resource;
	vaht_resource_grab(ret->res);
	
	return ret;
}

void vaht_card_close(vaht_card* card)
{
	vaht_resource_close(card->res);
	free(card);
}
