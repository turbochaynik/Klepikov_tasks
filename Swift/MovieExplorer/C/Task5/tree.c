#include <stdlib.h>
#include <setjmp.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include "list.h"
#include "tree.h"
#include "error.h"

int get_tree_depth(tree_node *root) {
    if (root == NULL) {
        return 0;
    }
    int pipe_depth = get_tree_depth(root->pipe);
    int subcmd_depth = get_tree_depth(root->psubcmd);
    int next_depth = get_tree_depth(root->next);
    int max_depth = pipe_depth;
    if (subcmd_depth > max_depth) {
        max_depth = subcmd_depth;
    }
    if (next_depth > max_depth) {
        max_depth = next_depth;
    }
    return max_depth + 1;
}

tree_node *init_tree(void) {
    tree_node *root = calloc(1, sizeof(tree_node));
    if (root == NULL) {
        return NULL;
    }
    root->argc = 0;
    root->argv = calloc(1, sizeof(char *));
    if (root->argv == NULL) {
        free(root);
        return NULL;
    }
    root->infile = NULL;
    root->outfile = NULL;
    root->append = 0;
    root->backgrnd = 0;
    root->psubcmd = NULL;
    root->pipe = NULL;
    root->next = NULL;
    root->type = NXT;
    return root;
}

void print_tree(tree_node *root) {
    if (root == NULL) {
        printf("\nEmpty tree\n");
        return;
    }

    printf("\nNODE:\n");
    printf("argc: %d\n", root->argc);

    printf("argv: ");
    for (int i = 0; i < root->argc; i++) {
        printf("%s ", root->argv[i]);
    }
    printf("\n");

    printf("infile: %s\n", root->infile ? root->infile : "NULL");
    printf("outfile: %s\n", root->outfile ? root->outfile : "NULL");
    printf("append: %d\n", root->append);
    printf("background: %d\n", root->backgrnd);
    printf("next type: %d\n", root->type);

    if (root->pipe) {
        printf("\nPIPE TO:\n");
        print_tree(root->pipe);
    }

    if (root->psubcmd) {
        printf("\nSUBSHELL:\n");
        print_tree(root->psubcmd);
    }

    if (root->next) {
        printf("\nNEXT COMMAND:\n");
        print_tree(root->next);
    }
}

void delete_tree(tree_node *root) {
    if (root == NULL)
        return;

    delete_tree(root->psubcmd);  
    delete_tree(root->pipe);    
    delete_tree(root->next);   
    if (root->argv != NULL) {
        for (int i = 0; root->argv[i] != NULL; i++) {
            free(root->argv[i]);
        }
        free(root->argv);
    }
    if (root->infile != NULL) {
        free(root->infile);
    }
    if (root->outfile != NULL) {
        free(root->outfile);
    }
    free(root);
}

void add_argv(tree_node *root, char *arg) {
    int num = ++root->argc;
    root->argv = realloc(root->argv, (num + 1)*sizeof(char *));
    root->argv[num - 1] = arg;
    root->argv[num] = NULL;
}

int is_op(Node *list) {
    if ((!strcmp(list->data, ">")) || (!strcmp(list->data, ">>")) ||
        (!strcmp(list->data, "&")) || (!strcmp(list->data, "|")) ||
        (!strcmp(list->data, "\"")) || (!strcmp(list->data, "<")) ||
        (!strcmp(list->data, "(")) || (!strcmp(list->data, ")")) ||
        (!strcmp(list->data, "||")) || (!strcmp(list->data, "&&")))
        return 0;

    return 1;
}

tree_node *build_tree(Node *list) {
    if (list == NULL) {
        return NULL;
    }
    Node *p = list;

    tree_node *root = init_tree();
    int tree_finished = 0;

    for (; p != NULL; p = p->next) {
        if (!strcmp(p->data, ">")) {
            if(root->outfile != NULL) {
                printf("my_shell: error two output files\n");
                delete_tree(root);
                return NULL;
            }
            root->outfile = strdup(p->next->data);
            p = p->next;
        }
        else if (!strcmp(p->data, ">>")) {
            if(root->outfile != NULL) {
                printf("my_shell: error two output files\n");
                delete_tree(root);
                return NULL;
            }
            root->outfile = strdup(p->next->data);
            root->append = 1;
            p = p->next;
        }
        else if (!strcmp(p->data, "<")) {
            if(root->infile != NULL) {
                printf("my_shell: error: two input files\n");
                delete_tree(root);
                return NULL;
            }
            root->infile = strdup(p->next->data);
            p = p->next;
        }
        else if (!strcmp(p->data, ";")) {
            root->type = NXT;
            root->next = build_tree(p->next);
            break;
        }
        else if (!strcmp(p->data, "&&")) {
            root->type = AND;
            root->next = build_tree(p->next);
            break;
        }
        else if (!strcmp(p->data, "||")) {
            root->type = OR;
            root->next = build_tree(p->next);
            break;
        }
        else if (!strcmp(p->data, "&")) {
            root->backgrnd = 1;
            root->next = build_tree(p->next);
            break;
        }
        else if (!strcmp(p->data, "|")) {
            root->pipe = build_tree(p->next);
            break;
        }
        else if (!strcmp(p->data, "(")) {
            if (root->argc != 0) {
                printf("my_shell: nexpected argument before '('\n");
                delete_tree(root);
            }
            root->psubcmd = build_tree(p->next);
        }
        else if (!strcmp(p->data, ")")) {
            if (!tree_finished) {
                delete_tree(root);
                return NULL;
            }
            else {
                return root;
            }
        }
        else {
            char *arg = calloc(1, strlen(p->data) + 1);
            strcpy(arg, p->data);
            add_argv(root, arg);
        }
        tree_finished = 1;
    }

    if(!tree_finished) {
        delete_tree(root);
        return NULL;
    }
    return root;
}

