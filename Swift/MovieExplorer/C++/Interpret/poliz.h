#ifndef POLIZ
#define POLIZ

#include "polizEx.h"
#include <string>
#include <vector>
class PolizElem;
struct PolizItem {
public:
    PolizElem *p;
    PolizItem *next;
    PolizItem(): p(nullptr), next(nullptr) {}
    PolizItem(PolizElem *a): p(a), next(nullptr) {}
    PolizItem(PolizElem *a, PolizItem *b): p(a), next(b) {}
    ~PolizItem() {};
};

class PolizElem {
public:
    virtual void Evaluate(PolizItem **stack, PolizItem **cur_cmd) const = 0;
    virtual ~PolizElem() {};
    virtual void print() = 0;
    static void Push(PolizItem **stack, PolizElem *elem);
    static PolizElem* Pop(PolizItem **stack); 
};

class PolizConst : public PolizElem {
public:
    virtual void Evaluate(PolizItem **stack, PolizItem **cur_cmd) const override;
    virtual void print() = 0;
    protected:
    virtual PolizElem* Clone() const = 0;
};

class PolizInt : public PolizConst {
    int value;
public:
    PolizInt(int a): value(a) {}
    virtual ~PolizInt() override {}
    virtual PolizElem* Clone() const override {return new PolizInt(value);}
    virtual void print() override;
    int Get() const {return value;}
};


class PolizFloat : public PolizConst {
    float value;
public:
    PolizFloat(float a): value(a) {}
    virtual ~PolizFloat() override {}
    virtual PolizElem* Clone() const override {return new PolizFloat(value);}
    virtual void print() override;
    float Get() const {return value;}
};

class PolizString : public PolizConst {
    std::string value;
public:
    PolizString(std::string a): value(a) {}
    virtual ~PolizString() override {}
    virtual PolizElem* Clone() const override {return new PolizString(value);}
    virtual void print() override;
    std::string Get() const {return value;}
};

class PolizVarAddr : public PolizConst {
    void* value;
public:
    PolizVarAddr(void* a): value(a) {}
    virtual ~PolizVarAddr() override {}
    virtual PolizElem* Clone() const override {return new PolizVarAddr(value);}
    virtual void print() override;
    void* Get() const {return value;}
};

class PolizLabel : public PolizConst {
    PolizItem* value;
public:
    PolizLabel(PolizItem* a): value(a) {}
    virtual ~PolizLabel() override {}
    virtual PolizElem* Clone() const override {return new PolizLabel(value);}
    virtual void print() override;
    PolizItem* Get() const {return value;}
    void Set(PolizItem* pointer) {value = pointer;}
};

class PolizOpGo : public PolizElem {
public:
    
    PolizOpGo(){}
    virtual ~PolizOpGo() override {}
    void Evaluate(PolizItem **stack, PolizItem **cur_cmd) const override;
    virtual void print() override;
}; 

class PolizOpGoFalse : public PolizElem {
public:
    
    PolizOpGoFalse() {}
    virtual ~PolizOpGoFalse() override {}
    void Evaluate(PolizItem **stack, PolizItem **cur_cmd) const override;
    virtual void print() override;
};

class PolizFunction : public PolizElem {
public:
    void Evaluate(PolizItem **stack, PolizItem **cur_cmd) const override;
    virtual void print() = 0;
protected:
    virtual PolizElem* EvaluateFun(PolizItem **stack) const = 0;
};

class PolizAssign : public PolizFunction {
public:
    
    PolizAssign() {}
    virtual ~PolizAssign() override {}
    PolizElem* EvaluateFun(PolizItem **stack) const override;
    virtual void print() override;
};


class PolizMassiveAssign : public PolizFunction {
public:
    PolizMassiveAssign() {}
    virtual ~PolizMassiveAssign() override {}
    PolizElem* EvaluateFun(PolizItem **stack) const override;
};


class PolizVar : public PolizFunction {
    void* addr;
    std::string type;
public:
    PolizVar(std::string t, void* p){type = t; addr = p;}
    virtual ~PolizVar() override {}
    virtual void print() override;
    
    PolizElem* EvaluateFun(PolizItem **stack) const override;
};


class PolizMassive : public PolizFunction {
    void** addr;
    std::string type;
    int max_size;
public:
    PolizMassive(std::string t, void** p, int x){type = t; addr = p; max_size = x;}
    virtual ~PolizMassive() override {}
    
    PolizElem* EvaluateFun(PolizItem **stack) const override;
};


class PolizFunPlus : public PolizFunction {
public:
    
    PolizFunPlus() {}
    virtual ~PolizFunPlus() override {}
    PolizElem* EvaluateFun(PolizItem **stack) const override;
    virtual void print() override;
};

class PolizFunMinus : public PolizFunction {
public:
    
    PolizFunMinus() {}
    virtual ~PolizFunMinus() override {}
    PolizElem* EvaluateFun(PolizItem **stack) const override;
    virtual void print() override;
};

class PolizFunMul : public PolizFunction {
public:
    
