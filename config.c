#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "install-log.h"

typedef struct ConfigData {
	char* tag;	/* Name in rc file */
	char* long_opt;	/* Name as a long option */
	char swtch;	/* Name as a commandline switch */
	void* var;	/* Variable in memory */
	int type;	/* Type of variable */
	int priority;	/* Only equal or higher priority can set this option */
} ConfigData;

#define help_type -1
#define string_type 0
#define string_list_type 1
#define int_type 2
#define bool_type 3

#define default_priority 0	/* From defaults */
#define rc_priority 1		/* From configuration file */
#define command_priority 2	/* From command line */

typedef struct option Option;

/* priority field defaults to 0 (default_priority) because it's not listed */
static ConfigData config_data[] = {
{"EDIT",	"edit",		'e',	&edit,		bool_type},
{"EDITOR",	"editor",	'E',	&editor,	string_type},
{"FORCE",	"force",	'f',	&force,		bool_type},
{NULL,		"help",		'h',	NULL,		help_type},
{"INCLUDE",	"include",	'i',	&include,	string_list_type},
{"LOGDIR",	"logdir",	'l',	&logdir,	string_type},
{"QUIET",	"quiet",	'q',	&quiet,		bool_type},
{NULL,		"root",		'r',	&root,		string_type},
{"VERBOSITY",	"verbosity",	'v',	&verbosity,	int_type},
{"EXCLUDE",	"exclude",	'x',	&exclude,	string_list_type}
};

static const int num_options = sizeof(config_data) / sizeof(ConfigData);

static void parse_command(int argc, char** argv);
static void parse_rc(const char* filename);
static char* build_short_options();
static Option* build_long_options();
static void set_option(int i, char* val, int p);
static void remove_space(char* buf);
static void remove_quotes(char* buf);
static void remove_comment(char* buf);

/* Get configuration data from all sources */
void get_config(int argc, char** argv)
{
	char* logdir2;
	char* config_file;

	init_globals(argv);			/* Get defaults */
	parse_command(argc, argv);		/* Read command line */

	config_file = safe_sprintf(NULL, NULL, "%s/etc/install-log.rc", root);
	parse_rc(config_file);			/* Read rc file */
	free(config_file);

	/* Append root to logdir */
	logdir2 = logdir;
	logdir = safe_sprintf(NULL, NULL, "%s/%s", root, logdir2);
	free(logdir2);
}

/* Handle the command line */
static void parse_command(int argc, char** argv)
{
	int c;
	int i;
	int option_index = 0;
	char* short_options;
	Option* long_options;

	short_options = build_short_options();
	long_options = build_long_options();	

	/* Parse switches */
	while ((c = getopt_long(argc, argv, short_options, long_options,
			&option_index)) != -1) {
		/* Find the corresponding config_data entry */
		for (i = 0; i < num_options; i++)
			if (c == 0 && strcmp(config_data[i].long_opt,
					long_options[option_index].name) == 0)
				break;
			else if (config_data[i].swtch == c)
				break;
		if (i == num_options) usage(EXIT_FAILURE);;
	
		set_option(i, optarg, command_priority);
	}

	free(long_options);
	free(short_options);
	
	/* Find the package name */
	if (optind == argc - 1)
		package = strdup(argv[optind]);
	else
		usage(EXIT_FAILURE);
}

/* Handle the rc file */
static void parse_rc(const char* filename)
{
	int i;
	int line_num = 0;
	size_t buf_len = 256;
	char* buf;	/* The whole line */
	char* value;	/* After the = */
	FILE* file;

	if ((file = fopen(filename, "r")) == NULL) return;
	buf = malloc(buf_len);
	
	while (true) {
		/* Read the line */
		line_num++;
		getline(&buf, &buf_len, file);
		if (feof(file)) break;
		remove_comment(buf);
		remove_space(buf);

		/* Determine if the line should be skipped */
		if (*buf == 0) continue;
		
		/* Split the string at the `=' */
		value = strchr(buf, '=');
		if (value != NULL) {
			*value = 0;
			value++;
			remove_space(value);
			remove_quotes(value);
		}
		
		remove_space(buf);
		remove_quotes(buf);

		/* Find the index which corresponds to the key */
		for (i = 0; i < num_options; i++)
			if (config_data[i].tag != NULL &&
					strcasecmp(config_data[i].tag, buf) ==
					0)
				break;
		if (i == num_options)
			fail("Parse error in %s on line %i\n", filename,
					line_num);

		set_option(i, value, rc_priority);
	}

	free(buf);
	fclose(file);
}

