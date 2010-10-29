#include "vahttool.h"

#include <string.h>

static void list_files(struct vt_options* opt, vaht_archive* archive)
{
	uint16_t resource_types_count = vaht_archive_get_resource_types(archive);
	
	unsigned int t;
	for (t = 0; t < resource_types_count; t++)
	{
		const char* type = vaht_archive_get_resource_type(archive, t);
		
		/* check if we pass the type filter */
		if (opt->filter_type != NULL && strcmp(opt->filter_type, type) != 0)
		{
			/* we have failed the type filter! */
			continue;
		}
		
		vaht_resource** resources = vaht_resources_open(archive, type);
		
		unsigned int r;
		for (r = 0; resources[r] != NULL; r++)
		{
			/* check if we pass the id filter */
			if (opt->filter_id != -1 && opt->filter_id != vaht_resource_id(resources[r]))
			{
				/* we failed the resource id filter! */
				continue;
			}
			
			vt_print(opt, " [%s %04i] %s",
					 vaht_resource_type(resources[r]),
					 vaht_resource_id(resources[r]),
					 vaht_resource_name(resources[r])
				);
			vt_inform(opt, "             - size: %i bytes", vaht_resource_size(resources[r]));
		}
		
		vaht_resources_close(resources);
	}
}

int vt_mode_list(struct vt_options* opt)
{
	unsigned int i;
	for (i = 0; i < opt->input_files_count; i++)
	{
		vaht_archive* archive = vaht_archive_open(opt->input_files[i]);
		
		if (archive == NULL)
		{
			/* we've had a problem opening the file... */
			vt_error(opt, "file \"%s\" is not a valid MHK archive", opt->input_files[i]);
			return 1;
		}
		
		/* we may need to specify which file we're outputting */
		if (opt->input_files_count > 1)
		{
			vt_print(opt, "%s :", opt->input_files[i]);
		}
		
		list_files(opt, archive);
		
		vaht_archive_close(archive);
	}
	
	return 0;
}
