#include <iostream>
#include "poliz.h"


void PolizElem::Push(PolizItem **stack, PolizElem *elem) {
    PolizItem *new_item = new PolizItem(elem);
    new_item->next = *stack;
    *stack = new_item;
}

PolizElem* PolizElem::Pop(PolizItem **stack) {
    if (!*stack) throw std::runtime_error("Stack is empty");
    PolizItem *item = *stack;
    PolizElem *elem = item->p;
    *stack = item->next;
    delete item;
    return elem;
}


void PolizConst::Evaluate(PolizItem **stack, PolizItem **cur_cmd) const {
    Push(stack, Clone());
    *cur_cmd = (*cur_cmd)->next;
}

void PolizFunction::Evaluate(PolizItem **stack, PolizItem **cur_cmd) const {
    PolizElem *res = EvaluateFun(stack);
    if(res) Push(stack, res);
    *cur_cmd = (*cur_cmd)->next;
}


void PolizOpGo::Evaluate(PolizItem **stack, PolizItem **cur_cmd) const {
    PolizElem *operand1 = Pop(stack);
    PolizLabel *lab = dynamic_cast<PolizLabel*>(operand1);
    if (!lab) throw PolizExNotLabel(operand1);
    PolizItem *addr = lab->Get();
    *cur_cmd = addr->next;
    delete operand1;
}


void PolizOpGoFalse::Evaluate(PolizItem **stack, PolizItem **cur_cmd) const {
    PolizElem *operand1 = Pop(stack);
    PolizLabel *lab = dynamic_cast<PolizLabel*>(operand1);
    if (!lab) throw PolizExNotLabel(operand1);
    PolizElem *operand2 = Pop(stack);
    PolizInt *i = dynamic_cast<PolizInt*>(operand2);
    if (!i) throw PolizExNotInt(operand2);
    if (!(i->Get())){
        PolizItem *addr = lab->Get();
        *cur_cmd = addr->next;
    }
    else {
        *cur_cmd = (*cur_cmd)->next;
    }
    delete operand1;
    delete operand2;
}


PolizElem* PolizAssign::EvaluateFun(PolizItem **stack) const {
    PolizElem *operand1 = Pop(stack);
    PolizConst *op = dynamic_cast<PolizConst*>(operand1);
    if (!op) throw PolizExNotConst(operand1);
    PolizElem *operand2 = Pop(stack);
    PolizVarAddr *addr = dynamic_cast<PolizVarAddr*>(operand2);
    if (!addr) throw PolizExNotVarAddr(operand2);
    if (PolizInt *int_op = dynamic_cast<PolizInt*>(op)){
        *(int*)(addr->Get()) = int_op->Get();
        // std::cout <<"Assign: " << int_op->Get() << std::endl;
    }
    else if (PolizFloat *float_op = dynamic_cast<PolizFloat*>(op)){
        *(float*)(addr->Get()) = float_op->Get();
    }
    else if (PolizString *string_op = dynamic_cast<PolizString*>(op)){
        *(std::string*)(addr->Get()) = string_op->Get();
    }
    else throw PolizExNotConst(operand1);
    delete operand1;
    delete operand2;
    return nullptr;
}


PolizElem* PolizMassiveAssign::EvaluateFun(PolizItem **stack) const {
    /*PolizElem *operand1 = Pop(stack);
    PolizConst *op = dynamic_cast<PolizConst*>(operand1);
    if (!op) throw PolizExNotConst(operand1);
    PolizElem *operand2 = Pop(stack);
    PolizInt *index = dynamic_cast<PolizInt*>(operand2);
    if (!index) throw PolizExNotInt(operand2);
    if (PolizInt *int_op = dynamic_cast<PolizInt*>(op)){
        *(int*)(addr->Get()) = int_op->Get();
        // std::cout <<"Assign: " << int_op->Get() << std::endl;
    }
    else if (PolizFloat *float_op = dynamic_cast<PolizFloat*>(op)){
        *(float*)(addr->Get()) = float_op->Get();
    }
    else if (PolizString *string_op = dynamic_cast<PolizString*>(op)){
        *(std::string*)(addr->Get()) = string_op->Get();
    }
    else throw PolizExNotConst(operand1);
    delete operand1;
    delete operand2;*/
    return nullptr;
}


