#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>
#include "list.h"

Data pop(Node **plist) {
    Node *p = *plist;
    Data res = p->data;
    *plist = p->next;
    free(p);
    return res;
}

void print_size(Node *list) {
    int size = 0;
    for(Node *p = list ; p!=NULL ; p = p->next) {
        ++size;
    }
    printf("List length: %d\n",size);
}

void print_list(Node *list) {
    printf("List:\n");
    for(Node *p = list ; p!=NULL ; p = p->next) {
        printf("%s ",p->data);
        printf("\n");
    }
}

int is_empty(Node * list) {
    return list == NULL;
}

void push_frwrd(Node **plist, Data d) {
    Node *p = calloc(1,sizeof(Node));
    if (d!= NULL) {
        p->data = strdup(d);
        p->next = *plist;
        *plist = p;
    }
}

void push_end(Node **plist, Data d) {
    Node *p = calloc(1,sizeof(Node));
    Node *q = *plist;
    p->data = strdup(d);
    p->next = NULL;

    if (*plist == NULL) {
        *plist = p;
    } else {
        while ((q->next)!=NULL) {
            q = q->next;
        }
        (q->next) = p;
    }
}

int get_len(Node *list) {
    int size = 0;
    for(Node *p = list ; p!=NULL ; p = p->next) {
        ++size;
    }
    return size;
}

void detele_list(Node **plist) {
    while (!is_empty(*plist)) {
        free(pop(plist));
    }
}

Node *get_node(Node *list, int index) {
    int i = 0;
    for(Node *p = list ; (i <= index) && p!=NULL ; p = p->next, ++i) {
        if (i == index)
            return p;
    }
    return NULL;
}

void s_sort(Node *list, int n) {
    if (list == NULL || n == 1)
        return;

    int min_index,i,j;
    //char *s_exch = NULL;
    Data data_exch = NULL;

    i = 0;
    while (i < n - 1) {
        min_index = i;
        j = i+1;
        while (j<n) {
            if (strcmp(get_node(list,j)->data, get_node(list,min_index)->data) < 0) {
                min_index = j;
            }
            ++j;
        }
        if (strcmp(get_node(list,i)->data, get_node(list,min_index)->data) > 0) {
            data_exch = get_node(list,min_index)->data;
            get_node(list,min_index)->data = get_node(list,i)->data;
            get_node(list,i)->data = data_exch;
        }
        ++i;
    }
}

int count_word(Node *list, char *word) {
    Node *p = list;
    int res = 0;
    while (p != NULL) {
        if (!strcmp(p->data, word))
            ++res;
        p = p->next;
    }
    return res;
}

void most_recent_word(Node *list) {
    int count = 0, max = 0;
    Node *most_recent_node;
    Node *p = list;

    while (p != NULL) {
        if ((count = count_word(list, p->data)) > max) {
            max = count;
            most_recent_node = p;
        }
        p = p->next;
    }

    printf("Most recent word: %s\n", most_recent_node->data);
    printf("Amount: %d\n", max);
}

void replase(Node *list) {
    Node *p = list;
    char home[] = " /Users/glebklepikov";
    char shell[] = "/Users/glebklepikov/2024_Klepikov215/Tasks/Task5";
    char who[] = "glebklepikov";

    while (p != NULL) {
        if (!strcmp(p->data, "HOME")) {
            free(p->data);
            p->data = calloc(1, sizeof(home));
            memmove(p->data, home, strlen(home));
        }
        else if (!strcmp(p->data, "SHELL")) {
            free(p->data);
            p->data = calloc(1, sizeof(shell));
            memmove(p->data, shell, strlen(shell));
        }
        else if (!strcmp(p->data, "USER")) {
            free(p->data);
            p->data = calloc(1, sizeof(who));
            memmove(p->data, who, strlen(who));
        }
        p = p->next;
    }
}

Node *get_cls_brckt(Node *list) {
    Node *p = list;
    while ((p != NULL) && (strcmp(p->data, ")"))) {
        p = p->next;
    }
    return p->next;
}

