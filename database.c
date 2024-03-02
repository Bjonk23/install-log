#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "install-log.h"

static void process_prev_file(List* node, va_list ap);

/* Get data from existing database */
void read_db(void)
{
	List* node = &prev_files;
	size_t buf_cap = 64;
	struct stat statbuf;
	char* buf;
	FILE* db_file;
	char* db_filename = safe_sprintf(NULL, NULL, "%s/%s", logdir, package);

	/* See if file exists */
	if (stat(db_filename, &statbuf) == -1) {
		free(db_filename);
		return;
	}
	
	if (!force) fail("\"%s\" has already been logged; use --force to "
			"update it\n", package);

	report(6, "Reading existing database\n");
	
	db_file = fopen(db_filename, "r");
	if (db_file == NULL) fail("fopen: %s", strerror(errno));

	buf = malloc(buf_cap);

	/* Read each line into prev_files */
	while (true) {
		getline(&buf, &buf_cap, db_file);
		if (feof(db_file)) break;

		if (buf[0] == '#') continue;	/* Discard old error tokens */
		*strchr(buf, '\n') = 0;		/* Strip ending newline */

		add_string_node(&node, buf);
	}
	
	free(buf);
	fclose(db_file);
	free(db_filename);
}

/* Commit file lists to the database */
void write_db(void)
{
	FILE* db_file;
	char* db_filename = safe_sprintf(NULL, NULL, "%s/%s", logdir, package);
	
	report(6, "Writing database\n");

	db_file = fopen(db_filename, "w");
	if (db_file == NULL) fail("fopen: %s", strerror(errno));

	fprintf_list(db_file, &del_files, "# Del # %!s\n");
	fprintf_list(db_file, &old_files, "# Old # %!s\n");
	fprintf_list(db_file, &new_files, "%!s\n");

	fclose(db_file);
	db_file = NULL;
	free(db_filename);
}

/* Detect old files and deleted files */
void find_old_and_del_files(void)
{
	char* fullname = NULL;
	int fullname_cap = 0;
	
	report(6, "Scanning for old and deleted files\n");
	
	clear_list(&old_files);
	clear_list(&del_files);

	proc_list_va(&prev_files, process_prev_file, &fullname, &fullname_cap);

	if (fullname != NULL) free(fullname);
}

/* List all the old and deleted files to the screen */
void report_old_and_del_files(void)
{	
	char* db_filename;

	/* Check to see if there are any */
	if (old_files.next == NULL && del_files.next == NULL) return;

	/* List 'em */
	alert("Stale and/or deleted files detected\n");
	if (old_files.next != NULL) {
		printf("Old files:\n");
		fprintf_list(stdout, &old_files, "  %!s\n");
	}
	if (del_files.next != NULL) {
		printf("Deleted files:\n");
		fprintf_list(stdout, &del_files, "  %!s\n");
	}
	db_filename = safe_sprintf(NULL, NULL, "%s/%s", logdir, package);
	collapse(db_filename, '/');
	printf("The complete list is in %s\n", db_filename);
	free(db_filename);
}

/* Helper */
static void process_prev_file(List* node, va_list ap)
{
	char** fullname = va_arg(ap, char**);
	int* fullname_cap = va_arg(ap, int*);
	struct stat statbuf;
	safe_sprintf(fullname, fullname_cap, "%s/%s", root, node->data);
	
	if (!list_has_string(&new_files, (char*)node->data)) {
		if (stat(*fullname, &statbuf) == 0) {	/* It exists */
			report(5, "Found old file: %s\n", (char*)node->data);
			insert_string_node(&old_files, (char*)node->data);
		} else {				/* It's deleted */
			report(5, "Found deleted file: %s\n",
					(char*)node->data);
			insert_string_node(&del_files, (char*)node->data);
		}
	}
}

