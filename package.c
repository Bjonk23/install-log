#include "install-log.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define TAR	1
#define ZIP	2
#define RAR	3
#define PATCH	4
#define DIFF	5

#define GZ	6
#define BZ2	7
#define Z	8

static char* get_extension(char* pointer);
static char* get_version(char* pointer);
static char* get_name(const char* start, const char* end);

typedef struct PackageInfo {
	char* filename; /* man-1.5i2.tar.bz2 | vim-5.7-5.8-rt.diff.bz2 */
	char* name;	/* man               | vim                     */
	char* part;	/*                   | rt                      */
			/*                   |                         */
	int major;	/* 1                 | 5                       */
	int minor;	/* 5                 | 7                       */
	char* extra;	/* i2                |                         */
			/*                   |                         */
	int to_major;   /*                   | 5                       */
	int to_minor;   /*                   | 8                       */
	char* to_extra; /*                   |                         */
			/*                   |                         */
	int type;	/* TAR               | DIFF                    */
	int comp;	/* BZ2               | BZ2                     */
} PackageInfo;

void get_package_info(const char* file, char** name, char** version,
		char** extension)
{
	char* p;
	char* filename = (char*)malloc(strlen(file) + 1);
	
	strcpy(filename, file);
	p = filename;

	for (; *p != 0; p++);	/* find end */
	
	*extension = get_extension(p);
	*version = get_version(p);
	*name = get_name(filename, p);
}

char* get_extension(char* p)
{
	int length;
	char* extension;
	char* end = p;

	for (; *p != 't'; p--);
	length = end - p;

	extension = (char*)malloc(length + 1);
	strcpy(extension, p);

	*(--p) = 0;
	return extension;
}

char* get_version(char* p)
{
	int length;
	char* version;
	char* end = p;
	
	for (; *p != '-'; p--);
	length = end - (p++);
	
	version = (char*)malloc(length);
	strcpy(version, p);

	*(--p) = 0;
	return version;
}

char* get_name(const char* start, const char* end)
{
	int length = end - start;
	char* name = (char*)malloc(length);
	
	strncpy(name, start, length);
	*(name + length) = 0;

	return name;
}
