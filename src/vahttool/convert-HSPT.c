#include "vahttool.h"
#include <stdlib.h>

int vt_convert_HSPT_write(struct vt_options* opt, vaht_resource* res, char* path)
{
    vaht_hspt* hspt = vaht_hspt_open(res);
    if (hspt == NULL)
    {
        vt_error(opt, "HSPT resource could not be converted: %04i", vaht_resource_id(res));
        return 1;
    }
    
    FILE* fp = fopen(path, "w");
    if (!fp)
    {
        vt_error(opt, "cannot open path: %s", path);
        vaht_hspt_close(hspt);
        return 1;
    }
    
    /* do the conversion */
    uint16_t count = vaht_hspt_records(hspt);
    uint16_t i;
    for (i = 1; i <= count; i++)
    {
        const char* name = vaht_hspt_name(hspt, i);
        uint16_t blst_id = vaht_hspt_blst_id(hspt, i);
        int16_t left, right, top, bottom;
        vaht_hspt_rect(hspt, i, &left, &right, &top, &bottom);
        uint16_t cursor = vaht_hspt_cursor(hspt, i);
        uint16_t zip = vaht_hspt_zip_mode(hspt, i);
        fprintf(fp, "Hotspot, Name: %s\n", name);
        fprintf(fp, "#\tblst_id\t-- L\tR\tT\tB\t-- cursor\tzip\n");
        fprintf(fp, "%i\t%i\t-- %i\t%i\t%i\t%i\t-- %i\t\t%i\n", i, blst_id, left, right, top, bottom, cursor, zip);
        fprintf(fp, "\nScripts:\n\n");
        vt_convert_script_write(opt, vaht_hspt_script(hspt, i), fp);
    }

    fclose(fp);
    vaht_hspt_close(hspt);
    
    return 0;
}