PolizElem* PolizVar::EvaluateFun(PolizItem **stack) const {
    if (type == "int") {
        return new PolizInt(*((int*)(addr)));
    }
    else return new PolizFloat(*((float*)(addr)));
}

PolizElem* PolizMassive::EvaluateFun(PolizItem **stack) const {
    PolizElem *operand1 = Pop(stack);
    PolizInt *op = dynamic_cast<PolizInt*>(operand1);
    if (!op) throw PolizExNotInt(operand1);
    int index = op->Get();
    if (index >= max_size) throw PolizExMassiveIsOutOfRange(operand1);

    if (type == "int") {
    std::cout << "\nMassiveEvaluateFun, " << index << std::endl;
    std::cout << "\nMassiveEvaluateFun, " << (**((int**)addr)) << std::endl;
        return new PolizInt(**((int**)(addr + index)));
    }
    else return new PolizFloat(**((float**)(addr + index)));
}


PolizElem* PolizFunPlus::EvaluateFun(PolizItem **stack) const {
    PolizElem *operand1 = Pop(stack);
    PolizConst *op1 = dynamic_cast<PolizConst*>(operand1);
    if (!op1) throw PolizExNotConst(operand1);
    PolizElem *operand2 = Pop(stack);
    PolizConst *op2 = dynamic_cast<PolizConst*>(operand2);
    if (!op2) throw PolizExNotConst(operand2);

    PolizInt *int1 = dynamic_cast<PolizInt*>(op1);
    PolizInt *int2 = dynamic_cast<PolizInt*>(op2);
    PolizFloat *fl1 = dynamic_cast<PolizFloat*>(op1);
    PolizFloat *fl2 = dynamic_cast<PolizFloat*>(op2);
    // int + int

    if (!(int1 || fl1)) throw PolizExNotNumber(operand1);
    if (!(int2 || fl2)) throw PolizExNotNumber(operand2);
    if ((int1) && (int2)) {
        int res = int1->Get() + int2->Get();
        delete operand1;
        delete operand2;
        return new PolizInt(res);
    }
    
    else{
        float res = 0;
        if (int1) res += int1->Get();
        else res += fl1->Get();
        if (int2) res += int2->Get();
        else res += fl2->Get();
        delete operand1;
        delete operand2;
        return new PolizFloat(res);
    }
}

PolizElem* PolizFunMinus::EvaluateFun(PolizItem **stack) const {
    PolizElem *operand1 = Pop(stack);
    PolizConst *op1 = dynamic_cast<PolizConst*>(operand1);
    if (!op1) throw PolizExNotConst(operand1);
    PolizElem *operand2 = Pop(stack);
    PolizConst *op2 = dynamic_cast<PolizConst*>(operand2);
    if (!op2) throw PolizExNotConst(operand2);

    PolizInt *int1 = dynamic_cast<PolizInt*>(op1);
    PolizInt *int2 = dynamic_cast<PolizInt*>(op2);
    PolizFloat *fl1 = dynamic_cast<PolizFloat*>(op1);
    PolizFloat *fl2 = dynamic_cast<PolizFloat*>(op2);
    // int - int

    if (!(int1 || fl1)) throw PolizExNotNumber(operand1);
    if (!(int2 || fl2)) throw PolizExNotNumber(operand2);
    if ((int1) && (int2)) {
        int res = int2->Get() - int1->Get();
        delete operand1;
        delete operand2;
        return new PolizInt(res);
    }
    
    else{
        float res = 0;
        if (int1) res -= int1->Get();
        else res -= fl1->Get();
        if (int2) res += int2->Get();
        else res += fl2->Get();
        delete operand1;
        delete operand2;
        return new PolizFloat(res);
    }
}


