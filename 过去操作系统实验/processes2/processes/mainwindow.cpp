#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "stdlib.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    pcb_model->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("pid")));
    pcb_model->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("Name")));
    pcb_model->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("time")));
    pcb_model->setHorizontalHeaderItem(3, new QStandardItem(QObject::tr("priority")));
    pcb_model->setHorizontalHeaderItem(4, new QStandardItem(QObject::tr("memory")));
    pcb_model->setHorizontalHeaderItem(5, new QStandardItem(QObject::tr("state")));
    ui->reserve->setModel(pcb_model);

    pcb_running->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("pid")));
    pcb_running->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("Name")));
    pcb_running->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("time")));
    pcb_running->setHorizontalHeaderItem(3, new QStandardItem(QObject::tr("priority")));
    pcb_running->setHorizontalHeaderItem(4, new QStandardItem(QObject::tr("memory")));
    pcb_running->setHorizontalHeaderItem(5, new QStandardItem(QObject::tr("state")));
    ui->ready->setModel(pcb_running);

    pcb_suspend->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("pid")));
    pcb_suspend->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("Name")));
    pcb_suspend->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("time")));
    pcb_suspend->setHorizontalHeaderItem(3, new QStandardItem(QObject::tr("priority")));
    pcb_suspend->setHorizontalHeaderItem(4, new QStandardItem(QObject::tr("memory")));
    pcb_suspend->setHorizontalHeaderItem(5, new QStandardItem(QObject::tr("state")));
    ui->suspend->setModel(pcb_suspend);

    pcb_dead->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("pid")));
    pcb_dead->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("Name")));
    pcb_dead->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("time")));
    pcb_dead->setHorizontalHeaderItem(3, new QStandardItem(QObject::tr("priority")));
    pcb_dead->setHorizontalHeaderItem(4, new QStandardItem(QObject::tr("memory")));
    pcb_dead->setHorizontalHeaderItem(5, new QStandardItem(QObject::tr("state")));
    ui->dead->setModel(pcb_dead);

    /*
    ui->reserve->horizontalHeader()->setResizeMode(0, QHeaderView::Fixed);
    ui->reserve->horizontalHeader()->setResizeMode(1, QHeaderView::Fixed);
    ui->reserve->horizontalHeader()->setResizeMode(2, QHeaderView::Fixed);
    ui->reserve->horizontalHeader()->setResizeMode(3, QHeaderView::Fixed);
    */




    setReserveWidth(width);
    setReadyWidth(width);
    setSuspendWidth(width);
    setDeadWidth(width);

    //利用setModel()方法将数据模型与QTableView绑定

    selectScheduling(priority);
    selectRunning(priority);
    //selectScheduling(first);
    //selectRunning(first);

    //以时间为种子
    srand( (unsigned)time( NULL ) );

    thread=new Thread(this);

    mcb=new MCB(500);
    ui->time_2->setText("1000");
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setReserveWidth(int width){
    ui->reserve->setColumnWidth(0,width);
    ui->reserve->setColumnWidth(1,width);
    ui->reserve->setColumnWidth(2,width);
    ui->reserve->setColumnWidth(3,width);
    ui->reserve->setColumnWidth(4,width);
    ui->reserve->setColumnWidth(5,width);
}

void MainWindow::setReadyWidth(int width){
    ui->ready->setColumnWidth(0,width);
    ui->ready->setColumnWidth(1,width);
    ui->ready->setColumnWidth(2,width);
    ui->ready->setColumnWidth(3,width);
    ui->ready->setColumnWidth(4,width);
    ui->ready->setColumnWidth(5,width);
}

void MainWindow::setSuspendWidth(int width){
    ui->suspend->setColumnWidth(0,width);
    ui->suspend->setColumnWidth(1,width);
    ui->suspend->setColumnWidth(2,width);
    ui->suspend->setColumnWidth(3,width);
    ui->suspend->setColumnWidth(4,width);
    ui->suspend->setColumnWidth(5,width);
}

void MainWindow::setDeadWidth(int width){
    ui->dead->setColumnWidth(0,width);
    ui->dead->setColumnWidth(1,width);
    ui->dead->setColumnWidth(2,width);
    ui->dead->setColumnWidth(3,width);
    ui->dead->setColumnWidth(4,width);
    ui->dead->setColumnWidth(5,width);
}

