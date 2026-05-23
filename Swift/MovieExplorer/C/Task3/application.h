#ifndef APP_H
#define APP_H

typedef struct 
{
	char **words;
	size_t size;
	size_t capacity;
} Array;

void sort(Array *list);
void sort2(Array *list);
void add(Array *list, char *word);
void free_list(Array *list);
void print_list(Array *list);
// void make_list(const char *line, Node *list);

#endif
