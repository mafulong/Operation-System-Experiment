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
    int state;//0表示后备，1表示就绪，2表示运行，3表示挂起, 4表示死亡
    int memory;
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

struct memory_list_st{
    int start;
    int end;
    long pid;
};

typedef struct memory_list_st memoryIterm;

class Memory_cl{
public:
    Memory_cl(int length=500);
    ~Memory_cl();
    int length();
    int remain();
    bool loadPCB(PCB *pcb);
    bool canLoadPCB(PCB *pcb);
    bool deletePCB(PCB *pcb);
private:
    void tight();
    int _length;
    int _remain;
    memoryIterm* list;
    int size;
};

typedef class Memory_cl MCB;

#endif // PCB_H
