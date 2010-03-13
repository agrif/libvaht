#include "vaht_intern.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

vaht_wav* vaht_wav_open(vaht_resource* resource)
{
	vaht_wav* ret = malloc(sizeof(vaht_wav));
	vaht_resource_grab(resource);
	ret->res = resource;
	
	char mhwk_magic[5];
	uint32_t effective_size;
	char wave_magic[5];
	
	vaht_resource_read(resource, sizeof(char) * 4, mhwk_magic);
	vaht_resource_read(resource, sizeof(uint32_t), &effective_size);
	vaht_resource_read(resource, sizeof(char) * 4, wave_magic);
	
	mhwk_magic[4] = 0;
	wave_magic[4] = 0;
	VAHT_SWAP_U32(effective_size);
	
	if (strcmp(mhwk_magic, "MHWK") != 0 || strcmp(wave_magic, "WAVE") != 0)
	{
		vaht_wav_close(ret);
		return NULL;
	}
	
	char chunk_type[5];
	uint32_t chunk_size;
	
	while (1)
	{
		vaht_resource_read(resource, sizeof(char) * 4, chunk_type);
		vaht_resource_read(resource, sizeof(uint32_t), &chunk_size);
		chunk_type[4] = 0;
		VAHT_SWAP_U32(chunk_size);
		
		//if (strcmp(chunk_type, "Data") == 0)
		//	break;
		
		fprintf(stderr, "Unknown chunk type: %s\n", chunk_type);
		vaht_wav_close(ret);
		return NULL;
	}

	return ret;
}

void vaht_wav_close(vaht_wav* wav)
{
	if (wav->res)
		vaht_resource_close(wav->res);
	
	free(wav);
}
