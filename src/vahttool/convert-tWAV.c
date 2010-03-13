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
	
	vaht_wav_close(wav);
	
	return 0;
}
