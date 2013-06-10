#include "vahttool.h"
#include <stdlib.h>

static const char* event_name(enum vaht_script_event_t event)
{
	switch (event)
	{
	case EVENT_MOUSE_DOWN:
		return "mouse-down";
	case EVENT_MOUSE_STILL_DOWN:
		return "mouse-still-down";
	case EVENT_MOUSE_UP:
		return "mouse-up";
	case EVENT_MOUSE_ENTER:
		return "mouse-enter";
	case EVENT_MOUSE_WITHIN:
		return "mouse-within";
	case EVENT_MOUSE_LEAVE:
		return "mouse-leave";
	case EVENT_LOAD_CARD:
		return "load-card";
	case EVENT_CLOSE_CARD:
		return "close-card";
	case EVENT_OPEN_CARD:
		return "open-card";
	case EVENT_DISPLAY_UPDATE:
		return "display-update";
	default:
		return NULL;
	}
}

static const char* cmd_name(uint16_t cmd)
{
	const char* table[] = {
		/* 0 */
		NULL,
		"draw-bmp",
		"goto-card",
		"inline-slst",
		"play-wav",
		/* 5 */
		NULL,
		NULL,
		"set-var",
		"conditional",
		"enable-hotspot",
		/* 10 */
		"disable-hotspot",
		NULL,
		NULL,
		"set-cursor",
		"pause",
		/* 15 */
		NULL,
		NULL,
		"call",
		"transition",
		"reload",
		/* 20 */
		"disable-update",
		"enable-update",
		NULL,
		NULL,
		"increment",
		/* 25 */
		NULL,
		NULL,
		"goto-stack",
		NULL,
		NULL,
		/* 30 */
		NULL,
		NULL,
		"play-foreground-movie",
		"play-background-movie",
		NULL,
		/* 35 */
		NULL,
		NULL,
		NULL,
		NULL,
		"activate-plst",
		/* 40 */
		"activate-slst",
		NULL,
		NULL,
		"activate-blst",
		"activate-flst",
		/* 45 */
		"zip",
		"activate-mlst",
	};
	
	if (cmd < sizeof(table) / sizeof(*table))
		return table[cmd];
	return NULL;
}

static inline void doindent(FILE* fp, unsigned int indent)
{
	while (indent > 0)
	{
		fprintf(fp, "  ");
		indent--;
	}
}

static void write_commands(vaht_command** commands, FILE* fp, unsigned int indent)
{
	unsigned int i, j;
	for (i = 0; commands[i] != NULL; i++)
	{
		vaht_command* cmd = commands[i];
		if (vaht_command_branch(cmd))
		{
			/* branch! */
			uint16_t var = vaht_command_branch_variable(cmd);
			uint16_t count = vaht_command_branch_count(cmd);
			
			doindent(fp, indent);
			fprintf(fp, "switch [%i] {\n", var);
			
			for (j = 0; j < count; j++)
			{
				uint16_t value = vaht_command_branch_value(cmd, j);
				vaht_command** sub_commands = vaht_command_branch_body(cmd, j);
				doindent(fp, indent);
				if (value == 0xffff)
					fprintf(fp, "default:\n");
				else
					fprintf(fp, "case %i:\n", value);
				
				write_commands(sub_commands, fp, indent + 1);
				
				doindent(fp, indent + 1);
				fprintf(fp, "break;\n");
			}
			
			doindent(fp, indent);
			fprintf(fp, "}\n");
		} else {
			/* no branch! */
			doindent(fp, indent);
			uint16_t code = vaht_command_code(cmd);
			uint16_t argcount = vaht_command_argument_count(cmd);
			const char* name = cmd_name(code);
			if (name)
				fprintf(fp, "%s", name);
			else
				fprintf(fp, "[%i]", code);
			for (j = 0; j < argcount; j++)
			{
				uint16_t arg = vaht_command_argument(cmd, j);
				fprintf(fp, " %i", arg);
			}
			fprintf(fp, ";\n");
		}
	}
}

static void vt_convert_script_write(struct vt_options* opt, vaht_script* script, FILE* fp)
{
	unsigned int event;
	for (event = 0; event < EVENT_COUNT; event++)
	{
		vaht_command** commands = vaht_script_handler(script, event);
		if (commands == NULL)
			continue;
		const char* name = event_name(event);
		fprintf(fp, "event ");
		if (name)
			fprintf(fp, "%s", name);
		else
			fprintf(fp, "[%i]", event);
		fprintf(fp, " {\n");
		write_commands(commands, fp, 1);
		fprintf(fp, "}\n\n");
	}
}

int vt_convert_CARD_write(struct vt_options* opt, vaht_resource* res, char* path)
{
	vaht_card* card = vaht_card_open(res);
	if (card == NULL)
	{
		vt_error(opt, "CARD resource could not be converted: %04i", vaht_resource_id(res));
		return 1;
	}
	
	FILE* fp = fopen(path, "w");
	if (!fp)
	{
		vt_error(opt, "cannot open path: %s", path);
		vaht_card_close(card);
		return 1;
	}
	
	/* do the conversion */
	fprintf(fp, "Name: %i %s\n", vaht_card_name_record(card), vaht_card_name(card));
	fprintf(fp, "Zip: %i\n", vaht_card_zip_mode(card));
	
	fprintf(fp, "\nScripts:\n\n");
	vt_convert_script_write(opt, vaht_card_script(card), fp);

	fclose(fp);
	vaht_card_close(card);
	
	return 0;
}
