#include "vahttool.h"
#include <stdlib.h>

static char* fades[] = {"none", "out", "in", "inout"};

int vt_convert_SLST_write(struct vt_options* opt, vaht_resource* res, char* path)
{
    vaht_slst* slst = vaht_slst_open(res);
	if (slst == NULL)
	{
		vt_error(opt, "SLST resource could not be converted: %04i", vaht_resource_id(res));
		return 1;
	}
	
	FILE* fp = fopen(path, "w");
	if (!fp)
	{
		vt_error(opt, "cannot open path: %s", path);
		vaht_slst_close(slst);
		return 1;
	}
	
	/* do the conversion */
	fprintf(fp, "#\tcount\tfade\tloop\tglobal_volume\n");
	fprintf(fp, "\tsoundid\tvolume\tbalance\n");
	uint16_t count = vaht_slst_records(slst);
	uint16_t i;
	for (i = 1; i <= count; i++)
	{
        uint16_t sound_count = vaht_slst_count(slst, i);
        uint16_t fade = vaht_slst_fade(slst, i);
        uint16_t loop = vaht_slst_loop(slst, i);
        uint16_t global_volume = vaht_slst_global_volume(slst, i);
        fprintf(fp, "%i\t%i\t%s\t%i\t%i\n", i, sound_count, fades[fade], loop, global_volume);
        uint16_t j;
        for (j = 0; j < sound_count; j++)
        {
            uint16_t sound_id = vaht_slst_sound_id(slst, i, j);
            uint16_t volume = vaht_slst_volume(slst, i, j);
            int16_t balance = vaht_slst_balance(slst, i, j);
            fprintf(fp, "\t%i\t%i\t%i\n", sound_id, volume, balance);
        }
	}

	fclose(fp);
	vaht_slst_close(slst);
	
	return 0;
}
