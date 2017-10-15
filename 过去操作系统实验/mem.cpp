#include "mem.h"
#include"algorithm"
#include<iostream>
using namespace std;
mem::mem()
{
    sum=0;
    mymem.clear();
    mymemory a(0,memSize,0);
    mymem.push_back(a);
}
void mem::remove(int memoryStart){
    for(vector<mymemory>::iterator it=mymem.begin();it!=mymem.end();it++){
        if((*it).front==memoryStart&&(*it).state==1){
            (*it).state=0;
            sum-=(*it).length;
            break;
        }
    }

    tight();

}
bool mem::add(PCB &a){
    if(a.memoryNeeds>100)
        return false;
    for(vector<mymemory>::iterator it=mymem.begin();it!=mymem.end();it++){
        if((*it).state==0&&(*it).length>=a.memoryNeeds){
            sum+=a.memoryNeeds;
            mymemory an((*it).front+a.memoryNeeds,(*it).length-a.memoryNeeds,0);//生成空内存分区
            (*it).length=a.memoryNeeds;
            (*it).state=1;
            a.memoryStart=(*it).front;
            mymem.push_back(an);
            tight();
            return true;
        }
    }
    cout<<"false"<<endl;
    return false;
}
void mem::tight(){
    sort(mymem.begin(),mymem.end());
    vector<mymemory>::iterator it=mymem.begin()+1;
    while(it!=mymem.end()){
        if((*it).state==0&&(*(it-1)).state==0){
            (*(it-1)).length+=(*it).length;
            it=mymem.erase(it);
        }else{
            it++;
        }
    }

}
