#include "vahttool.h"

#include <png.h>

int vt_convert_tBMP_write(struct vt_options* opt, vaht_resource* res, char* path)
{
	vaht_bmp* bmp = vaht_bmp_open(res);
	if (bmp == NULL)
	{
		vt_error(opt, "tBMP resource could not be converted: %04i", vaht_resource_id(res));
		return 1;
	}
	
	FILE* fp = fopen(path, "wb");
	if (!fp)
	{
		vt_error(opt, "cannot open path: %s", path);
		vaht_bmp_close(bmp);
		return 1;
	}
	
	png_structp png_ptr;
	png_infop info_ptr;
	
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	info_ptr = png_create_info_struct(png_ptr);
	if (setjmp(png_ptr->jmpbuf))
	{
		vt_error(opt, "PNG write error on: %s", path);
		vaht_bmp_close(bmp);
		fclose(fp);
		return 1;
	}
	
	png_init_io(png_ptr, fp);
	png_set_IHDR(png_ptr, info_ptr, vaht_bmp_width(bmp), vaht_bmp_height(bmp),
				 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
				 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_write_info(png_ptr, info_ptr);
	
	uint8_t* data = vaht_bmp_data(bmp);
	unsigned int i;
	for (i = 0; i < vaht_bmp_width(bmp) * vaht_bmp_height(bmp) * 3;
		 i += vaht_bmp_width(bmp) * 3)
	{
		png_write_row(png_ptr, &(data[i]));
	}
	
	png_write_end(png_ptr, info_ptr);
	png_destroy_write_struct(&png_ptr, &info_ptr);
	
	fclose(fp);
	vaht_bmp_close(bmp);
	
	return 0;
}