int set_inp_outp(tree_node *root) {
    int fd;
    if (root->infile != NULL) {
        if((fd = open(root->infile, O_RDONLY)) == -1) {
            perror("my_shell");
            return 1;
        }
        if (dup2(fd, 0) == -1) {
            perror("my_shell");
            return 1;
        }
    }

    if (root->outfile != NULL) {
        if (root->append) {
            if((fd = open(root->outfile, O_WRONLY | O_CREAT | O_APPEND, 0666)) == -1) {
                perror("my_shell");
                return 1;
            }
        }
        else {
            if((fd = open(root->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0666)) == -1) {
                perror("my_shell");
                return 1;
            }
        }

        if (dup2(fd, 1) == -1) {
            perror("my_shell");
            return 1;
        }
    }
    return 0;
}

tree_node *rewind_pipe_subshell(tree_node *root) {
    if ((root->pipe == NULL) && (root->psubcmd == NULL))
        return root;

    if(root->pipe != NULL)
        return rewind_pipe_subshell(root->pipe);

    if(root->psubcmd != NULL)
        return rewind_pipe_subshell(root->psubcmd);
    return NULL;
}

int run_shell_or_subshell(tree_node *root) {
    int returned_value = 0;

    if (root->argv[0] == NULL) {
        returned_value = tree_handler(root->psubcmd);
    }
    else {
        if (strcmp(root->argv[0], "false") == 0) {
            exit(1);
        }
        if (strcmp(root->argv[0], "true") == 0) {
            exit(0);
        }
        if (execvp(root->argv[0], root->argv) == -1) {
            printf("my_shell: unknown command\n");
            return -1;
        }
    }
    return returned_value;
}

int pipe_handler(tree_node *root) {
   pid_t pid;
   int status;
   int prev_pipe[2] = {-1, -1};
   int cur_pipe[2]; 
   int first = 1;   

   int orig_stdin = dup(0);

   while (root != NULL) {
       if (root->pipe != NULL) {
           if (pipe(cur_pipe) < 0) {
               perror("pipe");
               return -1;
           }
       }

       pid = fork();
       if (pid < 0) {
           perror("fork");
           return -1;
       }

       if (pid == 0) { 
           signal(SIGINT, SIG_DFL);

           if (!first) {
               if (dup2(prev_pipe[0], 0) < 0) {
                   perror("dup2");
                   exit(1);
               }
               close(prev_pipe[0]);
               close(prev_pipe[1]);
           }

           if (root->pipe != NULL) {
               if (dup2(cur_pipe[1], 1) < 0) {
                   perror("dup2");
                   exit(1);
               }
               close(cur_pipe[0]);
               close(cur_pipe[1]);
           }

           if (set_inp_outp(root) != 0) {
               exit(1);
           }

           if (run_shell_or_subshell(root) == -1) {
               exit(1);
           }

           exit(0);
       }

       if (!first) {
           close(prev_pipe[0]);
           close(prev_pipe[1]);
       }

       if (root->pipe != NULL) {
           prev_pipe[0] = cur_pipe[0];
           prev_pipe[1] = cur_pipe[1];
       }

       first = 0;
       root = root->pipe;
   }

   while ((pid = wait(&status)) > 0);

   dup2(orig_stdin, 0);
   close(orig_stdin);

   return status;
}


