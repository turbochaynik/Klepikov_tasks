#ifndef ERROR_H
#define ERROR_H

#include "list.h"

enum {
    ERR_BRCKT = 1,
    ERR_QTN_MRK = 2,
    ERR_SMCLN = 3,
    ERR_APSND = 4,
    ERR_DBL_APSND = 5,
    ERR_OUTPT = 6,
    ERR_DBL_OUTPT = 7,
    ERR_INP = 8,
    ERR_VRTCL_BR = 9,
    ERR_DBL_VRTCL_BR = 10,
    ERR_INP_FILE = 11,
    ERR_OUTPT_FILE = 12,
    ERR_PIPE = 13,
    ERR_AND_PR = 14,
    ERR_OR_PR = 15,
    ERR_NEXT = 16,
    ERR_SPCL_SYM = 17
};

int check_error(Node *list);
int print_error(int err);

#endif