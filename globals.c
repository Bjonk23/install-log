#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "install-log.h"

static char* getenv_default(char* var, char* def);

char* program_name;

bool force;
bool edit;
bool quiet;

List include;
List exclude;
char* package;
char* root;
char* logdir;
char* editor;
int verbosity;

List new_files;
List old_files;
List del_files;
List prev_files;

time_t timestamp;

static const char* default_include[] = {
	"/bin",
	"/lib",
	"/opt",
	"/sbin",
	"/usr",
	NULL
};
static const char* default_exclude[] = {
	"/lib/modules",
	"/usr/src",
	"/usr/local/src",
	NULL
};

/* Set global values to defaults and to environment variable settings */
void init_globals(char** argv)
{
	program_name = argv[0];

	force = false;
	edit = false;
	quiet = false;	

	make_string_list(&include, default_include);
	make_string_list(&exclude, default_exclude);

	package = NULL;
	root = getenv_default("LFS", "/");
	logdir = strdup("/var/install-logs");
	editor = getenv_default("EDITOR", "/bin/vi");
	verbosity = 0;

	atexit(free_globals);
}

/* Free up everything */
void free_globals(void)
{
	clear_list(&include);
	clear_list(&exclude);
	if (package != NULL) free(package);
	free(root);
	free(logdir);
	free(editor);
	
	clear_list(&new_files);
	clear_list(&old_files);
	clear_list(&del_files);
	clear_list(&prev_files);
}

/* Return either the environment setting for var or, failing that, def */
static char* getenv_default(char* var, char* def)
{
	char* value = getenv(var);
	if (value == NULL) return strdup(def);
	return strdup(value);
}
