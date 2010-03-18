#include "vahttool.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

static void print_version()
{
	fprintf(stderr, "vahttool " VAHTTOOL_VERSION "\n");
	fprintf(stderr, "Copyright (C) 2010 " VAHTTOOL_AUTHOR ".\n");
}

static void print_help()
{
	fprintf(stderr, "Usage: vahttool [OPTIONS] [archive.MHK ...]\n");
	fprintf(stderr, "vahttool extracts and inspects riven MHK archives.\n");

	fprintf(stderr, "\n Operation modes:\n\n");
	fprintf(stderr, "  -l, --list         list files in archive\n");
	fprintf(stderr, "  -x, --extract      extract the files\n");
	
	fprintf(stderr, "\n Extract options:\n\n");
	fprintf(stderr, "  -o, --output [dir] output extracted files to [dir]\n");
	fprintf(stderr, "  -c, --convert      convert resources to more readable types, like png images\n");
	fprintf(stderr, "                     (note, types not yet supported will *not* be extracted.)\n");

	fprintf(stderr, "\n Resource filters:\n\n");
	fprintf(stderr, "  -t, --type [type]  only operate on resources of type [type]\n");
	fprintf(stderr, "  -i, --id [id]      only operate on resources with id [id]\n");
	
	fprintf(stderr, "\n Console output control:\n\n");
	fprintf(stderr, "  -v, --verbose      print out more information than usual\n");
	fprintf(stderr, "  -q, --quiet        print out as little info as possible\n");
	
	fprintf(stderr, "\n Other options:\n\n");
	fprintf(stderr, "  -h, --help         give this help list\n");
	fprintf(stderr, "      --version      give program version\n");
	
	fprintf(stderr, "\nReport bugs to <" VAHTTOOL_EMAIL ">\n");
	/* reference 80 "12345678901234567890123456789012345678901234567890123456789012345678901234567890"); */
}

static void set_defaults(struct vt_options* opt)
{
	opt->mode = NONE;
	opt->verbosity = NORMAL;
	opt->input_files = NULL;
	opt->input_files_count = 0;
	opt->output = NULL;
	opt->filter_id = -1;
	opt->filter_type = NULL;
	opt->convert = 0;
}

int vt_options_parse(struct vt_options* opt, int argc, char** argv)
{
	set_defaults(opt);
	
	struct option long_options[] =
	{
		{"version", no_argument, 0, 0},
		{"verbose", no_argument, 0, 'v'},
		{"quiet", no_argument, 0, 'q'},
		{"help", no_argument, 0, 'h'},
		{"list", no_argument, 0, 'l'},
		{"extract", no_argument, 0, 'x'},
		{"output", required_argument, 0, 'o'},
		{"convert", no_argument, 0, 'c'},
		{"type", required_argument, 0, 't'},
		{"id", required_argument, 0, 'i'},
		{0, 0, 0, 0}
	};
	
	int option_index = 0;
	
	while (1)
	{
		char c = getopt_long(argc, argv, "vqhlxo:ct:i:", long_options, &option_index);

		if (c == -1)
			break;
		
		switch (c)
		{
		case 'v':
			opt->verbosity = VERBOSE;
			break;
		case 'q':
			opt->verbosity = QUIET;
			break;
		case 'h':
			print_help();
			return -1;
		case 'l':
			opt->mode = LIST;
			break;
		case 'x':
			opt->mode = EXTRACT;
			break;
		case 'o':
			opt->output = malloc(sizeof(char) * (strlen(optarg) + 1));
			strcpy(opt->output, optarg);
			break;
		case 'c':
			opt->convert = 1;
			break;
		case 't':
			opt->filter_type = malloc(sizeof(char) * 5);
			strcpy(opt->filter_type, optarg);
			break;
		case 'i':
			opt->filter_id = atoi(optarg);
			break;
		case '?':
			// an error was already printed
			return 1;
		default:
			// we have to check the option indexes now...
			switch (option_index)
			{
			case 0: // version
				print_version();
				return -1;
			default:
				fprintf(stderr, "An odd error occurred.\n");
				return 1;
			};
		};
	}
	
	if (optind < argc)
	{
		// we have input files!
		opt->input_files_count = argc - optind;
		opt->input_files = &argv[optind];
	} else {
		// there were no input files
		fprintf(stderr, "vahttool: error: no input files given.\n");
		fprintf(stderr, "Use `vahttool --help' for more information.\n");
		return 1;
	}

	if (opt->mode == NONE)
	{
		fprintf(stderr, "vahttool: error: no operation mode given.\n");
		fprintf(stderr, "Use `vahttool --help' for a list of modes.\n");
		return 1;
	}
	
	return 0;
}

void vt_options_free(struct vt_options* opt)
{
	if (opt->output)
	{
		free(opt->output);
	}

	if (opt->filter_type)
	{
		free(opt->filter_type);
	}
}

