#include "vaht_intern.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void handle_skip(vaht_wav* wav, uint32_t chunk_size)
{
	// this chunk provides us NO useful info, so skip
	uint32_t pos = vaht_resource_tell(wav->res);
	vaht_resource_seek(wav->res, pos + chunk_size);
}

static void handle_data(vaht_wav* wav, uint32_t chunk_size)
{
	// this chunk_size LIES, so fix it up
	chunk_size -= 8;
	
	struct
	{
		uint16_t sample_rate;
		uint32_t sample_count;
		uint8_t bits_per_sample;
		uint8_t channels;
		uint16_t encoding;
		uint16_t loop;
		uint32_t loop_start;
		uint32_t loop_end;
	} header;
	
	vaht_resource_read(wav->res, sizeof(header), &header);
	VAHT_SWAP_U16(header.sample_rate);
	VAHT_SWAP_U32(header.sample_count);
	VAHT_SWAP_U16(header.encoding);
	VAHT_SWAP_U16(header.loop);
	VAHT_SWAP_U32(header.loop_start);
	VAHT_SWAP_U32(header.loop_end);
	
	wav->sample_rate = header.sample_rate;
	wav->sample_count = header.sample_count;
	wav->bits_per_sample = header.bits_per_sample;
	wav->channels = header.channels;
	
	wav->encoding = tWAV_UNKNOWN;
	if (header.encoding == 0)
		wav->encoding = tWAV_PCM;
	else if (header.encoding == 1)
		wav->encoding = tWAV_ADPCM;
	else if (header.encoding == 2)
		wav->encoding = tWAV_MP2;
	
	wav->data_seek = vaht_resource_tell(wav->res);
}

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
		
		if (strcmp(chunk_type, "Data") == 0)
		{
			handle_data(ret, chunk_size);
			break;
		}
		
		if (strcmp(chunk_type, "ADPC") == 0)
		{
			handle_skip(ret, chunk_size);
			continue;
		}

		if (strcmp(chunk_type, "Cue#") == 0)
		{
			handle_skip(ret, chunk_size);
			continue;
		}
		
		fprintf(stderr, "Unknown chunk type: %s\n", chunk_type);
		vaht_wav_close(ret);
		return NULL;
	}
	
	if (ret->encoding != tWAV_ADPCM)
	{
		fprintf(stderr, "Unsupported format...\n");
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

uint16_t vaht_wav_samplerate(vaht_wav* wav)
{
	return wav->sample_rate;
}

uint32_t vaht_wav_samplecount(vaht_wav* wav)
{
	return wav->sample_count;
}


uint8_t vaht_wav_samplesize(vaht_wav* wav)
{
	return wav->bits_per_sample;
}

uint8_t vaht_wav_channels(vaht_wav* wav)
{
	return wav->channels;
}

enum vaht_wav_encoding_t vaht_wav_encoding(vaht_wav* wav)
{
	return wav->encoding;
}
