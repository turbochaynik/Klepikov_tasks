#ifndef LIST_H
#define LIST_H

#define BUFF_SIZE 150

typedef char * Data;
typedef struct Node Node;

struct Node {
    Data data;
    Node *next;
};

Data pop(Node **plist);
void print_list(Node *list);
int is_empty(Node * list);
void push_frwrd(Node **plist, Data d);
void push_end(Node **plist, Data d);
int get_len(Node *list);
void detele_list(Node **plist);
Node *get_node(Node *list, int index);
void s_sort(Node *list, int n);
void print_size(Node *list);
int count_word(Node *list, char *word);
void most_recent_word(Node *list);
Node *get_list(void);
void replase(Node *list);
Node *get_cls_brckt(Node *list);
void dollar_handler(Node *list);
int is_special(Node *list);

#endif