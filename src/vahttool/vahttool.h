#ifndef __INCLUDE_VAHTTOOL_H__
#define __INCLUDE_VAHTTOOL_H__

#include <vaht/vaht.h>
#include <stdio.h>

#define VAHTTOOL_VERSION PACKAGE_VERSION
#define VAHTTOOL_AUTHOR "Aaron Griffith"
#define VAHTTOOL_EMAIL PACKAGE_BUGREPORT

/* always print! */
#define vt_error(_opt, ...) fprintf(stderr, "vahttool: " __VA_ARGS__); fprintf(stderr, "\n");
#define vt_print(_opt, ...) fprintf(stdout, __VA_ARGS__); fprintf(stdout, "\n");
/* print unless --quiet */
#define vt_log(_opt, ...) if (_opt->verbosity != QUIET) { fprintf(stdout, __VA_ARGS__); fprintf(stdout, "\n"); }
/* print only if --verbose */
#define vt_inform(_opt, ...) if (_opt->verbosity == VERBOSE) { fprintf(stdout, __VA_ARGS__); fprintf(stdout, "\n"); }

/* options.c */

struct vt_options
{
	enum
	{
		VERBOSE, NORMAL, QUIET
	} verbosity;
	
	enum
	{
		NONE, LIST, EXTRACT
	} mode;
	
	/* filter_id == -1 means don't use! */
	int filter_id;
	char* filter_type;
	
	char** input_files;
	unsigned int input_files_count;
	
	char* output;
	
	int convert; /* 0 means no, 1 means yes (duh) */
};

int vt_options_parse(struct vt_options* opt, int argc, char** argv);
void vt_options_free(struct vt_options* opt);

/* mode-list.c */
int vt_mode_list(struct vt_options* opt);

/* mode-extract.c */
int vt_mode_extract(struct vt_options* opt);

/* convert-tBMP.c */
int vt_convert_tBMP_write(struct vt_options* opt, vaht_resource* res, char* path);

/* convert-tMOV.c */
int vt_convert_tMOV_write(struct vt_options* opt, vaht_resource* res, char* path);

/* convect-tWAV.c */
int vt_convert_tWAV_write(struct vt_options* opt, vaht_resource* res, char* path);

/* convect-NAME.c */
int vt_convert_NAME_write(struct vt_options* opt, vaht_resource* res, char* path);

#endif /* __INCLUDE_VAHTTOOL_H__ */