PolizElem* PolizFunMul::EvaluateFun(PolizItem **stack) const {
    PolizElem *operand1 = Pop(stack);
    PolizConst *op1 = dynamic_cast<PolizConst*>(operand1);
    if (!op1) throw PolizExNotConst(operand1);
    PolizElem *operand2 = Pop(stack);
    PolizConst *op2 = dynamic_cast<PolizConst*>(operand2);
    if (!op2) throw PolizExNotConst(operand2);

    PolizInt *int1 = dynamic_cast<PolizInt*>(op1);
    PolizInt *int2 = dynamic_cast<PolizInt*>(op2);
    PolizFloat *fl1 = dynamic_cast<PolizFloat*>(op1);
    PolizFloat *fl2 = dynamic_cast<PolizFloat*>(op2);
    // int * int

    if (!(int1 || fl1)) throw PolizExNotNumber(operand1);
    if (!(int2 || fl2)) throw PolizExNotNumber(operand2);
    if ((int1) && (int2)) {
        int res = int2->Get() * int1->Get();
        // std::cout << int1->Get() << ' ' << int2->Get() << std::endl;
        delete operand1;
        delete operand2;
        return new PolizInt(res);
    }
    else{
        float res = 0;
        if (int1) res = int1->Get();
        else res = fl1->Get();
        if (int2) res *= int2->Get();
        else res *= fl2->Get();
        delete operand1;
        delete operand2;
        return new PolizFloat(res);
    }
}


PolizElem* PolizFunDiv::EvaluateFun(PolizItem **stack) const {
    PolizElem *operand1 = Pop(stack);
    PolizConst *op1 = dynamic_cast<PolizConst*>(operand1);
    if (!op1) throw PolizExNotConst(operand1);
    PolizElem *operand2 = Pop(stack);
    PolizConst *op2 = dynamic_cast<PolizConst*>(operand2);
    if (!op2) throw PolizExNotConst(operand2);

    PolizInt *int1 = dynamic_cast<PolizInt*>(op1);
    PolizInt *int2 = dynamic_cast<PolizInt*>(op2);
    PolizFloat *fl1 = dynamic_cast<PolizFloat*>(op1);
    PolizFloat *fl2 = dynamic_cast<PolizFloat*>(op2);
    // int / int

    if (!(int1 || fl1)) throw PolizExNotNumber(operand1);
    if (!(int2 || fl2)) throw PolizExNotNumber(operand2);
    if ((int1) && (int2)) {
        int res = int2->Get() / int1->Get();
        //std::cout << int1->Get() << ' ' << int2->Get() << std::endl;
        delete operand1;
        delete operand2;
        return new PolizInt(res);
    }
    
    else{
        float res = 0;
        if (int2) res = int2->Get();
        else res = fl2->Get();
        if (int1) res /= int1->Get();
        else res /= fl1->Get();
        delete operand1;
        delete operand2;
        return new PolizFloat(res);
    }
}


PolizElem* PolizFunMod::EvaluateFun(PolizItem **stack) const {
    PolizElem *operand1 = Pop(stack);
    PolizConst *op1 = dynamic_cast<PolizConst*>(operand1);
    if (!op1) throw PolizExNotConst(operand1);
    PolizElem *operand2 = Pop(stack);
    PolizConst *op2 = dynamic_cast<PolizConst*>(operand2);
    if (!op2) throw PolizExNotConst(operand2);

    PolizInt *int1 = dynamic_cast<PolizInt*>(op1);
    PolizInt *int2 = dynamic_cast<PolizInt*>(op2);

    if (!(int1)) throw PolizExNotInt(operand1);
    if (!(int2)) throw PolizExNotInt(operand2);
    int res = int2->Get() % int1->Get();
    delete operand1;
    delete operand2;
    return new PolizInt(res);
}


PolizElem* PolizFunLess::EvaluateFun(PolizItem **stack) const {
    PolizElem *operand1 = Pop(stack);
    PolizConst *op1 = dynamic_cast<PolizConst*>(operand1);
    if (!op1) throw PolizExNotConst(operand1);
    PolizElem *operand2 = Pop(stack);
    PolizConst *op2 = dynamic_cast<PolizConst*>(operand2);
    if (!op2) throw PolizExNotConst(operand2);

    float val1, val2;
    
    if (auto int1 = dynamic_cast<PolizInt*>(op1)) {
        val1 = int1->Get();
    } else if (auto fl1 = dynamic_cast<PolizFloat*>(op1)) {
        val1 = fl1->Get();
    } else {
        delete operand1;
        delete operand2;
        throw PolizExNotNumber(op1);
    }

    if (auto int2 = dynamic_cast<PolizInt*>(op2)) {
        val2 = int2->Get();
    } else if (auto fl2 = dynamic_cast<PolizFloat*>(op2)) {
        val2 = fl2->Get();
    } else {
        delete operand1;
        delete operand2;
        throw PolizExNotNumber(op2);
    }
    int result = (val1 > val2) ? 1 : 0;   
    delete operand1;
    delete operand2;
    return new PolizInt(result);
}


