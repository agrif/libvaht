#include "vahttool.h"
#include <stdlib.h>

int vt_convert_BLST_write(struct vt_options* opt, vaht_resource* res, char* path)
{
    vaht_blst* blst = vaht_blst_open(res);
    if (blst == NULL)
    {
        vt_error(opt, "BLST resource could not be converted: %04i", vaht_resource_id(res));
        return 1;
    }
    
    FILE* fp = fopen(path, "w");
    if (!fp)
    {
        vt_error(opt, "cannot open path: %s", path);
        vaht_blst_close(blst);
        return 1;
    }
    
    /* do the conversion */
    fprintf(fp, "#\tenabled\thotspot_id\n");
    uint16_t count = vaht_blst_records(blst);
    uint16_t i;
    for (i = 1; i <= count; i++)
    {
        uint16_t enabled = vaht_blst_enabled(blst, i);
        uint16_t hotspot_id = vaht_blst_hotspot_id(blst, i);
        fprintf(fp, "%i\t%i\t%i\n", i, enabled, hotspot_id);
    }

    fclose(fp);
    vaht_blst_close(blst);
    
    return 0;
}
