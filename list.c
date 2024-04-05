#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "install-log.h"

static List *make_node(void *data);
static void free_node(List *node);
static void fprintf_node(List *node, va_list ap);

/* Linked lists begin with a head node containing no data */

/* Create a node and advance to it */
void add_node(List **node, void *data) {
  List *next = make_node(data);
  (*node)->next = next;
  *node = next;
}

/* Create a string node and advance to it */
void add_string_node(List **node, const char *string) {
  add_node(node, strdup(string));
}

/* Add a string node, using an insertion sort */
void insert_string_node(List *list, const char *string) {
  List *node = list;
  List *new_node = make_node(strdup(string));

  /* Scan for proper position */
  for (; node->next != NULL; node = node->next)
    if (strcmp((char *)node->next->data, string) > 0)
      break;

  /* Insert! */
  new_node->next = node->next;
  node->next = new_node;
}

/* Create a linked list of strings from a NULL-terminated array of strings */
void make_string_list(List *list, const char **string) {
  List *node;
  clear_list(list);
  node = list;
  for (; *string != NULL; string++)
    add_string_node(&node, *string);
}

/* Call func for each node in the list */
void proc_list(List *list, void (*func)(List *)) {
  list = list->next;
  while (list != NULL) {
    List *next = list->next;
    func(list);
    list = next;
  }
}

/* Call func for each node in the list, passing a va_list */
void proc_list_va(List *list, void (*func)(List *, va_list), ...) {
  va_list ap;
  list = list->next;
  while (list != NULL) {
    List *next = list->next;
    va_start(ap, func);
    func(list, ap);
    va_end(ap);
    list = next;
  }
}

/* fprintf to file every element of list using a format string
   Use %! instead of plain % to refer to node contents
   Example: %!s to print the node's data as a string
   (%! doesn't correspond to a parameter) */
void fprintf_list(FILE *file, List *list, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  proc_list_va(list, fprintf_node, file, fmt, ap);
  va_end(ap);
}

/* Remove all but the head node in the list */
void clear_list(List *list) {
  proc_list(list, free_node);
  list->next = NULL;
}

/* Check to see if the list contains the given string */
bool list_has_string(const List *list, const char *string) {
  while ((list = list->next) != NULL)
    if (strcmp((char *)list->data, string) == 0)
      return true;
  return false;
}

/* Create a new, isolated node with the given data pointer */
static List *make_node(void *data) {
  List *node = malloc(sizeof(List));
  node->data = data;
  node->next = NULL;
  return node;
}

/* Free the given node, deallocating data */
static void free_node(List *node) {
  free(node->data);
  free(node);
}

/* Print an individual node */
static void fprintf_node(List *node, va_list ap) {
  FILE *file = va_arg(ap, FILE *);
  char *fmt = va_arg(ap, char *);

  /* An individual %! token */
  int token_cap = 8;
  int token_len = 0;
  char *token = malloc(token_cap);
  /* The expanded version of one %! token */
  int exp_token_cap = 8;
  int exp_token_len = 0;
  char *exp_token = malloc(exp_token_cap);
  /* The format string with all %!'s expanded */
  char *fmt2 = strdup(fmt);
  int fmt2_len = strlen(fmt2);
  int fmt2_cap = strlen(fmt2);
  char *fmt2_p = fmt2;

  /* Expand all %! tokens */
  while ((fmt2_p = strstr(fmt2_p, "%!")) != NULL) {
    /* Extract %! token */
    token_len = strcspn(fmt2_p + 2, "diouxXeEfFgGaAcsCSpn%") + 3;
    if (token_len > token_cap) {
      token_cap = token_len + 8;
      token = realloc(token, token_cap);
    }
    strcpy(token, "%");
    strncpy(token + 1, fmt2_p + 2, token_len - 2);
    token[token_len - 1] = 0;

    /* Expand token and escape all % characters */
    safe_sprintf(&exp_token, &exp_token_cap, token, node->data);
    replace(&exp_token, &exp_token_cap, "%", "%%");
    exp_token_len = strlen(exp_token);

    /* Replace %! token with exp_token */
    fmt2_len += exp_token_len - token_len;
    if (fmt2_len > fmt2_cap) {
      int fmt2_idx = fmt2_p - fmt2;
      fmt2_cap = fmt2_len + 8;
      fmt2 = realloc(fmt2, fmt2_cap);
      fmt2_p = fmt2 + fmt2_idx;
    }
    memmove(fmt2_p + exp_token_len, fmt2_p + token_len,
            strlen(fmt2_p + token_len) + 1);
    memcpy(fmt2_p, exp_token, exp_token_len);
    fmt2_p += exp_token_len;
  }

  /* Finally, print */
  fprintf(file, fmt2, va_arg(ap, va_list));

  free(token);
  free(exp_token);
  free(fmt2);
}
