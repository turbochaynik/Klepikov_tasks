#ifndef POLIZ_EX
#define POLIZ_EX

#include <iostream>
#include <string>
#include "poliz.h"

class PolizElem;
class PolizEx{
public:
    virtual void print() const = 0;
    virtual ~PolizEx() {}
};


class PolizExNotLabel: public PolizEx{
    PolizElem *p;
public:
    PolizExNotLabel(PolizElem *a): p(a) {}
    virtual ~PolizExNotLabel() override {}
    virtual void print() const override {
        std::cout << "PolizExNotLabel: " << p << std::endl;
    }
};


class PolizExNotInt: public PolizEx{
    PolizElem *p;
public:
    PolizExNotInt(PolizElem *a): p(a) {}
    virtual ~PolizExNotInt() override {}
    virtual void print() const override {
        std::cout << "PolizExNotInt: " << p << std::endl;
    }
};

class PolizExNotFloat: public PolizEx{
    PolizElem *p;
public:
    PolizExNotFloat(PolizElem *a): p(a) {}
    virtual ~PolizExNotFloat() override {}
    virtual void print() const override {
        std::cout << "PolizExNotFloat: " << p << std::endl;
    }
};

class PolizExNotString: public PolizEx{
    PolizElem *p;
public:
    PolizExNotString(PolizElem *a): p(a) {}
    virtual ~PolizExNotString() override {}
    virtual void print() const override {
        std::cout << "PolizExNotString: " << p << std::endl;
    }
};


class PolizExNotConst: public PolizEx {
    PolizElem *p;
public:
    PolizExNotConst(PolizElem *a): p(a) {}
    virtual ~PolizExNotConst() override {}
    virtual void print() const override {
        std::cout << "PolizExNotConst: " << p << std::endl;
    }
};

class PolizExNotVarAddr: public PolizEx {
    PolizElem *p;
public:
    PolizExNotVarAddr(PolizElem *a): p(a) {}
    virtual ~PolizExNotVarAddr() override {}
    virtual void print() const override {
        std::cout << "PolizExNotVarAddr: " << p << std::endl;
    }
};


class PolizExNotNumber: public PolizEx {
    PolizElem *p;
public:
    PolizExNotNumber(PolizElem *a): p(a) {}
    virtual ~PolizExNotNumber() override {}
    virtual void print() const override {
        std::cout << "PolizExNotNumber: " << p << std::endl;
    }
};

class PolizExNotPrintable : public PolizEx {
    PolizElem *p;
public:
    PolizExNotPrintable(PolizElem *a): p(a) {}
    virtual ~PolizExNotPrintable() override {}
    virtual void print() const override {
        std::cout << "PolizExNotPrintable: " << p << std::endl;
    }
};

class PolizExMassiveIsOutOfRange : public PolizEx {
    PolizElem *p;
public:
    PolizExMassiveIsOutOfRange(PolizElem *a): p(a) {}
    virtual ~PolizExMassiveIsOutOfRange() override {}
    virtual void print() const override {
        std::cout << "PolizExMassiveIsOutOfRange: " << p << std::endl;
    }
};

#endif