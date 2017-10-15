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
    pcb_model->setHorizontalHeaderItem(4, new QStandardItem(QObject::tr("state")));
    ui->reserve->setModel(pcb_model);

    pcb_running->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("pid")));
    pcb_running->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("Name")));
    pcb_running->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("time")));
    pcb_running->setHorizontalHeaderItem(3, new QStandardItem(QObject::tr("priority")));
    pcb_running->setHorizontalHeaderItem(4, new QStandardItem(QObject::tr("state")));
    ui->ready->setModel(pcb_running);

    pcb_suspend->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("pid")));
    pcb_suspend->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("Name")));
    pcb_suspend->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("time")));
    pcb_suspend->setHorizontalHeaderItem(3, new QStandardItem(QObject::tr("priority")));
    pcb_suspend->setHorizontalHeaderItem(4, new QStandardItem(QObject::tr("state")));
    ui->suspend->setModel(pcb_suspend);

    /*
    ui->reserve->horizontalHeader()->setResizeMode(0, QHeaderView::Fixed);
    ui->reserve->horizontalHeader()->setResizeMode(1, QHeaderView::Fixed);
    ui->reserve->horizontalHeader()->setResizeMode(2, QHeaderView::Fixed);
    ui->reserve->horizontalHeader()->setResizeMode(3, QHeaderView::Fixed);
    */

    ui->reserve->setColumnWidth(0,100);
    ui->reserve->setColumnWidth(1,100);
    ui->reserve->setColumnWidth(2,100);
    ui->reserve->setColumnWidth(3,100);
    ui->reserve->setColumnWidth(4,100);
    //利用setModel()方法将数据模型与QTableView绑定

    selectScheduling(priority);
    selectRunning(priority);

    //以时间为种子
    srand( (unsigned)time( NULL ) );

    thread=new Thread(this);
}

MainWindow::~MainWindow()
{
    delete ui;
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

PCB* MainWindow::selectRunningByPriority(){
    PCB* result=ready;
    if(result==NULL){
        //就绪队列无可以调度的进程，直接作业调度
        result=(this->*selectProcess)();
        if(result==NULL)
            return NULL;
        else
            result->state=1;
    }
    deleteReady(result);
    result->pri--;
    result->time--;
    if(result->time>0)
        addReady(result);
    if(count_ready<maxLengthOfPCBs){
        if(pre!=NULL)
            pre->state=1;
        addReady(deletePCB(pre));
    }else if(count_ready>maxLengthOfPCBs){
        PCB* p0=deleteReady(rear_ready);
        addSuspend(p0);
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
        else
            result->state=1;
    }
    deleteReady(result);
    result->pri--;
    result->time--;
    if(result->time>0)
        addLastReady(result);
    if(count_ready<maxLengthOfPCBs){
        if(pre!=NULL)
            pre->state=1;
        addLastReady(deletePCB(pre));
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
    if(pre==NULL){
        pre=pcb;
        rear_pre=pcb;
        pcb->pre=NULL;
        pcb->next=NULL;
    }else{
        if(rear_pre->pri>=pcb->pri){
            rear_pre->next=pcb;
            pcb->pre=rear_pre;
            pcb->next=NULL;
            rear_pre=pcb;
        }else if(pcb->pri>pre->pri){
            pcb->next=pre;
            pcb->pre=NULL;
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

PCB* MainWindow::addPCB(QString& name,QString& time,QString& pri){
    bool ok;
    int int_time=time.toInt(&ok,10);
    if(!ok)
        return NULL;
    int int_pri=pri.toInt(&ok,10);
    if(!ok)
        return NULL;
    PCB* pcb=(PCB*)malloc(sizeof(PCB));
    if(pcb==NULL)
        return NULL;

    pcb->name=name.toStdString();
    pcb->time=int_time;
    pcb->pri=int_pri;
    pcb->state=0;
    pcb->pid=getPid();
    addPCB(pcb);
    return pcb;
}

PCB* MainWindow::addPCB(std::string name,int time,int pri){
    PCB* p=NULL;
    p=(PCB*)malloc(sizeof(PCB));
    if(p==NULL)
        return NULL;

    p->name=name;
    p->time=time;
    p->pri=pri;
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
    if(count_ready>=maxLengthOfPCBs)
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

    count_ready--;
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

    PCB* pcb=addPCB(name,time,pri);

    if(!pcb)
        return;

    //设置更新显示的表
    pcb_model->setItem(count_pre-1, 0, new QStandardItem(QString::number(pcb->pid,10)));
    pcb_model->setItem(count_pre-1, 1, new QStandardItem(name));
    pcb_model->setItem(count_pre-1, 2, new QStandardItem(time));
    pcb_model->setItem(count_pre-1, 3, new QStandardItem(pri));
    pcb_model->setItem(count_pre-1, 4, new QStandardItem(QString::fromStdString(getStateString(pcb->state))));
}

void MainWindow::updateReserveList(){
    PCB* tmp=pre;
    int index=0;
    pcb_model->clear();
    pcb_model->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("pid")));
    pcb_model->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("Name")));
    pcb_model->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("time")));
    pcb_model->setHorizontalHeaderItem(3, new QStandardItem(QObject::tr("priority")));
    pcb_model->setHorizontalHeaderItem(4, new QStandardItem(QObject::tr("state")));
    while(tmp){
        pcb_model->setItem(index, 0, new QStandardItem(QString::number(tmp->pid,10)));
        pcb_model->setItem(index, 1, new QStandardItem(QString::fromStdString(tmp->name)));
        pcb_model->setItem(index, 2, new QStandardItem(QString::number(tmp->time,10)));
        pcb_model->setItem(index, 3, new QStandardItem(QString::number(tmp->pri,10)));
        pcb_model->setItem(index, 4, new QStandardItem(QString::fromStdString(getStateString(tmp->state))));
        index++;
        tmp=tmp->next;
    }
}