    PolizFunMul() {}
    virtual ~PolizFunMul() override {}
    PolizElem* EvaluateFun(PolizItem **stack) const override;
    virtual void print() override;
};

class PolizFunDiv : public PolizFunction {
public:
    
    PolizFunDiv() {}
    virtual ~PolizFunDiv() override {}
    PolizElem* EvaluateFun(PolizItem **stack) const override;
    virtual void print() override;
};

class PolizFunMod : public PolizFunction {
public:
    
    PolizFunMod() {}
    virtual ~PolizFunMod() override {}
    PolizElem* EvaluateFun(PolizItem **stack) const override;
    virtual void print() override;
};

class PolizFunLess : public PolizFunction {
public:
    
    PolizFunLess() {}
    virtual ~PolizFunLess() override {}
    PolizElem* EvaluateFun(PolizItem **stack) const override;
    virtual void print() override;
};

class PolizFunGreater : public PolizFunction {
public:
    
    PolizFunGreater() {}
    virtual ~PolizFunGreater() override {}
    PolizElem* EvaluateFun(PolizItem **stack) const override;
    virtual void print() override;
};

class PolizFunLessEqual : public PolizFunction {
public:
    
    PolizFunLessEqual() {}
    virtual ~PolizFunLessEqual() override {}
    PolizElem* EvaluateFun(PolizItem **stack) const override;
    virtual void print() override;
};


class PolizFunGreaterEqual : public PolizFunction {
public:
    
    PolizFunGreaterEqual() {}
    virtual ~PolizFunGreaterEqual() override {}
    PolizElem* EvaluateFun(PolizItem **stack) const override;
    virtual void print() override;
};

class PolizFunEqual : public PolizFunction {
public:
    
    PolizFunEqual() {}
    virtual ~PolizFunEqual() override {}
    PolizElem* EvaluateFun(PolizItem **stack) const override;
    virtual void print() override;
};

class PolizFunNotEqual : public PolizFunction {
public:
    
    PolizFunNotEqual() {}
    virtual ~PolizFunNotEqual() override {}
    PolizElem* EvaluateFun(PolizItem **stack) const override;
    virtual void print() override;
};

class PolizFunAnd : public PolizFunction {
public:
    
    PolizFunAnd() {}
    virtual ~PolizFunAnd() override {}
    PolizElem* EvaluateFun(PolizItem **stack) const override;
    virtual void print() override;
};

class PolizFunOr : public PolizFunction {
public:
    
    PolizFunOr() {}
    virtual ~PolizFunOr() override {}
    PolizElem* EvaluateFun(PolizItem **stack) const override;
    virtual void print() override;
};

class PolizFunNot : public PolizFunction {
public:
    
    PolizFunNot() {}
    virtual ~PolizFunNot() override {}
    PolizElem* EvaluateFun(PolizItem **stack) const override;
};

class PolizBuy : public PolizFunction {
public:
    
    PolizBuy() {}
    virtual ~PolizBuy() override {}
    PolizElem* EvaluateFun(PolizItem **stack) const override;
    virtual void print() override;
};

class PolizSell : public PolizFunction {
public:
    
    PolizSell() {}
    virtual ~PolizSell() override {}
    PolizElem* EvaluateFun(PolizItem **stack) const override;
    virtual void print() override;
};

class PolizProd : public PolizFunction {
public:
    
    PolizProd() {}
    virtual ~PolizProd() override {}
    PolizElem* EvaluateFun(PolizItem **stack) const override;
    virtual void print() override;
};

class PolizBuild : public PolizFunction {
public:
    
    PolizBuild() {}
    virtual ~PolizBuild() override {}
    PolizElem* EvaluateFun(PolizItem **stack) const override;
    virtual void print() override;
};

class PolizEndturn : public PolizFunction {
public:
    
    PolizEndturn() {}
    virtual ~PolizEndturn() override {}
    PolizElem* EvaluateFun(PolizItem **stack) const override;
    virtual void print() override;
};

class PolizPrint : public PolizFunction {
public:
    
    PolizPrint() {}
    virtual ~PolizPrint() override {}
    PolizElem* EvaluateFun(PolizItem **stack) const override;
    virtual void print() override;
};

class PolizFunMyId : public PolizFunction {
public:
    PolizFunMyId() {}
    virtual ~PolizFunMyId() override {}
    PolizElem* EvaluateFun(PolizItem **stack) const override;
};

class PolizFunCurTurn : public PolizFunction {
public:
    PolizFunCurTurn() {}
    virtual ~PolizFunCurTurn() override {}
    PolizElem* EvaluateFun(PolizItem **stack) const override;
};

class PolizFunPlayers : public PolizFunction {
public:
    PolizFunPlayers() {}
    virtual ~PolizFunPlayers() override {}
    PolizElem* EvaluateFun(PolizItem **stack) const override;
};

class PolizFunActivePlayers: public PolizFunction {
public:
    PolizFunActivePlayers() {}
    virtual ~PolizFunActivePlayers() override {}
    PolizElem* EvaluateFun(PolizItem **stack) const override;
};
#endif