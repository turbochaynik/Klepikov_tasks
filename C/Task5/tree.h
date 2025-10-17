#ifndef TREE_H
#define TREE_H
#include <stdlib.h>
#include <setjmp.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "list.h"

enum type_of_next {
    NXT,
    AND,
    OR
};

typedef struct cmd_inf tree_node;

struct cmd_inf {
    int argc;
    char **argv;
    char *infile;
    char *outfile;
    int append;
    int backgrnd;
    struct cmd_inf *psubcmd;
    struct cmd_inf *pipe;
    struct cmd_inf *next;
    enum type_of_next type;
};
int get_tree_depth(tree_node *root);
void print_tree(tree_node *root);
tree_node *build_tree(Node *list);
void delete_tree(tree_node *root);
extern pid_t bckrd_pid;
extern int bckrd_flg;
tree_node *init_tree(void);
int is_op(Node *list);
void add_argv(tree_node *root, char *arg);
tree_node *rewind_pipe_subshell(tree_node *root);
int run_shell_or_subshell(tree_node *root);
int tree_handler(tree_node *root);
int how_much_cmd(tree_node *root);

#endif
