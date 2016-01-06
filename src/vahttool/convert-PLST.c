#include "vahttool.h"
#include <stdlib.h>

int vt_convert_PLST_write(struct vt_options* opt, vaht_resource* res, char* path)
{
    vaht_plst* plst = vaht_plst_open(res);
    if (plst == NULL)
    {
        vt_error(opt, "PLST resource could not be converted: %04i", vaht_resource_id(res));
        return 1;
    }
    
    FILE* fp = fopen(path, "w");
    if (!fp)
    {
        vt_error(opt, "cannot open path: %s", path);
        vaht_plst_close(plst);
        return 1;
    }
    
    /* do the conversion */
    fprintf(fp, "#\ttBMP\t-- L\tR\tT\tB\n");
    uint16_t count = vaht_plst_records(plst);
    uint16_t i;
    for (i = 1; i <= count; i++)
    {
        uint16_t bitmap_id = vaht_plst_bitmap_id(plst, i);
        uint16_t left, right, top, bottom;
        vaht_plst_rect(plst, i, &left, &right, &top, &bottom);
        
        fprintf(fp, "%i\t%i\t-- %i\t%i\t%i\t%i\n", i, bitmap_id, left, right, top, bottom);
    }

    fclose(fp);
    vaht_plst_close(plst);
    
    return 0;
}
