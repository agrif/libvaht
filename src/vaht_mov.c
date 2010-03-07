#include "vaht_intern.h"

vaht_mov* vaht_mov_open(vaht_resource* resource)
{
	if (strcmp(vaht_resource_type(resource), "tMOV") != 0)
		return NULL;
	
	vaht_mov* ret = malloc(sizeof(vaht_mov));
	return ret;
}

void vaht_mov_close(vaht_mov* mov)
{
	free(mov);
}
