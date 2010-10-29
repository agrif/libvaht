#include "vahttool.h"

#include <stdlib.h>

int vt_convert_tWAV_write(struct vt_options* opt, vaht_resource* res, char* path)
{
	vaht_wav* wav = vaht_wav_open(res);
	if (wav == NULL)
	{
		vt_error(opt, "tWAV resource could not be converted: %04i", vaht_resource_id(res));
		return 1;
	}
	
	FILE* fp = fopen(path, "wb");
	if (fp == NULL)
	{
		vt_error(opt, "cannot open path: %s", path);
		vaht_wav_close(wav);
		return 1;
	}
	
	uint32_t tmp32;
	uint16_t tmp16;
	uint32_t written;

	/* bytes per sample */
	uint8_t bps = vaht_wav_samplesize(wav) / 8;
	
	/* write out the WAV headers */
	fprintf(fp, "RIFF");
	tmp32 = 36 + (vaht_wav_channels(wav) * vaht_wav_samplecount(wav) * bps);
	written = fwrite(&tmp32, sizeof(uint32_t), 1, fp);
	if (written != 1)
		return 1;
	fprintf(fp, "WAVE");
	
	/* WAV headers: format */
	fprintf(fp, "fmt ");
	tmp32 = 16;
	written = fwrite(&tmp32, sizeof(uint32_t), 1, fp);
	if (written != 1)
		return 1;
	tmp16 = 1;
	written = fwrite(&tmp16, sizeof(uint16_t), 1, fp);
	if (written != 1)
		return 1;
	tmp16 = vaht_wav_channels(wav);
	written = fwrite(&tmp16, sizeof(uint16_t), 1, fp);
	if (written != 1)
		return 1;
	tmp32 = vaht_wav_samplerate(wav);
	written = fwrite(&tmp32, sizeof(uint32_t), 1, fp);
	if (written != 1)
		return 1;
	tmp32 = vaht_wav_samplerate(wav) * vaht_wav_channels(wav) * bps;
	written = fwrite(&tmp32, sizeof(uint32_t), 1, fp);
	if (written != 1)
		return 1;
	tmp16 = vaht_wav_channels(wav) * bps;
	written = fwrite(&tmp16, sizeof(uint16_t), 1, fp);
	if (written != 1)
		return 1;
	tmp16 = vaht_wav_samplesize(wav);
	written = fwrite(&tmp16, sizeof(uint16_t), 1, fp);
	if (written != 1)
		return 1;
	
	/* WAV headers: data */
	fprintf(fp, "data");
	tmp32 = vaht_wav_samplecount(wav) * vaht_wav_channels(wav) * bps;
	written = fwrite(&tmp32, sizeof(uint32_t), 1, fp);
	if (written != 1)
		return 1;
	
	/* what follows... data */
	uint32_t bufsize = 1024; /* a multiple of 4 (!!!) */
	uint8_t* buffer = malloc(sizeof(uint8_t) * bufsize);
	
	while (1)
	{
		uint32_t read = vaht_wav_read(wav, bufsize, buffer);
		written = fwrite(buffer, sizeof(uint8_t), read, fp);
		if (written != read || read != bufsize)
			break;
	}
	
	free(buffer);
	
	fclose(fp);
	vaht_wav_close(wav);
	
	return 0;
}