PCB* MainWindow::selectByPriority(){
    PCB* result=pre;
    if(pre==NULL)
        return pre;
    deletePCB(result);
    return result;
}

PCB* MainWindow::selectFirst(){
    PCB* result=pre;
    if(pre==NULL)
        return pre;
    deletePCB(result);
    return result;
}

void MainWindow::selectScheduling(scheduling sch){
    if(sch==this->sch)
        return;
    switch (sch) {
    case priority:
        selectProcess=selectByPriority;
        break;
    case first:
        selectProcess=selectFirst;
        break;
    default:
        break;
    }
}

PCB* MainWindow::afterRunning(PCB* pcb){
    deleteReady(pcb);

    pcb->pre=NULL;
    pcb->next=NULL;
    if(pcb->time>0)
        private_addReady(pcb);
    else{
        addDead(pcb);
        pcb->state=4;
        mcb->deletePCB(pcb);
        updateDeadList();
    }
    updateReadyList();
    updateReserveList();
    return pcb;
}

PCB* MainWindow::selectRunningByPriority(){
    PCB* result=ready;
    if(result==NULL){
        //就绪队列无可以调度的进程，直接作业调度
        result=(this->*selectProcess)();
        if(result==NULL)
            return NULL;
        else{
            result->pre=NULL;
            result->next=NULL;
            bool flag=addReady(result);
            if(!flag){
                addPCB(result);
                return NULL;
            }
            result->state=1;
            updateReserveList();
        }
    }
    /*
    if(count_ready<maxLengthOfPCBs){
        if(pre!=NULL)
            pre->state=1;
        addReady(deletePCB(pre));
    }else if(count_ready>maxLengthOfPCBs){
        PCB* p0=deleteReady(rear_ready);
        addSuspend(p0);
    }*/

    PCB* pcb=(this->*selectProcess)();
    if(addReady(pcb)){
        pcb->state=1;
        updateReserveList();

    }else if(pcb){
        pcb->state=0;
        addPCB(pcb);
    }

    return result;
}

PCB* MainWindow::selectRunningFirst(){
    PCB* result=ready;
    if(ready==NULL){
        //就绪队列无可以调度的进程，直接作业调度
        result=(this->*selectProcess)();
        if(result==NULL)
            return NULL;
        else{
            bool flag=mcb->loadPCB(result);
            if(!flag)
                return NULL;
            result->state=1;
            addReady(result);
        }
    }
    deleteReady(result);
    result->pri--;
    result->time--;
    if(result->time>0)
        addLastReady(result);
    else{
        addDead(result);
        result->state=4;
        mcb->deletePCB(result);
        updateDeadList();
    }
    /*
    if(count_ready<maxLengthOfPCBs){
        if(pre!=NULL)
            pre->state=1;
        addLastReady(deletePCB(pre));
    }*/
    if(pre&&mcb->remain()>=pre->memory){
        pre->state=1;
        mcb->loadPCB(pre);
        addReady(deletePCB(pre));
    }
    return result;
}

void MainWindow::selectRunning(scheduling sch){
    if(sch==this->run_sch)
        return;
    switch (sch) {
    case priority:
        private_selectRunningProcess=selectRunningByPriority;
        break;
    case first:
        private_selectRunningProcess=selectRunningFirst;
        break;
    default:
        break;
    }
}

PCB* MainWindow::selectRunningProcess(){
    return (this->*private_selectRunningProcess)();
}

void MainWindow::addPCB(PCB *pcb){
    if(pcb==NULL)
        return;
    pcb->pre=NULL;
    pcb->next=NULL;
    if(pre==NULL){
        pre=pcb;
        rear_pre=pcb;
    }else{
        if(rear_pre->pri>=pcb->pri){
            rear_pre->next=pcb;
            pcb->pre=rear_pre;
            rear_pre=pcb;
        }else if(pcb->pri>pre->pri){
            pcb->next=pre;
            pre->pre=pcb;
            pre=pcb;
        }else{
            PCB* temp=pre;
            while(temp->next&&temp->next->pri>=pcb->pri)
                temp=temp->next;
            pcb->next=temp->next;
            pcb->next->pre=pcb;
            temp->next=pcb;
            pcb->pre=temp;
        }
    }
    count_pre++;
}

