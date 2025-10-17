#ifndef EXECUTER 
#define EXECUTER

#include "iostream"
#include "poliz.h"
#include <vector>

class List {
public:
    PolizItem* start;
    PolizItem* cur;
    List (): start(nullptr){};
    ~List();
    void Add(PolizElem *elem);
    PolizElem* Current() const;
    PolizItem* Get_End();
};

class Executer {
    PolizItem *beginning;
    PolizItem* stack;
public:
    Executer(List& lst) : beginning(lst.start), stack(nullptr) {}
    void Execute();
};

#endif