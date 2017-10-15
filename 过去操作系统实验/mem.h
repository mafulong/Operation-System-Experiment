#ifndef MEM_H
#define MEM_H
#define memSize 100
#include<vector>
using namespace std;
struct PCB
{
    PCB(){

    }

    PCB(int pid1,int priority1,int time1,int memoryNeeds1){
        pid=pid1;
        priority=priority1;
        time=time1;
        memoryNeeds=memoryNeeds1;
    }

    int pid;
    int state;
    int priority;
    int time;    //要求运行时间
    int memoryNeeds;//the needs memory
    int memoryStart;//the memory start place
    operator <(PCB& a){
        return priority<a.priority;
    }
};

struct mymemory{
    mymemory();
    mymemory(int f,int l,int s){
        front=f;
        length=l;
        state=s;
    }
    bool operator <(const mymemory b){
        return front<b.front;
    }

    int front;
    int length;
    int state;//0表示free,1表示使用
};

class mem
{
public:
    vector<mymemory>mymem;
    bool add(PCB& a);//返回结果表示是否添加成功
    void remove(int memoryStart);//删除元素a的空间
    mem();
    int sum;
    void tight();

};

#endif // MEM_H
