#include "vahttool.h"

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

// these MUST BE FREED!
static char* path_join(char* base, char* tail)
{
	// this is a hack, sort of. I don't know how cross-platform it is
	// oh well...
	
	char* ret = malloc(sizeof(char) * (strlen(base) + strlen(tail) + 2));
	sprintf(ret, "%s/%s", base, tail);
	return ret;
}

// also must be freed
static char* make_path_safe(char* name)
{
	char* ret = malloc(sizeof(char) * (strlen(name) + 1));
	strcpy(ret, name);
	unsigned int i;
	for (i = 0; i < strlen(ret); i++)
	{
		// we should add more checks
		if (ret[i] == '/')
			ret[i] = '-';
	}
	
	return ret;
}

// still needs freeing!
static char* construct_path(vaht_resource* res, char* out, char* ext)
{
	char* name = vaht_resource_name(res);
	if (strlen(name) > 0)
		name = make_path_safe(name);
	// 11 = 4 + 1 + ... + 1 + 4 + 1
	// that is
	// '0000.[stuff].type\0'
	char* filename = malloc(sizeof(char) * (strlen(name) + 11));
	
	char* type = vaht_resource_type(res);
	if (ext == NULL)
		ext = type;
	
	if (strlen(name) > 0)
	{
		sprintf(filename, "%04i.%s.%s", vaht_resource_id(res), name, ext);
	} else {
		sprintf(filename, "%04i.%s", vaht_resource_id(res), ext);
	}
	
	// we don't need this anymore
	if (strlen(name) > 0)
	{
		free(name);
		name = NULL;
	}
	
	char* directory;
	char* path;
	
	directory = path_join(out, type);
	path = path_join(directory, filename);
	
	free(directory);
	directory = NULL;
	free(filename);
	filename = NULL;
	
	return path;
}

// this also requires freeing!
static char* construct_output_path(struct vt_options* opt, char* out, char* archivename)
{
	char* basename = malloc(sizeof(char) * (strlen(archivename) + 1));
	strcpy(basename, archivename);
	unsigned int startchar = 0;
	unsigned int endchar = strlen(archivename);
	
	unsigned int i;
	for (i = 0; i < strlen(archivename); i++)
	{
		if (archivename[i] == '/')
		{
			startchar = i + 1;
			endchar = strlen(archivename);
		}
		if (archivename[i] == '.')
			endchar = i;
	}
	
	// sanity checks
	if (startchar >= endchar)
	{
		// something funky happened
		return NULL;
	}
	
	// startchar is now set to the char after the last '/' (or 0)
	// endchar is now set to the last '.' (or the end)
	// AND endchar - startchar > 0
	
	for (i = startchar; i < endchar; i++)
	{
		basename[i - startchar] = archivename[i];
	}
	basename[endchar - startchar] = 0;
	
	char* ret = path_join(out, basename);
	
	free(basename);
	basename = NULL;
	
	return ret;
}

static int create_directory(struct vt_options* opt, char* name)
{
	// we set all permissions, umask will handle the rest
	if (mkdir(name, S_IRWXU | S_IRWXG | S_IRWXO) == -1 && errno != EEXIST)
	{
		vt_error(opt, "could not make directory: %s", name);
		return 1;
	}
	return 0;
}

static char* get_ext(struct vt_options* opt, char* type)
{
	if (opt->convert == 0)
		return NULL;
	if (strcmp("tBMP", type) == 0)
		return "png";
	if (strcmp("tMOV", type) == 0)
		return "mov";
	if (strcmp("tWAV", type) == 0)
		return "wav";
	return NULL;
}

static int write_resource(struct vt_options* opt, vaht_resource* res, char* path)
{
	if (opt->convert)
	{
		char* type = vaht_resource_type(res);
		
		vt_inform(opt, " %s", path);
		
		if (strcmp("tBMP", type) == 0)
			return vt_convert_tBMP_write(opt, res, path);
		if (strcmp("tMOV", type) == 0)
			return vt_convert_tMOV_write(opt, res, path);
		if (strcmp("tWAV", type) == 0)
			return vt_convert_tWAV_write(opt, res, path);
		return 0;
	}
	
	FILE* f = fopen(path, "wb");
	if (f == NULL)
	{
		vt_error(opt, "cannot open path: %s", path);
		return 1;
	}
	
	vt_inform(opt, " %s", path);
	
	uint32_t bufsize = 1024;
	uint8_t* buffer = malloc(sizeof(uint8_t) * bufsize);
	while (1)
	{
		uint32_t read = vaht_resource_read(res, bufsize, buffer);
		fwrite(buffer, sizeof(uint8_t), read, f);
		if (read != bufsize)
			break;
	}
	
	fclose(f);
	return 0;
}

static int extract_archive(struct vt_options* opt, vaht_archive* archive, char* out)
{
	uint16_t resource_types_count = vaht_archive_get_resource_types(archive);
	unsigned int t;
	for (t = 0; t < resource_types_count; t++)
	{
		char* type = vaht_archive_get_resource_type(archive, t);
		
		// check if we pass the type filter
		if (opt->filter_type != NULL && strcmp(opt->filter_type, type) != 0)
		{
			// we have failed the type filter!
			continue;
		}

		// if we're converting, but there's no extension for this type,
		// skip it
		if (opt->convert && get_ext(opt, type) == NULL)
			continue;
		
		char* newdir = path_join(out, type);
		if (create_directory(opt, newdir))
		{
			free(newdir);
			return 1;
		}
		
		free(newdir);
		
		vaht_resource** resources = vaht_resources_open(archive, type);
		
		unsigned int r;
		for (r = 0; resources[r] != NULL; r++)
		{
			// check if we pass the id filter
			if (opt->filter_id != -1 && opt->filter_id != vaht_resource_id(resources[r]))
			{
				// we failed the resource id filter!
				continue;
			}
			
			char* ext = get_ext(opt, type);
			char* path = construct_path(resources[r], out, ext);
			
			if (write_resource(opt, resources[r], path))
			{
				vaht_resources_close(resources);
				free(path);
				return 1;
			}
			
			free(path);
		}
		
		vaht_resources_close(resources);
	}
	
	return 0;
}

int vt_mode_extract(struct vt_options* opt)
{
	int error = 0;
	
	unsigned int input_file;
	for (input_file = 0; input_file < opt->input_files_count; input_file++)
	{
		vaht_archive* archive = vaht_archive_open(opt->input_files[input_file]);
		
		if (archive == NULL)
		{
			vt_error(opt, "file \"%s\" is not a valid MHK archive", opt->input_files[input_file]);
			return 1;
		}
		
		vt_log(opt, "extracting %s ...", opt->input_files[input_file]);
		
		char* out = opt->output;
		if (out == NULL)
		{
			out = ".";
		}
		
		// create "out"
		if (create_directory(opt, out))
			return 1;
		
		// special handling if output provided, only one archive
		if (opt->output != NULL && opt->input_files_count == 1)
		{
			if (create_directory(opt, out))
			{
				vaht_archive_close(archive);
				return 1;
			}
			
			error = extract_archive(opt, archive, out);
		} else {
			char* newout = construct_output_path(opt, out, opt->input_files[input_file]);
			if (create_directory(opt, newout))
			{
				free(newout);
				vaht_archive_close(archive);
				return 1;
			}
			
			error = extract_archive(opt, archive, newout);
			free(newout);
		}
		
		vaht_archive_close(archive);
		
		if (error)
			break;
	}

	return error;
}