void MainWindow::addLastPCB(PCB *pcb){
    if(pcb==NULL)
        return;
    if(pre==NULL){
           pre=pcb;
           rear_pre=pcb;
           pcb->pre=NULL;
           pcb->next=NULL;
    }else{
           rear_pre->next=pcb;
           pcb->pre=rear_pre;
           pcb->next=NULL;
           rear_pre=pcb;
    }
    count_pre++;
}

PCB* MainWindow::addPCB(QString& name,QString& time,QString& pri,QString& memory){
    bool ok;
    int int_time=time.toInt(&ok,10);
    if(!ok)
        return NULL;
    int int_pri=pri.toInt(&ok,10);
    if(!ok)
        return NULL;
    int int_memory=memory.toInt(&ok,10);
    if(!ok)
        return NULL;

    PCB* pcb=(PCB*)malloc(sizeof(PCB));
    if(pcb==NULL)
        return NULL;

    pcb->name=name.toStdString();
    pcb->time=int_time;
    pcb->pri=int_pri;
    pcb->memory=int_memory;
    pcb->state=0;
    pcb->pid=getPid();
    addPCB(pcb);
    return pcb;
}

PCB* MainWindow::addPCB(std::string name,int time,int pri,int memory){
    PCB* p=NULL;
    p=(PCB*)malloc(sizeof(PCB));
    if(p==NULL)
        return NULL;

    p->name=name;
    p->time=time;
    p->pri=pri;
    p->memory=memory;
    p->state=0;
    addPCB(p);
    return p;
}



PCB* MainWindow::deletePCB(PCB *pcb){
    if(!count_pre||!pcb)
        return NULL;
    if(pcb!=pre&&pcb!=rear_pre){
        pcb->pre->next=pcb->next;
        pcb->next->pre=pcb->pre;
        count_pre--;
        return pcb;
    }
    if(pcb==pre){
        pre=pcb->next;
        if(pre!=NULL)
            pre->pre=NULL;
    }
    if(pcb==rear_pre){
        rear_pre=pcb->pre;
        if(rear_pre!=NULL)
            rear_pre->next=NULL;
    }

    count_pre--;
    return pcb;
}

bool MainWindow::addReady(PCB *pcb){
    if(pcb==NULL)
        return false;
    /*
    if(count_ready>=maxLengthOfPCBs)
        return false;*/
    if(mcb->remain()<pcb->memory)
        return false;
    mcb->loadPCB(pcb);
    if(ready==NULL){
        ready=pcb;
        rear_ready=pcb;
        pcb->pre=NULL;
        pcb->next=NULL;
    }else{
        if(rear_ready->pri>=pcb->pri){
            rear_ready->next=pcb;
            pcb->pre=rear_ready;
            pcb->next=NULL;
            rear_ready=pcb;
        }else if(pcb->pri>ready->pri){
            pcb->next=ready;
            pcb->pre=NULL;
            ready->pre=pcb;
            ready=pcb;
        }else{
            PCB* temp=ready;
            while(temp->next&&temp->next->pri>=pcb->pri)
                temp=temp->next;
            pcb->next=temp->next;
            if(pcb->next!=NULL)
                pcb->next->pre=pcb;
            temp->next=pcb;
            pcb->pre=temp;
        }
    }
    count_ready++;
    return true;
}

bool MainWindow::private_addReady(PCB *pcb){
    if(pcb==NULL)
        return false;

    if(ready==NULL){
        ready=pcb;
        rear_ready=pcb;
        pcb->pre=NULL;
        pcb->next=NULL;
    }else{
        if(rear_ready->pri>=pcb->pri){
            rear_ready->next=pcb;
            pcb->pre=rear_ready;
            pcb->next=NULL;
            rear_ready=pcb;
        }else if(pcb->pri>ready->pri){
            pcb->next=ready;
            pcb->pre=NULL;
            ready->pre=pcb;
            ready=pcb;
        }else{
            PCB* temp=ready;
            while(temp->next&&temp->next->pri>=pcb->pri)
                temp=temp->next;
            pcb->next=temp->next;
            if(pcb->next!=NULL)
                pcb->next->pre=pcb;
            temp->next=pcb;
            pcb->pre=temp;
        }
    }
    count_ready++;
    return true;
}