PolizElem* PolizFunLessEqual::EvaluateFun(PolizItem **stack) const {
    PolizElem *operand1 = Pop(stack);
    PolizConst *op1 = dynamic_cast<PolizConst*>(operand1);
    if (!op1) throw PolizExNotConst(operand1);
    PolizElem *operand2 = Pop(stack);
    PolizConst *op2 = dynamic_cast<PolizConst*>(operand2);
    if (!op2) throw PolizExNotConst(operand2);

    float val1, val2;
    
    if (auto int1 = dynamic_cast<PolizInt*>(op1)) {
        val1 = int1->Get();
    } else if (auto fl1 = dynamic_cast<PolizFloat*>(op1)) {
        val1 = fl1->Get();
    } else {
        delete operand1;
        delete operand2;
        throw PolizExNotNumber(op1);
    }

    if (auto int2 = dynamic_cast<PolizInt*>(op2)) {
        val2 = int2->Get();
    } else if (auto fl2 = dynamic_cast<PolizFloat*>(op2)) {
        val2 = fl2->Get();
    } else {
        delete operand1;
        delete operand2;
        throw PolizExNotNumber(op2);
    }
    int result = (val1 >= val2) ? 1 : 0;   
    delete operand1;
    delete operand2;
    return new PolizInt(result);
}



PolizElem* PolizFunGreater::EvaluateFun(PolizItem **stack) const {
    PolizElem *operand1 = Pop(stack);
    PolizConst *op1 = dynamic_cast<PolizConst*>(operand1);
    if (!op1) throw PolizExNotConst(operand1);
    PolizElem *operand2 = Pop(stack);
    PolizConst *op2 = dynamic_cast<PolizConst*>(operand2);
    if (!op2) throw PolizExNotConst(operand2);

    float val1, val2;
    
    if (auto int1 = dynamic_cast<PolizInt*>(op1)) {
        val1 = int1->Get();
    } else if (auto fl1 = dynamic_cast<PolizFloat*>(op1)) {
        val1 = fl1->Get();
    } else {
        delete operand1;
        delete operand2;
        throw PolizExNotNumber(op1);
    }

    if (auto int2 = dynamic_cast<PolizInt*>(op2)) {
        val2 = int2->Get();
    } else if (auto fl2 = dynamic_cast<PolizFloat*>(op2)) {
        val2 = fl2->Get();
    } else {
        delete operand1;
        delete operand2;
        throw PolizExNotNumber(op2);
    }
    int result = (val1 < val2) ? 1 : 0;   
    delete operand1;
    delete operand2;
    return new PolizInt(result);
}


PolizElem* PolizFunGreaterEqual::EvaluateFun(PolizItem **stack) const {
    PolizElem *operand1 = Pop(stack);
    PolizConst *op1 = dynamic_cast<PolizConst*>(operand1);
    if (!op1) throw PolizExNotConst(operand1);
    PolizElem *operand2 = Pop(stack);
    PolizConst *op2 = dynamic_cast<PolizConst*>(operand2);
    if (!op2) throw PolizExNotConst(operand2);

    float val1, val2;
    
    if (auto int1 = dynamic_cast<PolizInt*>(op1)) {
        val1 = int1->Get();
    } else if (auto fl1 = dynamic_cast<PolizFloat*>(op1)) {
        val1 = fl1->Get();
    } else {
        delete operand1;
        delete operand2;
        throw PolizExNotNumber(op1);
    }

    if (auto int2 = dynamic_cast<PolizInt*>(op2)) {
        val2 = int2->Get();
    } else if (auto fl2 = dynamic_cast<PolizFloat*>(op2)) {
        val2 = fl2->Get();
    } else {
        delete operand1;
        delete operand2;
        throw PolizExNotNumber(op2);
    }
    int result = (val1 <= val2) ? 1 : 0;   
    delete operand1;
    delete operand2;
    return new PolizInt(result);
}

