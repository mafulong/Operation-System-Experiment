#ifndef PCBQUEUE_H
#define PCBQUEUE_H

#include<iostream>
#include<stdio.h>
#include<queue>
#define TASK_RESERVE   0 //后备状态
#define TASK_READY       1 //就绪状态
#define TASK_RUNNING   2 //运行状态
#define TASK_WAITE       3 //阻塞状态
#define TASK_HANG       4 //挂起状态
#define TASK_OVER       5 //finished状态
#define DEFAULT_TIME_SLICE  10
#define MAX_MEM 30
using namespace std;

struct PCB
{

    int pid;
    int state;
    int priority;
    int time;    //要求运行时间
    int memoryNeeds;//the needs memory
    int memoryStart;//the memory start place
    PCB *next;
};

class pcbqueue
{
public:
    pcbqueue();
    int num;
    PCB* top();
    PCB *head;//the head of the queue,without any data
    PCB* newPcb(int pid,int priority,int time,int memory);
    bool isempty();
    int push(PCB* b);
    int pushRear(PCB* b);
    PCB* pop();//head pop
    PCB* findPop(int mypid);
};

#endif // PCBQUEUE_H
