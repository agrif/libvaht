#include "vaht_intern.h"
#include <stdlib.h>
#include <string.h>

vaht_card* vaht_card_open(vaht_resource* resource)
{
	if (strcmp(vaht_resource_type(resource), "CARD") != 0)
		return NULL;
	
	vaht_card* ret = malloc(sizeof(vaht_card));
	
	ret->cached_name = NULL;
	vaht_resource_seek(resource, 0);
	
	vaht_resource_read(resource, sizeof(int16_t), &(ret->name_rec));
	VAHT_SWAP_S16(ret->name_rec);
	vaht_resource_read(resource, sizeof(uint16_t), &(ret->zip_mode_place));
	VAHT_SWAP_U16(ret->zip_mode_place);
	
	ret->script = vaht_script_read(resource);
	if (ret->script == NULL)
	{
		free(ret);
		return NULL;
	}
	
	ret->res = resource;
	vaht_resource_grab(ret->res);
	
	return ret;
}

void vaht_card_close(vaht_card* card)
{
	if (card->cached_name)
		free(card->cached_name);
	vaht_script_free(card->script);
	vaht_resource_close(card->res);
	free(card);
}

int16_t vaht_card_name_record(vaht_card* card)
{
	return card->name_rec;
}

const char* vaht_card_name(vaht_card* card)
{
	if (card->cached_name)
		return card->cached_name;
	if (card->name_rec < 0)
		return NULL;
	
	/* open name resource 1 */
	vaht_resource* name_res = vaht_resource_open(card->res->archive, "NAME", 1);
	if (!name_res)
		return NULL;
	
	vaht_name* name = vaht_name_open(name_res);
	vaht_resource_close(name_res);
	if (!name)
		return NULL;
	
	/* we got this far, cache the name */
	card->cached_name = vaht_name_get(name, card->name_rec);
	
	vaht_name_close(name);
	return card->cached_name;
}

uint16_t vaht_card_zip_mode(vaht_card* card)
{
	return card->zip_mode_place;
}

vaht_script* vaht_card_script(vaht_card* card)
{
	return card->script;
}

vaht_plst* vaht_card_plst_open(vaht_card* card)
{
	/* open the PLST */
	vaht_resource* plst_res = vaht_resource_open(card->res->archive, "PLST", card->res->id);
	if (!plst_res)
		return NULL;
	
	vaht_plst* plst = vaht_plst_open(plst_res);
	vaht_resource_close(plst_res);
	
	return plst;
}
