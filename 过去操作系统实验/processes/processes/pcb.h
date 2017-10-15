#ifndef PCB_H
#define PCB_H
#include "string.h"
#include <iostream>
using namespace std;

struct pcb_st{
    string name;
    long pid;
    int pri;
    int time;
    int state;//0表示后备，1表示就绪，2表示运行，3表示挂起
    struct pcb_st *next;
    struct pcb_st *pre;
};

std::string getStateString(int state);

typedef struct pcb_st PCB;

enum scheduling_em{
    def,
    priority,
    first
};
typedef enum scheduling_em scheduling;

#endif // PCB_H
