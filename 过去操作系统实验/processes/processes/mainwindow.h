#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QTableView>
#include <QMutex>

#include <time.h>
#include <stdlib.h>

#include "pcb.h"

namespace Ui {
class MainWindow;
class Thread_cl;
}

typedef class Thread_cl Thread;

#ifndef THREAD_H
#include "thread.h"
#endif

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    friend class Thread_cl;

private slots:

    void on_addProcess_clicked();

    void on_start_clicked();

    void on_exit_clicked();

    void on_pause_clicked();

    void on_reserve_clicked(const QModelIndex &index);

    void on_ready_clicked(const QModelIndex &index);

    void on_suspend_2_clicked();

    void on_awake_clicked();

    void on_suspend_clicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    Thread *thread;
    QStandardItemModel *pcb_model = new QStandardItemModel();
    QStandardItemModel *pcb_running = new QStandardItemModel();
    QStandardItemModel *pcb_suspend = new QStandardItemModel();
    //后备队列
    PCB *pre=NULL;//头指针
    PCB *rear_pre=NULL;//尾指针
    int count_pre=0;//计数器

    //就绪队列
    PCB *ready=NULL;//头指针
    PCB *rear_ready=NULL;//尾指针
    int count_ready=0;//计数器

    //挂起队列
    PCB *suspend=NULL;//头指针
    PCB *rear_suspend=NULL;//尾指针
    int count_suspend=0;//计数器

    //调度类型
    scheduling sch=def;
    scheduling run_sch=def;

    //最大进程数
    int maxLengthOfPCBs=5;


    //作业调度算法的代理
    //使用的时候直接使用(this->*selectProcess)()就可以进行作业调度获得PCB
    //注意如果没有可以调度的返回值是NULL
    PCB* (MainWindow::*selectProcess)()=NULL;

    //进程调度算法的代理
    //使用的时候直接使用(this->*private_selectRunningProcess)()就可以进行进程调度获得PCB
    //注意如果没有可以调度的返回值是NULL
    PCB* (MainWindow::*private_selectRunningProcess)()=NULL;
    PCB* selectRunningProcess();

    PCB* selectByPriority();
    PCB* selectFirst();

    PCB* selectRunningByPriority();
    PCB* selectRunningFirst();

    void selectScheduling(scheduling sch);
    void selectRunning(scheduling sch);

    //带权值的插入后备队列
    void addPCB(PCB *pcb);
    //插入后备队列队尾
    void addLastPCB(PCB *pcb);
    PCB* addPCB(QString& name,QString& time,QString& pri);
    PCB* addPCB(std::string name,int time,int pri);
    //从后备队列摘下来
    PCB* deletePCB(PCB *pcb);

    PCB* selectPCB=NULL;
    PCB* awakePCB=NULL;

    //带权值的插入就绪队列
    bool addReady(PCB *pcb);
    //插入就绪队列尾
    bool addLastReady(PCB *pcb);
    //从就绪队列摘下来
    PCB* deleteReady(PCB *pcb);

    PCB* findReadyPCB(long pid);

    //带权值的插入就挂起列
    void addSuspend(PCB *pcb);
    //插入挂起队列尾
    void addLastSuspend(PCB *pcb);
    //从挂起队列摘下来
    PCB* deleteSuspend(PCB *pcb);

    PCB* findSuspendPCB(long pid);

    void updateReserveList();
    void updateReadyList();
    void updateSuspendList();

    bool paused=false;

    //获得唯一识别符号pid
    long getPid();

    QMutex mutex1,mutex2,mutex3;
};

#endif // MAINWINDOW_H