PolizElem* PolizFunEqual::EvaluateFun(PolizItem **stack) const {
    PolizElem *operand1 = Pop(stack);
    PolizConst *op1 = dynamic_cast<PolizConst*>(operand1);
    if (!op1) throw PolizExNotConst(operand1);
    PolizElem *operand2 = Pop(stack);
    PolizConst *op2 = dynamic_cast<PolizConst*>(operand2);
    if (!op2) throw PolizExNotConst(operand2);

    float val1, val2;
    
    if (auto int1 = dynamic_cast<PolizInt*>(op1)) {
        val1 = int1->Get();
    } else if (auto fl1 = dynamic_cast<PolizFloat*>(op1)) {
        val1 = fl1->Get();
    } else {
        delete operand1;
        delete operand2;
        throw PolizExNotNumber(op1);
    }

    if (auto int2 = dynamic_cast<PolizInt*>(op2)) {
        val2 = int2->Get();
    } else if (auto fl2 = dynamic_cast<PolizFloat*>(op2)) {
        val2 = fl2->Get();
    } else {
        delete operand1;
        delete operand2;
        throw PolizExNotNumber(op2);
    }
    int result = (val1 == val2) ? 1 : 0;   
    delete operand1;
    delete operand2;
    return new PolizInt(result);
}


PolizElem* PolizFunNotEqual::EvaluateFun(PolizItem **stack) const {
    PolizElem *operand1 = Pop(stack);
    PolizConst *op1 = dynamic_cast<PolizConst*>(operand1);
    if (!op1) throw PolizExNotConst(operand1);
    PolizElem *operand2 = Pop(stack);
    PolizConst *op2 = dynamic_cast<PolizConst*>(operand2);
    if (!op2) throw PolizExNotConst(operand2);

    float val1, val2;
    
    if (auto int1 = dynamic_cast<PolizInt*>(op1)) {
        val1 = int1->Get();
    } else if (auto fl1 = dynamic_cast<PolizFloat*>(op1)) {
        val1 = fl1->Get();
    } else {
        delete operand1;
        delete operand2;
        throw PolizExNotNumber(op1);
    }

    if (auto int2 = dynamic_cast<PolizInt*>(op2)) {
        val2 = int2->Get();
    } else if (auto fl2 = dynamic_cast<PolizFloat*>(op2)) {
        val2 = fl2->Get();
    } else {
        delete operand1;
        delete operand2;
        throw PolizExNotNumber(op2);
    }
    int result = (val1 != val2) ? 1 : 0;   
    delete operand1;
    delete operand2;
    return new PolizInt(result);
}

PolizElem* PolizFunAnd::EvaluateFun(PolizItem **stack) const {
    PolizElem *operand1 = Pop(stack);
    PolizConst *op1 = dynamic_cast<PolizConst*>(operand1);
    if (!op1) throw PolizExNotConst(operand1);
    PolizElem *operand2 = Pop(stack);
    PolizConst *op2 = dynamic_cast<PolizConst*>(operand2);
    if (!op2) throw PolizExNotConst(operand2);

    float val1, val2;
    
    if (auto int1 = dynamic_cast<PolizInt*>(op1)) {
        val1 = int1->Get();
    } else if (auto fl1 = dynamic_cast<PolizFloat*>(op1)) {
        val1 = fl1->Get();
    } else {
        delete operand1;
        delete operand2;
        throw PolizExNotNumber(op1);
    }

    if (auto int2 = dynamic_cast<PolizInt*>(op2)) {
        val2 = int2->Get();
    } else if (auto fl2 = dynamic_cast<PolizFloat*>(op2)) {
        val2 = fl2->Get();
    } else {
        delete operand1;
        delete operand2;
        throw PolizExNotNumber(op2);
    }
    int result = (val1 && val2) ? 1 : 0;   
    delete operand1;
    delete operand2;
    return new PolizInt(result);
}


PolizElem* PolizFunOr::EvaluateFun(PolizItem **stack) const {
    PolizElem *operand1 = Pop(stack);
    PolizConst *op1 = dynamic_cast<PolizConst*>(operand1);
    if (!op1) throw PolizExNotConst(operand1);
    PolizElem *operand2 = Pop(stack);
    PolizConst *op2 = dynamic_cast<PolizConst*>(operand2);
    if (!op2) throw PolizExNotConst(operand2);

    float val1, val2;
    
    if (auto int1 = dynamic_cast<PolizInt*>(op1)) {
        val1 = int1->Get();
    } else if (auto fl1 = dynamic_cast<PolizFloat*>(op1)) {
        val1 = fl1->Get();
    } else {
        delete operand1;
        delete operand2;
        throw PolizExNotNumber(op1);
    }

    if (auto int2 = dynamic_cast<PolizInt*>(op2)) {
        val2 = int2->Get();
    } else if (auto fl2 = dynamic_cast<PolizFloat*>(op2)) {
        val2 = fl2->Get();
    } else {
        delete operand1;
        delete operand2;
        throw PolizExNotNumber(operand2);
    }
    int result = (val1 || val2) ? 1 : 0;   
    delete operand1;
    delete operand2;
    return new PolizInt(result);
}


