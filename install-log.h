#ifndef __install_log_h__
#define __install_log_h__

#include <time.h>
#include <stdarg.h>
#include <stdio.h>

typedef int bool;

#ifndef false
#define false 0
#endif

#ifndef true
#define true -1
#endif

typedef struct List {
	void* data;
	struct List* next;
} List;

/* globals.c */
extern char* program_name;
extern bool force;
extern bool edit;
extern bool quiet;
extern char* package;
extern List include;
extern List exclude;
extern char* root;
extern char* logdir;
extern char* editor;
extern int verbosity;
extern List new_files;
extern List old_files;
extern List del_files;
extern List prev_files;
extern time_t timestamp;
extern void init_globals(char** argv);
extern void free_globals(void);

/* install-log.c */
extern void usage(int status);
extern int main(int argc, char** argv);

/* config.c */
extern void get_config(int argc, char** argv);

/* logger.c */
extern void find_all(void);

/* timestamp.c */
extern void get_timestamp(void);
extern void touch_timestamp(void);

/* list.c */
extern void add_node(List** node, void* data);
extern void add_string_node(List** node, const char* string);
extern void insert_string_node(List* list, const char* string);
extern void make_string_list(List* list, const char** string);
extern void proc_list(List* list, void (*func)(List*));
extern void proc_list_va(List* list, void (*func)(List*, va_list), ...);
extern void clear_list(List* list);
extern bool list_has_string(const List* list, const char* string);

/* util.c */
extern char* safe_sprintf(char** to, int* to_cap, char* fmt, ...);
extern void replace(char** context, int* context_cap, char* from, char* to);
extern void collapse(char* string, char c);
extern void alert(const char* format, ...);
extern void fail(const char* format, ...);
extern void report(int level, const char* format, ...);

/* database.c */
extern void read_db(void);
extern void write_db(void);
extern void find_old_and_del_files(void);
extern void report_old_and_del_files(void);

/* editor.c */
extern void edit_database();
extern void fprintf_list(FILE* file, List*, char* fmt, ...);

/* package.c */
extern void get_package_info(const char* filename, char** name, 
		char** version, char** extension);

#endif
