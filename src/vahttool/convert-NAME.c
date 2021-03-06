#include "vahttool.h"
#include <stdlib.h>

int vt_convert_NAME_write(struct vt_options* opt, vaht_resource* res, char* path)
{
    vaht_name* name = vaht_name_open(res);
    if (name == NULL)
    {
        vt_error(opt, "NAME resource could not be converted: %04i", vaht_resource_id(res));
        return 1;
    }
    
    FILE* fp = fopen(path, "w");
    if (!fp)
    {
        vt_error(opt, "cannot open path: %s", path);
        vaht_name_close(name);
        return 1;
    }
    
    /* do the conversion */
    uint16_t count = vaht_name_count(name);
    uint16_t i;
    for (i = 0; i < count; i++)
    {
        char* str = vaht_name_get(name, i);
        fprintf(fp, "%i\t%s\n", i, str);
        free(str);
    }
    
    fclose(fp);
    vaht_name_close(name);
    
    return 0;
}