PolizElem* PolizFunNot::EvaluateFun(PolizItem **stack) const {
    PolizElem *operand1 = Pop(stack);
    PolizConst *op1 = dynamic_cast<PolizConst*>(operand1);
    if (!op1) throw PolizExNotConst(operand1);

    float val1;
    
    if (auto int1 = dynamic_cast<PolizInt*>(op1)) {
        val1 = int1->Get();
    } else if (auto fl1 = dynamic_cast<PolizFloat*>(op1)) {
        val1 = fl1->Get();
    } else {
        delete operand1;
        throw PolizExNotNumber(operand1);
    }
    int result = (!val1) ? 1 : 0;   
    delete operand1;
    return new PolizInt(result);
}


PolizElem* PolizBuy::EvaluateFun(PolizItem **stack) const {
    PolizElem *priceElem = PolizElem::Pop(stack);
    PolizElem *quantityElem = PolizElem::Pop(stack);

    PolizInt *price = dynamic_cast<PolizInt*>(priceElem);
    PolizInt *quantity = dynamic_cast<PolizInt*>(quantityElem);
    if (!price) throw PolizExNotInt(priceElem);
    if (!quantity) throw PolizExNotInt(quantityElem);

    // std::cout << "[BUY] Quantity: " << quantity->Get() << ", Price: " << price->Get() << std::endl;

    delete priceElem;
    delete quantityElem;
    return nullptr;
}



PolizElem* PolizSell::EvaluateFun(PolizItem **stack) const {
    PolizElem *priceElem = PolizElem::Pop(stack);
    PolizElem *quantityElem = PolizElem::Pop(stack);

    PolizInt *price = dynamic_cast<PolizInt*>(priceElem);
    PolizInt *quantity = dynamic_cast<PolizInt*>(quantityElem);
    if (!price) throw PolizExNotInt(priceElem);
    if (!quantity) throw PolizExNotInt(quantityElem);

    // std::cout << "[SELL] Quantity: " << quantity->Get() << ", Price: " << price->Get() << std::endl;

    delete priceElem;
    delete quantityElem;
    return nullptr;
}


PolizElem* PolizProd::EvaluateFun(PolizItem **stack) const {
    PolizElem *quantityElem = PolizElem::Pop(stack);
    PolizInt *quantity = dynamic_cast<PolizInt*>(quantityElem);
    if (!quantity) throw PolizExNotInt(quantityElem);

    // std::cout << "[PROD] Quantity: " << quantity->Get() << std::endl;
    delete quantityElem;
    return nullptr;
}


PolizElem* PolizBuild::EvaluateFun(PolizItem **stack) const {
    PolizElem *quantityElem = PolizElem::Pop(stack);
    PolizInt *quantity = dynamic_cast<PolizInt*>(quantityElem);
    if (!quantity) throw PolizExNotInt(quantityElem);

    // std::cout << "[BUILD] Quantity: " << quantity->Get() << std::endl;
    delete quantityElem;
    return nullptr;
}


PolizElem* PolizEndturn::EvaluateFun(PolizItem **stack) const {
    // std::cout << "[ENDTURN]" << std::endl;
    return nullptr;
}


