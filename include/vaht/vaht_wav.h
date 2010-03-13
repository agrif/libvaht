#ifndef __INCLUDE_VAHT_WAV_H__
#define __INCLUDE_VAHT_WAV_H__

#include <stdint.h>
#include "vaht_resource.h"

struct vaht_wav_s;
typedef struct vaht_wav_s vaht_wav;

enum vaht_wav_encoding_t
{
	tWAV_UNKNOWN, tWAV_PCM, tWAV_ADPCM, tWAV_MP2
};

vaht_wav* vaht_wav_open(vaht_resource* resource);
void vaht_wav_close(vaht_wav* wav);

#endif // __INCLUDE_VAHT_WAV_H__