void MainWindow::updateReadyList(){
    PCB* tmp=ready;
    int index=0;
    pcb_running->clear();
    pcb_running->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("pid")));
    pcb_running->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("Name")));
    pcb_running->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("time")));
    pcb_running->setHorizontalHeaderItem(3, new QStandardItem(QObject::tr("priority")));
    pcb_running->setHorizontalHeaderItem(4, new QStandardItem(QObject::tr("state")));
    while(tmp){
        pcb_running->setItem(index, 0, new QStandardItem(QString::number(tmp->pid,10)));
        pcb_running->setItem(index, 1, new QStandardItem(QString::fromStdString(tmp->name)));
        pcb_running->setItem(index, 2, new QStandardItem(QString::number(tmp->time,10)));
        pcb_running->setItem(index, 3, new QStandardItem(QString::number(tmp->pri,10)));
        pcb_running->setItem(index, 4, new QStandardItem(QString::fromStdString(getStateString(tmp->state))));
        index++;
        tmp=tmp->next;
    }
}

void MainWindow::updateSuspendList(){
    PCB* tmp=suspend;
    int index=0;
    pcb_suspend->clear();
    pcb_suspend->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("pid")));
    pcb_suspend->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("Name")));
    pcb_suspend->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("time")));
    pcb_suspend->setHorizontalHeaderItem(3, new QStandardItem(QObject::tr("priority")));
    pcb_suspend->setHorizontalHeaderItem(4, new QStandardItem(QObject::tr("state")));
    while(tmp){
        if(tmp->state!=3)
            tmp->state=3;
        pcb_suspend->setItem(index, 0, new QStandardItem(QString::number(tmp->pid,10)));
        pcb_suspend->setItem(index, 1, new QStandardItem(QString::fromStdString(tmp->name)));
        pcb_suspend->setItem(index, 2, new QStandardItem(QString::number(tmp->time,10)));
        pcb_suspend->setItem(index, 3, new QStandardItem(QString::number(tmp->pri,10)));
        pcb_suspend->setItem(index, 4, new QStandardItem(QString::fromStdString(getStateString(tmp->state))));
        index++;
        tmp=tmp->next;
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
                addSuspend(selectPCB);
                updateSuspendList();
                updateReadyList();
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
            awakePCB->state=3;
            if(awakePCB->time>0)
                addSuspend(awakePCB);
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
