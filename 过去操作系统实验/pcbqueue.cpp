#include "pcbqueue.h"

pcbqueue::pcbqueue()
{
    num = 0;
    head = new PCB();
    head->next = NULL;
}
bool pcbqueue::isempty() {//return isempty
    return num == 0;
}
PCB* pcbqueue::newPcb(int pid, int priority, int time,int memory){
    PCB* a=new PCB();
    a->pid=pid;
    a->priority=priority;
    a->time=time;
    a->next=NULL;
    a->memoryNeeds=memory;
    return a;
}

PCB* pcbqueue::top(){
    return head->next;
}

int pcbqueue::push(PCB* b) {//push the node into queue with priority
    num++;
    PCB *a=new PCB();
    *a=*b;
    a->next = NULL;
    PCB *p = head;
    PCB *t = head->next;
    while (t != NULL&&a->priority<t->priority) {
        p = t;
        t = t->next;
    }
    if (t == NULL) {
        p->next = a;
    }
    else {
        p->next = a;
        a->next = t;
    }
    return 0;
}
int pcbqueue::pushRear(PCB *b){//if by timePian,use this
    num++;
    PCB *a=new PCB();
    *a=*b;
    a->next = NULL;
    PCB *p = head;
    while(p->next!=NULL){
        p=p->next;
    }
    p->next=a;
    return 0;
}

PCB* pcbqueue::pop() {//pop the max priority node
    if (head->next != NULL) {
        PCB* a=new PCB();
        *a=*head->next;
        head = head->next;
        a->next = NULL;
        num--;
        return a;
    }
    else {
        return NULL;
    }
}
PCB* pcbqueue::findPop(int mypid) {
    PCB* t = head;
    PCB* p = head->next;
    while (p != NULL&&p->pid != mypid)
    {
        t = p;
        p = p->next;
    }
    if (p == NULL)
        return NULL;
    else {
        num--;
        PCB* p1 = new PCB();
        *p1 = *p;
//        cout << p1->pid <<"time"<<p1->priority<<p1->time<< endl;
        t->next=p->next;

        return p1;
    }
}
