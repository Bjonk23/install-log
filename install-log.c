#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <mcheck.h>
#include <string.h>

#include "install-log.h"

/* Prints usage information and exits */
void usage(int status)
{
	if (status != 0)
		fprintf(stderr, "Try `%s --help' for more information.\n",
				program_name);
	else
		printf(
"Usage: %s <package> [OPTIONS]\n"
"Manage package installation history database\n"
"\n"
"   -e, --edit                  edit log\n"
"   -E, --editor=CMD            use CMD with --edit\n"
"   -f, --force                 replace existing installation log\n"
"   -h, --help                  this truck\n"
"   -i, --include=DIR1:DIR2:... scan these directories\n"
"   -l, --logdir=DIR            keep logs in DIR\n"
"   -q, --quiet                 do not output messages (except errors)\n"
"   -r, --root=DIR              use DIR as root\n"
"   -v, --verbosity=LEVEL       report progress with given level of detail\n"
"   -x, --exclude=DIR1:DIR2:... do not scan these directories\n",
		program_name);
	exit(status);
}

/* So ubiquitous, it needs no comment */
int main(int argc, char** argv)
{
	char* name;
	char* version;
	char* extension;
	
	get_config(argc, argv);

	get_package_info(package, &name, &version, &extension);
	printf("name=%s\nversion=%s\nextension=%s\n", name, version, extension);
	
/*	get_timestamp();
	read_db();
	find_all();
	find_old_and_del_files();
	write_db();
	touch_timestamp();
	if (edit)
		edit_database();
	else if (!quiet)
		report_old_and_del_files();*/

	return EXIT_SUCCESS;
}
