#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include "list.h"
#include "error.h"

int check_error(Node *list) {
    Node *p = list;

    if (list != NULL && is_special(list))
        return ERR_SPCL_SYM;

    int qtn_mrk_1 = 0;
    int qtn_mrk_2 = 0;
    while (p != NULL) {
        if(!strcmp(p->data, "(")) {
            return ERR_BRCKT;
        }

        if(!strcmp(p->data, ")")) {
            return ERR_BRCKT;
        }

        if((!strcmp(p->data, ";"))) {
            if(is_special(p->next))
                return ERR_SMCLN;
        }
        else if((!strcmp(p->data, ">"))) {
            if(is_special(p->next))
                return ERR_OUTPT;
        }
        else if((!strcmp(p->data, "<"))) {
            if(is_special(p->next))
                return ERR_INP;
        }
        else if((!strcmp(p->data, "&"))) {
            if(is_special(p->next))
                return ERR_APSND;
        }
        else if((!strcmp(p->data, "|"))) {
            if(is_special(p->next))
                return ERR_VRTCL_BR;
        }
        else if((!strcmp(p->data, ">>"))) {
            if(is_special(p->next))
                return ERR_DBL_OUTPT;
        }
        else if((!strcmp(p->data, "||"))) {
            if(is_special(p->next))
                return ERR_DBL_VRTCL_BR;
        }
        else if((!strcmp(p->data, "&&"))) {
            if(is_special(p->next))
                return ERR_DBL_APSND;
        }
        else if((!strcmp(p->data, "\'"))) {
            if(is_special(p->next))
                return ERR_QTN_MRK;
            ++qtn_mrk_2;
        }
        else if((!strcmp(p->data, "\""))) {
            if(is_special(p->next))
                return ERR_QTN_MRK;
            ++qtn_mrk_2;
        }
        p = p->next;
    }

    if (qtn_mrk_1 % 2 != 0) {
        return ERR_QTN_MRK;
    }

    if (qtn_mrk_2 % 2 != 0) {
        return ERR_QTN_MRK;
    }

    return 0;
}

int print_error(int err) {
    switch (err) {
    case ERR_BRCKT:
        printf("shell: error near ()\n");
        return 1;
        break;

    case ERR_QTN_MRK:
        printf("shell: error in quotation marks placement\n");
        return 1;
        break;

    case ERR_SMCLN:
        printf("shell: error near ;\n");
        return 1;
        break;

    case ERR_APSND:
        printf("shell: error near &\n");
        return 1;
        break;

    case ERR_DBL_APSND:
        printf("shell: error near &&\n");
        return 1;
        break;

    case ERR_OUTPT:
        printf("shell: error near >\n");
        return 1;
        break;

    case ERR_DBL_OUTPT:
        printf("shell: error near >>\n");
        return 1;
        break;

    case ERR_INP:
        printf("shell: error near <\n");
        return 1;
        break;

    case ERR_VRTCL_BR:
        printf("shell: error near |\n");
        return 1;
        break;

    case ERR_DBL_VRTCL_BR:
        printf("shell: error near ||\n");
        return 1;
        break;

    case ERR_SPCL_SYM:
        printf("shell: unexpected special lexeme\n");
        return 1;
        break;

    default:
        return 0;
        break;
    }
}