bool MainWindow::addLastReady(PCB *pcb){
    if(pcb==NULL)
        return false;
    if(count_ready>=maxLengthOfPCBs)
        return false;
    if(ready==NULL){
        ready=pcb;
        rear_ready=pcb;
        pcb->pre=NULL;
        pcb->next=NULL;
    }else{
        rear_ready->next=pcb;
        pcb->pre=rear_ready;
        pcb->next=NULL;
        rear_ready=pcb;
    }
    count_ready++;
    return true;
}

PCB* MainWindow::deleteReady(PCB *pcb){
    if(!count_ready||!pcb)
        return NULL;
    if(pcb!=ready&&pcb!=rear_ready){
        if(pcb->pre!=NULL)
            pcb->pre->next=pcb->next;
        if(pcb->next!=NULL)
            pcb->next->pre=pcb->pre;
        count_ready--;
        return pcb;
    }
    if(pcb==ready){
        ready=pcb->next;
        if(ready!=NULL)
            ready->pre=NULL;
    }
    if(pcb==rear_ready){
        rear_ready=pcb->pre;
        if(rear_ready!=NULL)
            rear_ready->next=NULL;
    }

    count_ready--;
    return pcb;
}

PCB* MainWindow::findReadyPCB(long pid){
    PCB* tmp=ready;
    while(tmp){
        if(tmp->pid==pid)
            return tmp;
        tmp=tmp->next;
    }
    return NULL;
}

void MainWindow::addSuspend(PCB *pcb){
    if(pcb==NULL)
        return;
    if(suspend==NULL){
        suspend=pcb;
        rear_suspend=pcb;
        pcb->pre=NULL;
        pcb->next=NULL;
    }else{
        if(rear_suspend->pri>=pcb->pri){
            rear_suspend->next=pcb;
            pcb->pre=rear_suspend;
            pcb->next=NULL;
            rear_suspend=pcb;
        }else if(pcb->pri>suspend->pri){
            pcb->next=suspend;
            pcb->pre=NULL;
            suspend->pre=pcb;
            suspend=pcb;
        }else{
            PCB* temp=suspend;
            while(temp->next&&temp->next->pri>=pcb->pri)
                temp=temp->next;
            pcb->next=temp->next;
            if(pcb->next!=NULL)
                pcb->next->pre=pcb;
            temp->next=pcb;
            pcb->pre=temp;
        }

    }
    count_suspend++;
}

void MainWindow::addLastSuspend(PCB *pcb){
    if(pcb==NULL)
        return;
    if(suspend==NULL){
        suspend=pcb;
        rear_suspend=pcb;
        pcb->pre=NULL;
        pcb->next=NULL;
    }else{
        rear_suspend->next=pcb;
        pcb->pre=rear_suspend;
        pcb->next=NULL;
        rear_suspend=pcb;
    }
    count_suspend++;
}

PCB* MainWindow::deleteSuspend(PCB *pcb){
    if(!count_suspend||!pcb)
        return NULL;
    if(pcb!=suspend&&pcb!=rear_suspend){
        if(pcb->pre!=NULL)
            pcb->pre->next=pcb->next;
        if(pcb->next!=NULL)
            pcb->next->pre=pcb->pre;
        count_suspend--;
        return pcb;
    }
    if(pcb==suspend){
        suspend=pcb->next;
        if(suspend!=NULL)
            suspend->pre=NULL;
    }
    if(pcb==rear_suspend){
        rear_suspend=pcb->pre;
        if(rear_suspend!=NULL)
            rear_suspend->next=NULL;
    }

    count_suspend--;
    return pcb;
}

PCB* MainWindow::findSuspendPCB(long pid){
    PCB* tmp=suspend;
    while(tmp){
        if(tmp->pid==pid)
            return tmp;
        tmp=tmp->next;
    }
    return NULL;
}

