#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "install-log.h"

static void edit_file(char* filename);

void edit_database()
{
	char* filename = safe_sprintf(NULL, NULL, "%s/%s", logdir, package);
	collapse(filename, '/');
	edit_file(filename);
	free(filename);
}

static void edit_file(char* filename)
{
	char* command = safe_sprintf(NULL, NULL, "%s %s", editor, filename);
	switch (system(command)) {
	case -1:
	case 127:
		alert("system(%s): %s\n", command, strerror(errno));
	}
	free(command);
}
