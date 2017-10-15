#include "thread.h"

Thread_cl::Thread_cl(class MainWindow* win):win(win)
{
    stopped=false;
    started=false;
    paused=false;
}

void Thread_cl::stop(){
    stopped=true;
}

bool Thread_cl::isStart(){
    return started;
}

void Thread_cl::start(){
    started=true;
    QThread::start();
}

bool Thread_cl::isStopped(){
    return stopped;
}

void Thread_cl::pause(){
    paused=true;
}

void Thread_cl::continueRun(){
    paused=false;
}

void Thread_cl::setTime(long time){
    if(time!=sleepTime)
        sleepTime=time;
}

void Thread_cl::run(){
    while(!stopped){
        if(!paused){
            msleep(sleepTime);
            if(lastPCB!=NULL){
                lastPCB->pri--;
                lastPCB->time--;
                lastPCB->state=1;
                win->afterRunning(lastPCB);
                win->updateReserveList();
            }
            PCB* pcb=win->selectRunningProcess();
            if(pcb==NULL){
                win->updateReadyList();
                continue;
            }
            pcb->state=2;
            lastPCB=pcb;
            win->updateReadyList();

            /*
            int index=0;
            PCB *tmp=win->ready,*tmp2=NULL;
            while(tmp&&index++<win->maxLengthOfPCBs){
                tmp=tmp->next;
            }
            while(tmp){
                tmp2=tmp;
                tmp=tmp->next;
                win->deleteReady(tmp2);
                win->addSuspend(tmp2);
                tmp2->state=3;
            }
            win->count_ready=index;*/
        }else{
            msleep(2000);
        }

    }
}
