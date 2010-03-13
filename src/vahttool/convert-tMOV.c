#include "vahttool.h"

#include <stdlib.h>

int vt_convert_tMOV_write(struct vt_options* opt, vaht_resource* res, char* path)
{
	vaht_mov* mov = vaht_mov_open(res);
	if (mov == NULL)
	{
		vt_error(opt, "tMOV resource could not be converted: %04i", vaht_resource_id(res));
		return 1;
	}
	
	FILE* fp = fopen(path, "wb");
	if (!fp)
	{
		vt_error(opt, "cannot open path: %s", path);
		vaht_mov_close(mov);
		return 1;
	}
	
	uint32_t bufsize = 1024;
	uint8_t* buffer = malloc(sizeof(uint8_t) * bufsize);
	while (1)
	{
		uint32_t read = vaht_mov_read(mov, bufsize, buffer);
		fwrite(buffer, sizeof(uint8_t), read, fp);
		if (read != bufsize)
			break;
	}
	
	free(buffer);
	
	fclose(fp);
	vaht_mov_close(mov);
	
	return 0;
}
