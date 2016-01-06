#include "vahttool.h"
#include <stdlib.h>

int vt_convert_RMAP_write(struct vt_options* opt, vaht_resource* res, char* path)
{
    vaht_rmap* rmap = vaht_rmap_open(res);
    if (rmap == NULL)
    {
        vt_error(opt, "RMAP resource could not be converted: %04i", vaht_resource_id(res));
        return 1;
    }
    
    FILE* fp = fopen(path, "w");
    if (!fp)
    {
        vt_error(opt, "cannot open path: %s", path);
        vaht_rmap_close(rmap);
        return 1;
    }
    
    /* do the conversion */
    uint16_t count = vaht_rmap_count(rmap);
    uint16_t i;
    for (i = 0; i < count; i++)
    {
        uint32_t code = vaht_rmap_get(rmap, i);
        fprintf(fp, "%i\t%i\n", i, code);
    }
    
    fclose(fp);
    vaht_rmap_close(rmap);
    
    return 0;
}