int is_special(Node *list) {
    if (list == NULL)
        return 0;

    char c = list->data[0];
    switch (c) {
    case '|':
    case '\"':
    case '\'':
    case '&':
    case ';':
    case '>':
    case '<':
    case '(':
    case ')':
    case '#':
    case '\\':
        return 1;
    default:
        return 0;
    }
}

Node *get_list(void) {
    Node *list = NULL;
    char *s = malloc(BUFF_SIZE);
    if (s == NULL) {
        return NULL;
    }

    char *word = malloc(BUFF_SIZE);
    if (word == NULL) {
        free(s);
        return NULL;
    }

    char c = 32;
    int q_flag = 0;
    int q_one_flag = 0;
    int comm_flag = 1;
    int break_flag = 1;

    int i = 0;
    int i_word_start = 0;
    int i_start = 0, i_end = 0;

    if (fgets(s, BUFF_SIZE, stdin) == NULL) {
        free(word);
        free(s);
        return NULL;
    }

    size_t len = strlen(s);
    char *p = s + i_word_start;

    for (; comm_flag && break_flag && (i < len); i++) {
        c = *p;
        switch (c) {
        case '\"':
            if (!q_flag)
                ++q_flag;

            if (i_start != i_end) {
                if (q_flag == 1) {
                    if (i_end - i_start >= BUFF_SIZE) {
                        free(word);
                        free(s);
                        return NULL;
                    }
                    memcpy(word, s + i_start, i_end - i_start);
                    word[i_end - i_start] = '\0';
                    ++q_flag;
                } else {
                    if (i_end - i_start >= BUFF_SIZE) {
                        free(word);
                        free(s);
                        return NULL;
                    }
                    memcpy(word, s + i_start, i_end - i_start);
                    word[i_end - i_start] = '\0';
                }
                Node *new_node = malloc(sizeof(Node));
                if (new_node == NULL) {
                    free(word);
                    free(s);
                    return NULL;
                }
                new_node->data = strdup(word);
                if (new_node->data == NULL) {
                    free(new_node);
                    free(word);
                    free(s);
                    return NULL;
                }
                new_node->next = NULL;

                if (list == NULL) {
                    list = new_node;
                } else {
                    Node *current = list;
                    while (current->next != NULL) {
                        current = current->next;
                    }
                    current->next = new_node;
                }
            }
            if (q_flag == 2)
                q_flag = 0;

            i_start = i + 1;
            i_end = i + 1;
            i_word_start = i + 1;
            break;

        case '\'':
            // Аналогичная обработка как для двойных кавычек
            if (!q_one_flag)
                ++q_one_flag;

            if (i_start != i_end) {
                if (q_one_flag == 1) {
                    if (i_end - i_start >= BUFF_SIZE) {
                        free(word);
                        free(s);
                        return NULL;
                    }
                    memcpy(word, s + i_start, i_end - i_start);
                    word[i_end - i_start] = '\0';
                    ++q_one_flag;
                } else {
                    if (i_end - i_start >= BUFF_SIZE) {
                        free(word);
                        free(s);
                        return NULL;
                    }
                    memcpy(word, s + i_start, i_end - i_start);
                    word[i_end - i_start] = '\0';
                }
                Node *new_node = malloc(sizeof(Node));
                if (new_node == NULL) {
                    free(word);
                    free(s);
                    return NULL;
                }
                new_node->data = strdup(word);
                if (new_node->data == NULL) {
                    free(new_node);
                    free(word);
                    free(s);
                    return NULL;
                }
                new_node->next = NULL;

                if (list == NULL) {
                    list = new_node;
                } else {
                    Node *current = list;
                    while (current->next != NULL) {
                        current = current->next;
                    }
                    current->next = new_node;
                }
            }
            if (q_one_flag == 2)
                q_one_flag = 0;

            i_start = i + 1;
            i_end = i + 1;
            i_word_start = i + 1;
            break;

        case ' ':
        case '\t':
        case '\n':
        case '\v':
        case '\b':
        case '\r':
        case '\f':
            if (q_flag || q_one_flag) {
                i_end = i + 1;
                break;
            }

            if (i_start == i_end) {
                i_start = i + 1;
                i_end = i + 1;
                i_word_start = i + 1;
                break;
            }

            if (i_end - i_start >= BUFF_SIZE) {
                free(word);
                free(s);
                return NULL;
            }
            memcpy(word, s + i_start, i_end - i_start);
            word[i_end - i_start] = '\0';

            Node *new_node = malloc(sizeof(Node));
            if (new_node == NULL) {
                free(word);
                free(s);
                return NULL;
            }
            new_node->data = strdup(word);
            if (new_node->data == NULL) {
                free(new_node);
                free(word);
                free(s);
                return NULL;
            }
            new_node->next = NULL;

            if (list == NULL) {
                list = new_node;
            } else {
                Node *current = list;
                while (current->next != NULL) {
                    current = current->next;
                }
                current->next = new_node;
            }

            i_start = i + 1;
            i_end = i + 1;
            i_word_start = i + 1;
            break;

        case '|':
        case '&':
        case ';':
        case '>':
        case '<':
        case '(':
        case ')':
        case '#':
            if (q_flag || q_one_flag) {
                i_end = i + 1;
                break;
            }

            if (i_start != i_end) {
                if (i_end - i_start >= BUFF_SIZE) {
                    free(word);
                    free(s);
                    return NULL;
                }
                memcpy(word, s + i_start, i_end - i_start);
                word[i_end - i_start] = '\0';

                Node *new_node = malloc(sizeof(Node));
                if (new_node == NULL) {
                    free(word);
                    free(s);
                    return NULL;
                }
                new_node->data = strdup(word);
                if (new_node->data == NULL) {
                    free(new_node);
                    free(word);
                    free(s);
                    return NULL;
                }
                new_node->next = NULL;

                if (list == NULL) {
                    list = new_node;
                } else {
                    Node *current = list;
                    while (current->next != NULL) {
                        current = current->next;
                    }
                    current->next = new_node;
                }
            }

            if (c == '#') {
                comm_flag = 0;
                break;
            }

            if ((i + 1 < len) && (c == *(p+1))) {
                if (2 >= BUFF_SIZE) {
                    free(word);
                    free(s);
                    return NULL;
                }
                memcpy(word, s + i, 2);
                word[2] = '\0';

                Node *new_node = malloc(sizeof(Node));
                if (new_node == NULL) {
                    free(word);
                    free(s);
                    return NULL;
                }
                new_node->data = strdup(word);
                if (new_node->data == NULL) {
                    free(new_node);
                    free(word);
                    free(s);
                    return NULL;
                }
                new_node->next = NULL;

                if (list == NULL) {
                    list = new_node;
                } else {
                    Node *current = list;
                    while (current->next != NULL) {
                        current = current->next;
                    }
                    current->next = new_node;
                }

                i += 1;
                ++p;
            } else {
                if (1 >= BUFF_SIZE) {
                    free(word);
                    free(s);
                    return NULL;
                }
                memcpy(word, s + i, 1);
                word[1] = '\0';

                Node *new_node = malloc(sizeof(Node));
                if (new_node == NULL) {
                    free(word);
                    free(s);
                    return NULL;
                }
                new_node->data = strdup(word);
                if (new_node->data == NULL) {
                    free(new_node);
                    free(word);
                    free(s);
                    return NULL;
                }
                new_node->next = NULL;

                if (list == NULL) {
                    list = new_node;
                } else {
                    Node *current = list;
                    while (current->next != NULL) {
                        current = current->next;
                    }
                    current->next = new_node;
                }
            }

            i_start = i + 1;
            i_end = i + 1;
            i_word_start = i + 1;
            break;

        case '\\':
            // Обработка экранированных символов
            if (q_flag || q_one_flag) {
                ++i_end;
                break;
            }

            if ((i + 2 < len) && (*(p+1) == '\\') &&
                (*(p+2) == 'b' || *(p+2) == 'r' || *(p+2) == 'n' ||
                 *(p+2) == 't' || *(p+2) == 'f' || *(p+2) == 'v')) {

                if (2 >= BUFF_SIZE) {
                    free(word);
                    free(s);
                    return NULL;
                }
                memcpy(word, s + i + 1, 2);
                word[2] = '\0';

                Node *new_node = malloc(sizeof(Node));
                if (new_node == NULL) {
                    free(word);
                    free(s);
                    return NULL;
                }
                new_node->data = strdup(word);
                if (new_node->data == NULL) {
                    free(new_node);
                    free(word);
                    free(s);
                    return NULL;
                }
                new_node->next = NULL;

                if (list == NULL) {
                    list = new_node;
                } else {
                    Node *current = list;
                    while (current->next != NULL) {
                        current = current->next;
                    }
                    current->next = new_node;
                }

                i += 3;
                p += 3;
                i_start = i + 1;
                i_end = i + 1;
                i_word_start = i + 1;
            }
            else if ((i + 1 < len) && (*(p+1) == '\"' || *(p+1) == '\'')) {
                if (1 >= BUFF_SIZE) {
                    free(word);
                    free(s);
                    return NULL;
                }
                memcpy(word, s + i + 1, 1);
                word[1] = '\0';

                Node *new_node = malloc(sizeof(Node));
                if (new_node == NULL) {
                    free(word);
                    free(s);
                    return NULL;
                }
                new_node->data = strdup(word);
                if (new_node->data == NULL) {
                    free(new_node);
                    free(word);
                    free(s);
                    return NULL;
                }
                new_node->next = NULL;

                if (list == NULL) {
                    list = new_node;
                } else {
                    Node *current = list;
                    while (current->next != NULL) {
                        current = current->next;
                    }
                    current->next = new_node;
                }

                i += 2;
                p += 2;
                i_start = i + 1;
                i_end = i + 1;
                i_word_start = i + 1;
            }
            break;

        default:
            ++i_end;
            break;
        }
        ++p;
    }

    // Проверяем, если остался необработанный текст
    if (i_start != i_end && i_end - i_start < BUFF_SIZE) {
        memcpy(word, s + i_start, i_end - i_start);
        word[i_end - i_start] = '\0';

        Node *new_node = malloc(sizeof(Node));
        if (new_node == NULL) {
            free(word);
            free(s);
            return NULL;
        }
        new_node->data = strdup(word);
        if (new_node->data == NULL) {
            free(new_node);
            free(word);
            free(s);
            return NULL;
        }
        new_node->next = NULL;

        if (list == NULL) {
            list = new_node;
        } else {
            Node *current = list;
            while (current->next != NULL) {
                current = current->next;
            }
            current->next = new_node;
        }
    }

    free(word);
    free(s);
    return list;
}