void MainWindow::addDead(PCB *pcb){
    if(pcb==NULL)
        return;
    pcb->pre=NULL;
    pcb->next=NULL;
    if(dead==NULL){
        dead=pcb;
        rear_dead=pcb;
    }else{
        if(rear_dead->pri>=pcb->pri){
            rear_dead->next=pcb;
            pcb->pre=rear_dead;
            rear_dead=pcb;
        }else if(pcb->pri>dead->pri){
            pcb->next=dead;
            dead->pre=pcb;
            dead=pcb;
        }else{
            PCB* temp=dead;
            while(temp->next&&temp->next->pri>=pcb->pri)
                temp=temp->next;
            pcb->next=temp->next;
            if(pcb->next!=NULL)
                pcb->next->pre=pcb;
            temp->next=pcb;
            pcb->pre=temp;
        }

    }
    count_dead++;
}

void MainWindow::addLastDead(PCB *pcb){
    if(pcb==NULL)
        return;
    if(dead==NULL){
        dead=pcb;
        rear_dead=pcb;
        pcb->pre=NULL;
        pcb->next=NULL;
    }else{
        rear_dead->next=pcb;
        pcb->pre=rear_dead;
        pcb->next=NULL;
        rear_dead=pcb;
    }
    count_dead++;
}

PCB* MainWindow::deleteDead(PCB *pcb){
    if(!count_dead||!pcb)
        return NULL;
    if(pcb!=dead&&pcb!=rear_dead){
        if(pcb->pre!=NULL)
            pcb->pre->next=pcb->next;
        if(pcb->next!=NULL)
            pcb->next->pre=pcb->pre;
        count_dead--;
        return pcb;
    }
    if(pcb==dead){
        dead=pcb->next;
        if(dead!=NULL)
            dead->pre=NULL;
    }
    if(pcb==rear_dead){
        rear_dead=pcb->pre;
        if(rear_dead!=NULL)
            rear_dead->next=NULL;
    }

    count_dead--;
    return pcb;
}

PCB* MainWindow::findDeadPCB(long pid){
    PCB* tmp=dead;
    while(tmp){
        if(tmp->pid==pid)
            return tmp;
        tmp=tmp->next;
    }
    return NULL;
}

long MainWindow::getPid(){
rerand:    long pid=rand();
    if(pid<0)
        pid=-pid;
    PCB* pcb=pre;
    while(pcb){
        if(pcb->pid==pid)
            goto rerand;
        pcb=pcb->next;
    }
    pcb=ready;
    while(pcb){
        if(pcb->pid==pid)
            goto rerand;
        pcb=pcb->next;
    }
    pcb=suspend;
    while(pcb){
        if(pcb->pid==pid)
            goto rerand;
        pcb=pcb->next;
    }
    return pid;
}

void MainWindow::on_addProcess_clicked()
{

    QString name=ui->name->text();
    QString time=ui->time->text();
    QString pri=ui->priority->text();
    QString memory=ui->memory->text();

    PCB* pcb=addPCB(name,time,pri,memory);

    if(!pcb)
        return;

    //设置更新显示的表
    pcb_model->setItem(count_pre-1, 0, new QStandardItem(QString::number(pcb->pid,10)));
    pcb_model->setItem(count_pre-1, 1, new QStandardItem(name));
    pcb_model->setItem(count_pre-1, 2, new QStandardItem(time));
    pcb_model->setItem(count_pre-1, 3, new QStandardItem(pri));
    pcb_model->setItem(count_pre-1, 4, new QStandardItem(QString::number(pcb->memory,10)));
    pcb_model->setItem(count_pre-1, 5, new QStandardItem(QString::fromStdString(getStateString(pcb->state))));
}

