#include "executer.h"
void List::Add(PolizElem *elem){
    PolizItem *new_item = new PolizItem(elem);
    if (!start) start = new_item;
    else {
        PolizItem *cur_p = start;
        while(cur_p->next) cur_p = cur_p->next;
        cur_p->next = new_item;
    }
}


PolizItem* List::Get_End() {
    if (start == nullptr) throw std::runtime_error("List is empty");
    PolizItem *cur_p = start;
        while(cur_p->next) cur_p = cur_p->next;
    return cur_p;
}


PolizElem* List::Current() const {
    if (cur == nullptr) throw std::runtime_error("List is empty");
    return cur->p;
}

List::~List() {
    while (start) {
        PolizItem *temp = start;
        start = start->next;
        delete temp->p;
        delete temp;
    }
    cur = nullptr;
}



void Executer::Execute() {
    //int a[30];
    //std::string b[30];
    //for (int i = 0; i < 30; ++i) a[i] = 0;
    PolizItem *cur_cmd = beginning;
    //int i = 1;
    while (cur_cmd) {
        PolizElem *cmd = cur_cmd->p;
        /*if (dynamic_cast<PolizOpGo*>(cmd)) a[0]++;
        else if (dynamic_cast<PolizOpGoFalse*>(cmd)) a[2]++;
        else if (dynamic_cast<PolizFunPlus*>(cmd)) a[3]++;
        else if (dynamic_cast<PolizFunMinus*>(cmd)) a[4]++;
        else if (dynamic_cast<PolizFunDiv*>(cmd)) a[5]++;
        else if (dynamic_cast<PolizFunMod*>(cmd)) a[6]++;
        else if (dynamic_cast<PolizFunMul*>(cmd)) a[7]++;
        else if (dynamic_cast<PolizFunGreater*>(cmd)) a[8]++;
        else if (dynamic_cast<PolizFunGreaterEqual*>(cmd)) a[9]++;
        else if (dynamic_cast<PolizFunLess*>(cmd)) a[10]++;
        else if (dynamic_cast<PolizFunLessEqual*>(cmd)) a[11]++;
        else if (dynamic_cast<PolizFunEqual*>(cmd)) a[12]++;
        else if (dynamic_cast<PolizFunNotEqual*>(cmd)) a[13]++;
        else if (dynamic_cast<PolizFunAnd*>(cmd)) a[14]++;
        else if (dynamic_cast<PolizFunOr*>(cmd)) a[15]++;
        else if (dynamic_cast<PolizAssign*>(cmd)) a[16]++;
        else if (dynamic_cast<PolizPrint*>(cmd)) a[17]++;*/

        //Вывод ПОЛИЗ
        // std::cout << i++ << '.';
        // cmd->print();
        //Вывод ПОЛИЗ
        cmd->Evaluate(&stack, &cur_cmd);
        if (!cur_cmd) break;
    }
    /*int max = 0;
    for (int i = 0; i < 30; ++i){
        if (a[i] > max) max = a[i];
    }   
        std::cout << "The most popular:" << std::endl;

        if (a[0] == max) std::cout<<"PolizOpGo\n";
        else if (a[2] == max)std::cout<<"PolizOpGoFalse\n";
        else if (a[3] == max)std::cout<<"PolizFunPlus\n";
        else if (a[4] == max)std::cout<<"PolizFunMinus\n";
        else if (a[5] == max)std::cout<<"PolizFunDiv\n";
        else if (a[6] == max)std::cout<<"PolizFunMod\n";
        else if (a[7] == max)std::cout<<"PolizFunMul\n";
        else if (a[8] == max)std::cout<<"PolizFunGreater\n";
        else if (a[9] == max)std::cout<<"PolizFunGreaterEqual\n";
        else if (a[10] == max)std::cout<<"PolizFunLess\n";
        else if (a[11] == max)std::cout<<"PolizFunLessEqual\n";
        else if (a[12] == max)std::cout<<"PolizFunEqual\n";
        else if (a[13] == max)std::cout<<"PolizFunNotEqual\n";
        else if (a[14] == max)std::cout<<"PolizFunAnd\n";
        else if (a[15] == max)std::cout<<"PolizFunOr\n";
        else if (a[16] == max)std::cout<<"PolizAssign\n";
        else if (a[17] == max)std::cout<<"PolizPrint\n";*/

}