#include "vaht_intern.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static int8_t adpcm_index_table[16] = {
	-1, -1, -1, -1, 2, 4, 6, 8,
	-1, -1, -1, -1, 2, 4, 6, 8
};

static uint16_t adpcm_step_table[89] = { 
	7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 
	19, 21, 23, 25, 28, 31, 34, 37, 41, 45, 
	50, 55, 60, 66, 73, 80, 88, 97, 107, 118, 
	130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
	337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
	876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066, 
	2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
	5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899, 
	15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767 
};

// turn a step, (signed) nibble into a delta
static inline int32_t adpcm_delta(uint16_t step, uint8_t n)
{
	int32_t delta = step >> 3;
	if (n & (1 << 0))
		delta += (step >> 2);
	if (n & (1 << 1))
		delta += (step >> 1);
	if (n & (1 << 2))
		delta += (step >> 0);
	
	// sign bit
	if (n & (1 << 3))
		return -delta;
	return delta;
}

static inline uint32_t adpcm_read(vaht_wav* wav, uint32_t size, void* buffer)
{
	// input buffer, 4 bits per sample, (size / 4) bytes
	uint32_t insize = size / 4;
	uint8_t* input = malloc(sizeof(uint8_t) * insize);
	
	insize = vaht_resource_read(wav->res, insize, input);
	
	// output buffer, 16 bits per sample, insize * 2 samples
	int16_t* output = buffer;
	uint16_t outcount = insize * 2;
	
	unsigned int i;
	for (i = 0; i < outcount; i++)
	{
		uint8_t nibble = input[i / 2];
		
		// select the correct nibble from the byte
		nibble &= 0x0F << (4 * (i % 2));
		nibble = nibble >> (4 * (i % 2));
		
		// do a decode loop
		
		struct vaht_wav_adpcm_decode_state* state = wav->adpcm_state[i % 2];
		
		state->index += adpcm_index_table[nibble];
		// clamp index to [0, 88]
		state->index = state->index > 88 ? 88 : state->index;
		state->index = state->index < 0 ? 0 : state->index;
		
		state->predictor += adpcm_delta(state->step, nibble);
		// clamp predictor to [-32768, 32767]
		state->predictor = state->predictor > 32767 ? 32767 : state->predictor;
		state->predictor = state->predictor < -32768 ? -32768 : state->predictor;
		
		state->step = adpcm_step_table[state->index];
		
		// write our sample
		output[i] = state->predictor;
	}
	
	free(input);

	return outcount * 2;
}

static inline void handle_skip(vaht_wav* wav, uint32_t chunk_size)
{
	// this chunk provides us NO useful info, so skip
	uint32_t pos = vaht_resource_tell(wav->res);
	vaht_resource_seek(wav->res, pos + chunk_size);
}

static inline void handle_data(vaht_wav* wav, uint32_t chunk_size)
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
	
	ret->adpcm_state[0] = NULL;
	ret->adpcm_state[1] = NULL;
	
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
	
	if (ret->channels > 2)
	{
		fprintf(stderr, "Can't handle more than 2 channels...\n");
		vaht_wav_close(ret);
		return NULL;
	}
	
	if (ret->encoding != tWAV_ADPCM)
	{
		fprintf(stderr, "Unsupported format...\n");
		vaht_wav_close(ret);
		return NULL;
	}

	// prepare to decode
	vaht_wav_reset(ret);
	
	return ret;
}

void vaht_wav_close(vaht_wav* wav)
{
	if (wav->res)
		vaht_resource_close(wav->res);
	
	if (wav->adpcm_state[0])
		free(wav->adpcm_state[0]);
	if (wav->adpcm_state[1] && wav->adpcm_state[1] != wav->adpcm_state[0])
		free(wav->adpcm_state[1]);
	
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

uint32_t vaht_wav_read(vaht_wav* wav, uint32_t size, void* buffer)
{
	// our arcane restriction so that ADPCM decoding always ends on a byte
	// boundary
	if (size % 4 != 0)
		return 0;
	
	if (wav->encoding == tWAV_ADPCM)
		return adpcm_read(wav, size, buffer);
	
	return 0;
}

void vaht_wav_reset(vaht_wav* wav)
{
	if (wav->adpcm_state[0])
		free(wav->adpcm_state[0]);
	if (wav->adpcm_state[1] && wav->adpcm_state[1] != wav->adpcm_state[0])
		free(wav->adpcm_state[1]);
	wav->adpcm_state[0] = NULL;
	wav->adpcm_state[1] = NULL;
	
	if (wav->encoding == tWAV_ADPCM)
	{
		wav->adpcm_state[0] = malloc(sizeof(struct vaht_wav_adpcm_decode_state));
		wav->adpcm_state[0]->index = 0;
		wav->adpcm_state[0]->predictor = 0;
		wav->adpcm_state[0]->step = adpcm_step_table[0];

		if (wav->channels == 2)
		{
			wav->adpcm_state[1] = malloc(sizeof(struct vaht_wav_adpcm_decode_state));
			wav->adpcm_state[1]->index = 0;
			wav->adpcm_state[1]->predictor = 0;
			wav->adpcm_state[1]->step = adpcm_step_table[0];
		}
		else
		{
			wav->adpcm_state[1] = wav->adpcm_state[0];
		}
	}
	
	vaht_resource_seek(wav->res, wav->data_seek);
}