void MainWindow::updateReserveList(){
    PCB* tmp=pre;
    int index=0;

    /*
    pcb_model->clear();
    pcb_model->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("pid")));
    pcb_model->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("Name")));
    pcb_model->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("time")));
    pcb_model->setHorizontalHeaderItem(3, new QStandardItem(QObject::tr("priority")));
    pcb_model->setHorizontalHeaderItem(4, new QStandardItem(QObject::tr("memory")));
    pcb_model->setHorizontalHeaderItem(5, new QStandardItem(QObject::tr("state")));
*/
    pcb_model->removeRows(0,pcb_model->rowCount());
    while(tmp){
        pcb_model->setItem(index, 0, new QStandardItem(QString::number(tmp->pid,10)));
        pcb_model->setItem(index, 1, new QStandardItem(QString::fromStdString(tmp->name)));
        pcb_model->setItem(index, 2, new QStandardItem(QString::number(tmp->time,10)));
        pcb_model->setItem(index, 3, new QStandardItem(QString::number(tmp->pri,10)));
        pcb_model->setItem(index, 4, new QStandardItem(QString::number(tmp->memory,10)));
        pcb_model->setItem(index, 5, new QStandardItem(QString::fromStdString(getStateString(tmp->state))));
        index++;
        tmp=tmp->next;
    }
    //setReserveWidth(width);
}

void MainWindow::updateReadyList(){
    PCB* tmp=ready;
    int index=0;

    /*
    pcb_running->clear();
    pcb_running->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("pid")));
    pcb_running->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("Name")));
    pcb_running->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("time")));
    pcb_running->setHorizontalHeaderItem(3, new QStandardItem(QObject::tr("priority")));
    pcb_running->setHorizontalHeaderItem(4, new QStandardItem(QObject::tr("memory")));
    pcb_running->setHorizontalHeaderItem(5, new QStandardItem(QObject::tr("state")));
*/
    pcb_running->removeRows(0,pcb_running->rowCount());
    while(tmp){
        pcb_running->setItem(index, 0, new QStandardItem(QString::number(tmp->pid,10)));
        pcb_running->setItem(index, 1, new QStandardItem(QString::fromStdString(tmp->name)));
        pcb_running->setItem(index, 2, new QStandardItem(QString::number(tmp->time,10)));
        pcb_running->setItem(index, 3, new QStandardItem(QString::number(tmp->pri,10)));
        pcb_running->setItem(index, 4, new QStandardItem(QString::number(tmp->memory,10)));
        pcb_running->setItem(index, 5, new QStandardItem(QString::fromStdString(getStateString(tmp->state))));
        index++;
        tmp=tmp->next;
    }
    //setReadyWidth(width);
}

void MainWindow::updateSuspendList(){
    PCB* tmp=suspend;
    int index=0;

    /*
    pcb_suspend->clear();
    pcb_suspend->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("pid")));
    pcb_suspend->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("Name")));
    pcb_suspend->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("time")));
    pcb_suspend->setHorizontalHeaderItem(3, new QStandardItem(QObject::tr("priority")));
    pcb_suspend->setHorizontalHeaderItem(4, new QStandardItem(QObject::tr("memory")));
    pcb_suspend->setHorizontalHeaderItem(5, new QStandardItem(QObject::tr("state")));
*/

    pcb_suspend->removeRows(0,pcb_suspend->rowCount());
    while(tmp){
        if(tmp->state!=3)
            tmp->state=3;
        pcb_suspend->setItem(index, 0, new QStandardItem(QString::number(tmp->pid,10)));
        pcb_suspend->setItem(index, 1, new QStandardItem(QString::fromStdString(tmp->name)));
        pcb_suspend->setItem(index, 2, new QStandardItem(QString::number(tmp->time,10)));
        pcb_suspend->setItem(index, 3, new QStandardItem(QString::number(tmp->pri,10)));
        pcb_suspend->setItem(index, 4, new QStandardItem(QString::number(tmp->memory,10)));
        pcb_suspend->setItem(index, 5, new QStandardItem(QString::fromStdString(getStateString(tmp->state))));
        index++;
        tmp=tmp->next;
    }

    //setSuspendWidth(width);
}

