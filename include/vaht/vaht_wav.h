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
uint16_t vaht_wav_samplerate(vaht_wav* wav);
uint32_t vaht_wav_samplecount(vaht_wav* wav);
// this is measured in BITS
uint8_t vaht_wav_samplesize(vaht_wav* wav);
uint8_t vaht_wav_channels(vaht_wav* wav);
enum vaht_wav_encoding_t vaht_wav_encoding(vaht_wav* wav);
// size MUST BE a multiple of 4 (weird, huh)
uint32_t vaht_wav_read(vaht_wav* wav, uint32_t size, void* buffer);
void vaht_wav_reset(vaht_wav* wav);

#endif // __INCLUDE_VAHT_WAV_H__