/*
int pipe_handler(tree_node *root) {
    pid_t pid;
    int status;
    int prev_pipe[2] = {-1, -1}; // для хранения предыдущего пайпа
    int cur_pipe[2];  // текущий пайп
    int first = 1;    // флаг первой команды
    int pipeline_index = 1; // Индекс текущего конвейера (начинаем с 1)

    // Обрабатываем каждую группу команд (конвейеры), разделенные `;`
    while (root != NULL) {
        // Пропускаем нечетные конвейеры
        if (pipeline_index % 2 != 0) {
            // Переходим к следующему конвейеру
            while (root != NULL && root->next != NULL) {
                root = root->next; // Пропускаем команды текущего конвейера
            }
            pipeline_index++;
            root = root != NULL ? root->next : NULL; // Переходим к следующему конвейеру
            continue;
        }

        // Выполняем четный конвейер
        first = 1; // Сбрасываем флаг для нового конвейера
        prev_pipe[0] = -1;
        prev_pipe[1] = -1;

        while (root != NULL && root->pipe != NULL) {
            if (pipe(cur_pipe) < 0) {
                perror("pipe");
                return -1;
            }

            pid = fork();
            if (pid < 0) {
                perror("fork");
                return -1;
            }

            if (pid == 0) { // Дочерний процесс
                signal(SIGINT, SIG_DFL);

                // Если это не первая команда, читаем из предыдущего пайпа
                if (!first) {
                    if (dup2(prev_pipe[0], 0) < 0) {
                        perror("dup2");
                        exit(1);
                    }
                    close(prev_pipe[0]);
                    close(prev_pipe[1]);
                }

                // Если есть следующая команда, пишем в текущий пайп
                if (root->pipe != NULL) {
                    if (dup2(cur_pipe[1], 1) < 0) {
                        perror("dup2");
                        exit(1);
                    }
                    close(cur_pipe[0]);
                    close(cur_pipe[1]);
                }

                // Обрабатываем перенаправления ввода/вывода
                if (set_inp_outp(root) != 0) {
                    exit(1);
                }

                // Выполняем команду
                if (run_shell_or_subshell(root) == -1) {
                    exit(1);
                }

                exit(0);
            }

            // Родительский процесс
            if (!first) {
                close(prev_pipe[0]);
                close(prev_pipe[1]);
            }

            if (root->pipe != NULL) {
                prev_pipe[0] = cur_pipe[0];
                prev_pipe[1] = cur_pipe[1];
            }

            first = 0;
            root = root->pipe;
        }

        // Ждем завершения всех процессов текущего конвейера
        while ((pid = wait(&status)) > 0);

        // Переходим к следующему конвейеру
        pipeline_index++;
        root = root != NULL ? root->next : NULL;
    }

    return status;
}
*/


int counter = 0;

int tree_handler(tree_node *root) {
    if (root == NULL)
        return 0;
    int status=1;
//int k = 0;

    if (root->pipe != NULL) {
        //if (k%2 == 0) {
	    int in = dup(0);
        status = pipe_handler(root);
	//k+=1;
        dup2(in, 0);
        close(in);
        return status;  
    }
	//return status;



    if ((root->argv[0] != NULL) && (strcmp(root->argv[0], "cd") == 0)) {
    const char *path;
    if (root->argv[1] == NULL) {
        path = getenv("HOME");
        if (path == NULL) {
            fprintf(stderr, "my_shell: HOME not set\n");
            return 1;
        }
    } else {
        path = root->argv[1];
    }

    if (chdir(path) != 0) {
        perror("my_shell: cd");
        return 1;
    }
    }
    else {
        pid_t child_pid;

        if ((child_pid = fork()) == 0) {
            if (root->backgrnd) {
                signal(SIGINT, SIG_IGN);
                if (root->infile == NULL) {
                    int fd = open("/dev/null", O_RDONLY);
                    if (fd != -1) {
                        dup2(fd, 0);
                        close(fd);
                    }
                }
            } else {
                signal(SIGINT, SIG_DFL);
            }

            int err = set_inp_outp(root);
            if (err)
                exit(1);

            if (root->argv[0] == NULL) {
                if (root->psubcmd != NULL) {
                    status = tree_handler(root->psubcmd);
                    exit(status);
                }
            } else {
                if (strcmp(root->argv[0], "false") == 0) {
                    exit(1);
                }
                if (strcmp(root->argv[0], "true") == 0) {
                    exit(0);
                }
                if (execvp(root->argv[0], root->argv) == -1) {
                    printf("my_shell: unknown command\n");
                    exit(1);
                }
            }
            exit(0);
        }
        else if (child_pid == -1) {
            perror("fork failed");
            return -1;
        }

        if (!root->backgrnd) {
            if (waitpid(child_pid, &status, 0) > 0) {
                status = WEXITSTATUS(status);
            }
        } else {
            printf("[started]: %d\n", child_pid);
        }
    }

    if (root->next != NULL) {
        if ((root->type == NXT) ||
            ((root->type == AND) && (status == 0)) ||
            ((root->type == OR) && (status != 0))) {
            status = tree_handler(root->next);
        }
    }

    return status;
}
int how_much_cmd(tree_node *root) {
    int sum = 0;

    if ((root->pipe == NULL) && (root->next == NULL))
        return 1;
    else if (root->pipe != NULL) {
        sum = 1 + how_much_cmd(root->pipe);
    }
    else if (root->next != NULL) {
        sum = 1 + how_much_cmd(root->next);
    }

    return sum;
}


