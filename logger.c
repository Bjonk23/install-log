#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <err.h>

#include "install-log.h"

typedef struct dirent Dirent;
typedef struct stat Stat;

static void find_newer_wrapper(List* node);
static void find_newer(const char* cwd);

/* Find all files newer than timestamp */
void find_all()
{
	clear_list(&new_files);
	proc_list(&include, find_newer_wrapper);
}

/* For use with proc_list */
static void find_newer_wrapper(List* node)
{
	report(1, "Scanning %s\n", (char*)node->data);
	find_newer((char*)node->data);
}

/* Recursively find all files newer than timestamp, starting in cwd */
static void find_newer(const char* cwd)
{
	char* name = NULL;
	int name_len;
	char* chroot_name = NULL;
	int chroot_name_len;
	DIR* dir;
	Dirent* d;
	Stat stat;

	/* Scan all entries in this directory */
	safe_sprintf(&chroot_name, &chroot_name_len, "%s/%s", root, cwd);

	if ((dir = opendir(chroot_name)) == NULL) {
		warn("Cannot scan %s\n", chroot_name);
		return;
	}

	while (d = readdir(dir))
	{
		/* Skip '.' and '..' */
		if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0)
			continue;

		/* Calculate the filename */
		safe_sprintf(&name, &name_len, "%s/%s", cwd, d->d_name);
		safe_sprintf(&chroot_name, &chroot_name_len, "%s/%s", root,
				name);
		
		/* Process this directory entry */
		if (lstat(chroot_name, &stat) == -1) {
			/* lstat failed, so skip file */
			alert("lstat(%s): %s\n", chroot_name,
					strerror(errno));
		} else if (list_has_string(&exclude, name)) {
			report(2, "Excluding %s\n", name);
		} else if (S_ISDIR(stat.st_mode)) {
			report(3, "Descending into %s\n", name);
			find_newer(name);
		} else if (timestamp < stat.st_mtime) {
			report(4, "Adding %s\n", name);
			insert_string_node(&new_files, name);
		}
	}
	closedir(dir);
	free(name);
	free(chroot_name);
}
