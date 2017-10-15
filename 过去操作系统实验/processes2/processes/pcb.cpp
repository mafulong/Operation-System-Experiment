#include "pcb.h"

std::string getStateString(int state){
    switch (state) {
    case 0:
        return "后备";
    case 1:
        return "就绪";
    case 2:
        return "运行";
    case 3:
        return "挂起";
    case 4:
        return "死亡";
    default:
        break;
    }
    return "";
}


Memory_cl::Memory_cl(int length):_length(length),_remain(length){
    list=new memoryIterm[length];
    size=0;
}

Memory_cl::~Memory_cl(){
    delete list;
}

int Memory_cl::length(){
    return _length;
}

int Memory_cl::remain(){
    return _remain;
}

bool Memory_cl::loadPCB(PCB *pcb){
    if(_remain<pcb->memory)
        return false;
    int i=0,remain0;
    bool flag=false;
    for(;i<size-1;i++){
        remain0=list[i+1].start-list[i].end-1;
        if(remain0>=pcb->memory){
            flag=true;
        }
    }
    if(flag){
        for(int j=size;j>i;j--){
            list[j]=list[j-1];
        }
        list[i]={list[i-1].end+1,list[i-1].end+1+pcb->memory,pcb->pid};
        size++;
    }else{
        if(_length-list[i].end<pcb->memory)
            tight();
        list[size++]={list[i].end+1,list[i].end+1+pcb->memory,pcb->pid};
    }
    _remain-=pcb->memory;
    return true;
}

bool Memory_cl::canLoadPCB(PCB *pcb){
    return pcb->memory<=_remain;
}

bool Memory_cl::deletePCB(PCB *pcb){
    if(pcb==NULL)
        return false;
    int i=0;
    for(;i<size;i++){
        if(list[i].pid==pcb->pid)
            break;
    }
    if(i<size){
        for(;i<size;i++){
            list[i]=list[i+1];
        }
        size--;

        _remain+=pcb->memory;
        return true;
    }
    return false;
}

void Memory_cl::tight(){
    int last=0;
    for(int i=0;i<size;i++){
        if(last!=list[i].start){
            list[i].end=list[i].end-list[i].start+last;
            list[i].start=last;
        }
        last=list[i].end+1;
    }
}