PolizElem* PolizPrint::EvaluateFun(PolizItem **stack) const {
    PolizElem *quantityElem = PolizElem::Pop(stack);
    PolizInt *quantity = dynamic_cast<PolizInt*>(quantityElem);
    if (!quantity) throw PolizExNotInt(quantityElem);

    std::vector<PolizElem*> args;
    int i = 0;
    while (i++ < quantity->Get()) {
        PolizElem *elem = PolizElem::Pop(stack);
        args.push_back(elem);
    }

    for (auto it = args.rbegin(); it != args.rend(); ++it) {
        PolizElem *elem = *it;
        if (PolizString *str = dynamic_cast<PolizString*>(elem)) {
            std::cout << str->Get();
        } else if (PolizInt *num = dynamic_cast<PolizInt*>(elem)) {
            std::cout << num->Get();
        } else if (PolizFloat *f = dynamic_cast<PolizFloat*>(elem)) {
            std::cout << f->Get();
        } else {
            delete elem;
            throw PolizExNotPrintable(elem);
        }
        delete elem;
    }
    return nullptr;
}


PolizElem* PolizFunMyId::EvaluateFun(PolizItem **stack) const {
    int myId = 2;
    // std::cout << "my_id" << std::endl;
    return new PolizInt(myId);
}

PolizElem* PolizFunCurTurn::EvaluateFun(PolizItem **stack) const {
    int cur_turn = 1;
    // std::cout << "Cur_turn" << std::endl;
    return new PolizInt(cur_turn);
}


PolizElem* PolizFunPlayers::EvaluateFun(PolizItem **stack) const {
    int players = 2;
    // std::cout << "Players" << std::endl;
    return new PolizInt(players);
}

PolizElem* PolizFunActivePlayers::EvaluateFun(PolizItem **stack) const {
    int act_players = 2;
    // std::cout << "Active players" << std::endl;
    return new PolizInt(act_players);
}


void PolizInt::print() {
    std::cout << value << " \tКонстанта int: " << std::endl;
}

void PolizFloat::print() {
    std::cout << value << " \tКонстанта float: "<< std::endl;
}

void PolizString::print() {
    std::cout << value << " \tСтроковая константа: " << std::endl;
}

void PolizVarAddr::print() {
    std::cout << "& \tАдресс переменной " << value << std::endl;
}

void PolizLabel::print() {
    std::cout << "Label \tУказатель перехода"  << std::endl;
}

void PolizOpGo::print() {
    std::cout << "!  \tОператор безусловного перехода OpGo" << std::endl;
}

void PolizOpGoFalse::print() {
    std::cout << "!F \tОператор перехода по лжи OpGoFalse" << std::endl;
}

void PolizAssign::print() {
    std::cout << "= \tОператор присвоения" << std::endl;
}

void PolizVar::print() {
    std::cout << type << " variable \t Переменная типа " + type << std::endl;
}

void PolizFunPlus::print() {
    std::cout << "+ \t  Оператор плюс" << std::endl;
}

void PolizFunMinus::print() {
    std::cout << "- \t  Оператор минус" << std::endl;
}

void PolizFunMul::print() {
    std::cout << "* \t  Оператор умножения" << std::endl;
}

void PolizFunDiv::print() {
    std::cout << "/ \t  Оператор деления" << std::endl;
}

void PolizFunMod::print() {
    std::cout << "% \t  Оператор взятия остатка" << std::endl;
}

void PolizFunLess::print() {
    std::cout << "< \t Оператор меньше" << std::endl;
}

void PolizFunLessEqual::print() {
    std::cout << "<= \t Оператор меньше или равно" << std::endl;
}

void PolizFunGreater::print() {
    std::cout << "> \t Оператор больше" << std::endl;
}

void PolizFunGreaterEqual::print() {
    std::cout << ">= \t Оператор больше или равно" << std::endl;
}

void PolizFunEqual::print() {
    std::cout << "== \t Оператор равенства" << std::endl;
}

void PolizFunNotEqual::print() {
    std::cout << "!= \t Оператор не равно" << std::endl;
}

void PolizFunAnd::print() {
    std::cout << "&& \t Логическое И" << std::endl;
}

void PolizFunOr::print() {
    std::cout << "|| \t Логические ИЛИ" << std::endl;
}

void PolizBuy::print() {
    std::cout << "buy \t Функция buy" << std::endl;
}

void PolizSell::print() {
    std::cout << "sell\t Функция sell" << std::endl;
}

void PolizPrint::print() {
    std::cout << "print \t Функция print" << std::endl;
}
void PolizBuild::print() {
    std::cout << "build \t Функция build" << std::endl;
}
void PolizProd::print() {
    std::cout << "prod \t Функция prod" << std::endl;
}
void PolizEndturn::print() {
    std::cout << "turn \t Функция turn" << std::endl;
}