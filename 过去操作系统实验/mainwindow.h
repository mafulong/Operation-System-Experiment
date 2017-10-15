#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include"qtimer.h"
#include"QTableWidgetItem"
#include"mem.h"
#include<iostream>
#include<queue>
#include<vector>
#include<algorithm>
#define TASK_RESERVE   0 //后备状态
#define TASK_READY       1 //就绪状态
#define TASK_RUNNING   2 //运行状态
#define TASK_WAITE       3 //阻塞状态
#define TASK_HANG       4 //挂起状态
#define TASK_OVER       5 //finished状态
#define DEFAULT_TIME_SLICE  10
#define MAX_MEM 30
using namespace std;

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QTimer *t1;
    QTimer *t2;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void updateReady();
    void updateReserve();
    void updateHang();
    int selectPid;//用于控件的获取pid
    int scheWay;
    vector<PCB> ready;
    vector<PCB> reserve;
    vector<PCB> hang;
    PCB run;
    mem memo;

private slots:
    void on_pushButtonNewPcb_clicked();//建立PCB

    void on_pushButtonSche_clicked();//响应开始调度按钮
    void pcbsche();
    int getPid(const QModelIndex &index);
    void UpdateProcessBar();
    void on_tableWidget_Ready_clicked(const QModelIndex &index);

    void on_pushButtonHang_clicked();

    void on_pushButtonWakeUp_clicked();

    void on_tableWidget_Hang_clicked(const QModelIndex &index);

    void on_pushButtonSchePause_clicked();

    void on_pushButtonSrand_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