void dollar_handler(Node *list) {
    Node *p = list;
    char *temp;

    while (p != NULL) {
        char *new_data = NULL;

        if (!strcmp(p->data, "$HOME")) {
            temp = getenv("HOME");
            if (temp) {
                new_data = realloc(p->data, strlen(temp) + 1);
                if (new_data) {
                    p->data = new_data;
                    strncpy(p->data, temp, strlen(temp) + 1);
                }
            }
        }
        else if (!strcmp(p->data, "$SHELL")) {
            temp = getenv("SHELL");
            if (temp) {
                new_data = realloc(p->data, strlen(temp) + 1);
                if (new_data) {
                    p->data = new_data;
                    strncpy(p->data, temp, strlen(temp) + 1);
                }
            }
        }
        else if (!strcmp(p->data, "$USER")) {
            temp = getenv("LOGNAME");
            if (temp) {
                new_data = realloc(p->data, strlen(temp) + 1);
                if (new_data) {
                    p->data = new_data;
                    strncpy(p->data, temp, strlen(temp) + 1);
                }
            }
        }
        else if (!strcmp(p->data, "$EUID")) {
            char euid_str[32];
            snprintf(euid_str, sizeof(euid_str), "%d", geteuid());
            new_data = realloc(p->data, strlen(euid_str) + 1);
            if (new_data) {
                p->data = new_data;
                strncpy(p->data, euid_str, strlen(euid_str) + 1);
            }
        }
        p = p->next;
    }
}