void MainWindow::updateDeadList(){
    PCB* tmp=dead;
    int index=0;

    /*
    pcb_dead->clear();
    pcb_dead->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("pid")));
    pcb_dead->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("Name")));
    pcb_dead->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("time")));
    pcb_dead->setHorizontalHeaderItem(3, new QStandardItem(QObject::tr("priority")));
    pcb_dead->setHorizontalHeaderItem(4, new QStandardItem(QObject::tr("memory")));
    pcb_dead->setHorizontalHeaderItem(5, new QStandardItem(QObject::tr("state")));
*/
    pcb_dead->removeRows(0,pcb_dead->rowCount());
    while(tmp){
        if(tmp->state!=4)
            tmp->state=4;
        pcb_dead->setItem(index, 0, new QStandardItem(QString::number(tmp->pid,10)));
        pcb_dead->setItem(index, 1, new QStandardItem(QString::fromStdString(tmp->name)));
        pcb_dead->setItem(index, 2, new QStandardItem(QString::number(tmp->time,10)));
        pcb_dead->setItem(index, 3, new QStandardItem(QString::number(tmp->pri,10)));
        pcb_dead->setItem(index, 4, new QStandardItem(QString::number(tmp->memory,10)));
        pcb_dead->setItem(index, 5, new QStandardItem(QString::fromStdString(getStateString(tmp->state))));
        index++;
        tmp=tmp->next;
    }

    //setDeadWidth(width);
}

void MainWindow::updateProcessRunningTime(long pid,int time){
    PCB* pcb=ready;
    while(pcb){
        if(pcb->pid==pid){
            pcb->time=time;
            return;
        }
        pcb=pcb->next;
    }
    pcb=pre;
    while(pcb){
        if(pcb->pid==pid){
            pcb->time=time;
            return;
        }
        pcb=pcb->next;
    }
    pcb=suspend;
    while(pcb){
        if(pcb->pid==pid){
            pcb->time=time;
            return;
        }
        pcb=pcb->next;
    }
}

void MainWindow::on_start_clicked()
{

    if(!thread->isStart()){
       thread->start();
    }
}

void MainWindow::on_exit_clicked()
{
    QApplication::quit();
}

void MainWindow::on_pause_clicked()
{
    if(!paused){
        thread->pause();
        ui->pause->setText("继续");
        paused=true;
    }else{
        thread->continueRun();
        ui->pause->setText("暂停");
        paused=false;
    }
}

void MainWindow::on_reserve_clicked(const QModelIndex &index)
{
    int row=index.row();
    QModelIndex i = pcb_model->index(row,0);
    QVariant data = pcb_model->data(i);
    long pid=data.toString().toLong();
    std::cout<<pid<<std::endl;
}

void MainWindow::on_ready_clicked(const QModelIndex &index)
{
    int row=index.row();
    QModelIndex i = pcb_running->index(row,0);
    QVariant data = pcb_running->data(i);
    long pid=data.toString().toLong();
    selectPCB=findReadyPCB(pid);
    std::cout<<pid<<std::endl;
}

void MainWindow::on_suspend_2_clicked()
{
    if(selectPCB!=NULL){
        if(selectPCB->time>0){
            selectPCB=deleteReady(selectPCB);
            if(selectPCB!=NULL){
                selectPCB->pre=NULL;
                selectPCB->next=NULL;
                selectPCB->state=3;
                mcb->deletePCB(selectPCB);
                addSuspend(selectPCB);
                updateSuspendList();
                updateReadyList();
                updateDeadList();
            }
        }
    }
}

void MainWindow::on_awake_clicked()
{
    if(awakePCB!=NULL){

        deleteSuspend(awakePCB);
        awakePCB->pre=NULL;
        awakePCB->next=NULL;
        awakePCB->state=1;
        bool flag=addReady(awakePCB);
        if(!flag){
            if(awakePCB->time>0){
                //mcb->deletePCB(awakePCB);
                addSuspend(awakePCB);
                awakePCB->state=3;
            }
            else{
                addDead(awakePCB);
                awakePCB->state=4;
            }
            return;
        }

        updateSuspendList();
        updateReadyList();
    }
}

void MainWindow::on_suspend_clicked(const QModelIndex &index)
{
    int row=index.row();
    QModelIndex i = pcb_suspend->index(row,0);
    QVariant data = pcb_suspend->data(i);
    long pid=data.toString().toLong();
    awakePCB=findSuspendPCB(pid);
    std::cout<<pid<<std::endl;
}

void MainWindow::on_pushButton_clicked()
{
    QString time=ui->time_2->text();
    thread->setTime(time.toLong());
}
