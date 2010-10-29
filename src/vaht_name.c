#include "vaht_intern.h"
#include <stdlib.h>
#include <string.h>

vaht_name* vaht_name_open(vaht_resource* resource)
{
	if (strcmp(vaht_resource_type(resource), "NAME") != 0)
		return NULL;
	
	vaht_name* ret = malloc(sizeof(vaht_name));
	
	vaht_resource_seek(resource, 0);
	
	ret->res = resource;
	vaht_resource_grab(ret->res);
	
	return ret;
}

void vaht_name_close(vaht_name* name)
{
	vaht_resource_close(name->res);
	free(name);
}
