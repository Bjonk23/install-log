#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <time.h>
#include <utime.h>
#include <unistd.h>
#include <errno.h>
#include <string.h> 
#include "install-log.h"

typedef struct stat Stat;

/* Get the time of the timestamp file */
void get_timestamp(void)
{
	Stat status;
	
	char* filename = safe_sprintf(NULL, NULL, "%s/.timestamp", logdir);
	if (stat(filename, &status) == -1)
		fail("Run 'touch %s' and install something first\n", filename);
	free(filename);

	timestamp = status.st_mtime;
}

/* Make timestamp reflect the current time */
void touch_timestamp(void)
{
	char* filename = safe_sprintf(NULL, NULL, "%s/.timestamp", logdir);
	if (utime(filename, NULL) == -1)
		alert("timestamp(%s/.timestamp): %s\n", logdir,
				strerror(errno));
	free(filename);
}