/* Prepare a list of short options for getopt */
static char* build_short_options()
{
	int i;
	char* o_p;
	char* o = malloc(num_options * 3 + 1);
	memset(o, 0, strlen(o));

	for (i = 0, o_p = o; i < num_options; i++) {
		/* See if this config item has a short option */
		if (config_data[i].swtch == 0) continue;

		/* Place this config item in the option list */
		*(o_p++) = config_data[i].swtch;

		/* See if there's a parameter */
		switch (config_data[i].type) {
		case bool_type:
			/* Fallthrough intentional */
			*(o_p++) = ':';
		case string_type:
		case string_list_type:
		case int_type:
			*(o_p++) = ':';
		}
	}

	return o;
	
}

/* Prepare a list of long options for getopt */
static Option* build_long_options()
{
	int i;
	Option* o_p;
	Option* o = malloc(sizeof(Option) * (num_options + 1));

	for (i = 0, o_p = o; i < num_options; i++) {
		/* See if this config item has a long option */
		if (config_data[i].long_opt == NULL) continue;

		/* Build an entry in the long option table */
		o_p->name = config_data[i].long_opt;
		switch (config_data[i].type) {
		case string_type:
		case string_list_type:
		case int_type:
			o_p->has_arg = required_argument;
			break;
		case bool_type:
			o_p->has_arg = optional_argument;
			break;
		case help_type:
			o_p->has_arg = no_argument;
		}
		o_p->flag = NULL;
		o_p->val = config_data[i].swtch;
		o_p++;
	}

	/* Make the terminating entry */
	o_p->name = NULL;
	o_p->has_arg = no_argument;
	o_p->flag = NULL;
	o_p->val = 0;

	return o;
}

/* Set variable index i to val */
static void set_option(int i, char* val, int p)
{
	/* Lower priorities cannot override higher priorities */
	if (p < config_data[i].priority) return;
	config_data[i].priority = p;

	switch (config_data[i].type) {
	case help_type:
		usage(EXIT_SUCCESS);
		break;
	case string_type: {
		char** string = (char**)config_data[i].var;
		free(*string);
		*string = strdup(val);
		break;
	} case string_list_type: {
		char* val_p;
		List* node = (List*)config_data[i].var;
		clear_list(node);
		do {
			/* Turn : into \0 */
			val_p = strchr(val, ':');
			if (val_p != NULL) *val_p = 0;

			/* Add this string to the list if not zero-length */
			if (*val != 0) add_string_node(&node, val);

			/* Skip to the next character in the input */
			val = val_p + 1;
		} while (val_p != NULL);
		break;
	} case int_type:
		*(int*)config_data[i].var = atoi(val);
		break;
	case bool_type:
		if (val == NULL || strcasecmp(val, "yes") == 0 ||
				strcasecmp(val, "true") == 0)
			*(bool*)config_data[i].var = true;
		else
			*(bool*)config_data[i].var = false;
	}
}

/* Remove whitespace from the beginning and end of buf */
static void remove_space(char* buf)
{
	/* Remove leading whitespace */
	for (; *buf != 0 && isspace(*buf); buf++)
		strcpy(buf, buf + 1);
	
	/* Remove trailing whitespace */
	for (buf = strchr(buf, 0) - 1; isspace(*buf); buf--);
	buf[1] = 0;
}

/* Remove all quote characters from buf */
static void remove_quotes(char* buf)
{
	for (; *buf != 0; buf++)
		if (*buf == '"')
			strcpy(buf, buf + 1);
}

/* Zero the first # not between quotes */
static void remove_comment(char* buf)
{
	bool in_quotes = false;
	for (; *buf != 0; buf++)
		switch (*buf) {
		case '"':
			in_quotes = !in_quotes;
			break;
		case '#':
			if (!in_quotes) {
				*buf = 0;
				return;
			}
		}
}
