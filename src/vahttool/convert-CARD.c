#include "vahttool.h"
#include <stdlib.h>

int vt_convert_CARD_write(struct vt_options* opt, vaht_resource* res, char* path)
{
	vaht_card* card = vaht_card_open(res);
	if (card == NULL)
	{
		vt_error(opt, "CARD resource could not be converted: %04i", vaht_resource_id(res));
		return 1;
	}
	
	FILE* fp = fopen(path, "w");
	if (!fp)
	{
		vt_error(opt, "cannot open path: %s", path);
		vaht_card_close(card);
		return 1;
	}
	
	/* do the conversion */
	fprintf(fp, "Name: %i %s\n", vaht_card_name_record(card), vaht_card_name(card));
	fprintf(fp, "Zip: %i\n", vaht_card_zip_mode(card));
	
	fprintf(fp, "\nScripts:\n\n");
	vt_convert_script_write(opt, vaht_card_script(card), fp);

	fclose(fp);
	vaht_card_close(card);
	
	return 0;
}
