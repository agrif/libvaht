#include "vahttool.h"

int main(int argc, char** argv)
{
    struct vt_options opt;
    int error = vt_options_parse(&opt, argc, argv);
    if (error != 0)
    {
        /* we're done here */
        vt_options_free(&opt);

        if (error < 0)
        {
            /* a negative means "exit, but not error" */
            return 0;
        }
        return error;
    }
    
    switch (opt.mode)
    {
    case LIST:
        error = vt_mode_list(&opt);
        break;
    case EXTRACT:
        error = vt_mode_extract(&opt);
    default:
        error = 1;
        break;
    };
    
    vt_options_free(&opt);
    
    if (error > 0)
    {
        vt_error(&opt, "Aborting.");
        return error;
    }
    return 0;
}
