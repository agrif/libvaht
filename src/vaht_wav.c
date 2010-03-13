#include "vaht_intern.h"
#include <stdlib.h>

vaht_wav* vaht_wav_open(vaht_resource* resource)
{
	vaht_wav* ret = malloc(sizeof(vaht_wav));
	vaht_resource_grab(resource);
	ret->res = resource;
	return ret;
}

void vaht_wav_close(vaht_wav* wav)
{
	if (wav->res)
		vaht_resource_close(wav->res);
	
	free(wav);
}
