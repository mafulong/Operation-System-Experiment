#include"mainwindow.h"
#include "ui_mainwindow.h"
#include<QMessageBox>
#include "qpainter.h"
#include <QPainter>
#define random(a,b) (rand()%(b-a+1)+a)//a-b的随机数
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tableWidget_Ready->setColumnCount(4);
    ui->tableWidget_Ready->setRowCount(6);
    ui->tableWidgetRun->setColumnCount(4);
    ui->tableWidgetRun->setRowCount(1);
    ui->tableWidget_Hang->setColumnCount(4);
    ui->tableWidget_Hang->setRowCount(6);
    ui->tableWidget_Reserve->setColumnCount(4);
    ui->tableWidget_Reserve->setRowCount(50);
    QStringList header;
    header<<"PID"<<"Priority"<<"Time"<<"Memory"<<"Memory";
    ui->tableWidget_Hang->setHorizontalHeaderLabels(header);
    ui->tableWidgetRun->setHorizontalHeaderLabels(header);
    ui->tableWidget_Ready->setHorizontalHeaderLabels(header);
    ui->tableWidget_Reserve->setHorizontalHeaderLabels(header);
    connect(ui->tableWidget_Ready,SIGNAL(clicked(QModelIndex)),this,SLOT(getPid(QModelIndex)));
    connect(ui->tableWidget_Hang,SIGNAL(clicked(QModelIndex)),this,SLOT(getPid(QModelIndex)));

}
//更新进度条
void MainWindow::UpdateProcessBar(){
    if(ui->progressBar->value()==100){
        ui->progressBar->setValue(0);
    }
    ui->progressBar->setValue(ui->progressBar->value()+10);
}

MainWindow::~MainWindow()
{
    delete ui;
}
//CPU调度
void MainWindow::pcbsche(){
    if(reserve.size()==0&&ready.size()==0){
        t1->stop();
        t2->stop();
        ui->progressBar->setValue(0);
        ui->tableWidgetRun->clear();
        QMessageBox::about(NULL, "About", "Please add a PCB");

    }
    else{
        for(vector<PCB>::iterator it=reserve.begin();it!=reserve.end();)
        {
            //当道数小于6时,从后备队列取出PCB添加到就绪队列，直到道数为6或者后备队列为空
             if(ready.size()+hang.size()>=6)
                 break;

             if(memo.add(*it)){//if add sucessfully可以容纳
                 ui->textBrowserResult->append(QString::number((*it).pid)+"  已进入就绪队列!");
                 ready.insert(ready.begin(),*it);
                 it=reserve.erase(it);
                 ui->progressBarMemory->setValue(memo.sum);
             }
             else{
                 ui->textBrowserResult->append(QString::number((*it).pid)+"  所需内存过大，进入就绪队列失败!");
                 it++;
             }


         }
         if(ready.size()==0){
             t1->stop();
             t2->stop();
             ui->progressBar->setValue(0);
             ui->tableWidgetRun->clear();
             QMessageBox::about(NULL, "About", "就绪队列无进程！");
             return;
         }
         updateReserve();
         if(scheWay==0){
            sort(ready.begin(),ready.end());
         }
         run=ready.back();
         ready.pop_back();
         ui->textBrowserResult->append(QString::number(run.pid)+" 正在运行");
         ui->tableWidgetRun->clear();
         ui->tableWidgetRun->setItem(0,0,new QTableWidgetItem(QString::number(run.pid)));
         ui->tableWidgetRun->setItem(0,1,new QTableWidgetItem(QString::number(run.priority)));
         ui->tableWidgetRun->setItem(0,2,new QTableWidgetItem(QString::number(run.time)));
         ui->tableWidgetRun->setItem(0,3,new QTableWidgetItem(QString::number(run.memoryNeeds)));
         updateReady();
         run.time--;
         if(scheWay==0)
            run.priority--;
         if(run.time==0){
             //进程已经完成
            int memoryStart=run.memoryStart;
            memo.remove(memoryStart);
            ui->progressBarMemory->setValue(memo.sum);
            ui->textBrowserResult->append(QString::number(run.pid)+" 运行完成");
         }else{//如果进程尚未完成，重新添加到就绪队列
            ready.insert(ready.begin(),run);
         }


    }
    //画矩形

}
//建立PCB
void MainWindow::on_pushButtonNewPcb_clicked()
{

    QString name=ui->lineEditName->text();
    QString priority=ui->lineEditPriority->text();
    QString time=ui->lineEditTime->text();
    QString mem=ui->lineEditMem->text();
    ui->lineEditName->clear();
    ui->lineEditPriority->clear();
    ui->lineEditTime->clear();
    ui->lineEditMem->clear();
    if(ui->comboBox->isEnabled()){
        scheWay=ui->comboBox->currentIndex();
        ui->comboBox->hide();
    }
    reserve.push_back(PCB(name.toInt(),priority.toInt(),time.toInt(),mem.toInt()));
    updateReserve();

}
//更新就绪队列的控件显示
void MainWindow::updateReady(){
    int index=0;
    QStringList header;
    header<<"PID"<<"Priority"<<"Time"<<"Memory";
    ui->tableWidget_Ready->clear();
    ui->tableWidget_Ready->setHorizontalHeaderLabels(header);
    vector<PCB>::iterator it=ready.begin();
    while(it!=ready.end()){
        ui->tableWidget_Ready->setItem(index,0,new QTableWidgetItem(QString::number((*it).pid)));
        ui->tableWidget_Ready->setItem(index,1,new QTableWidgetItem(QString::number((*it).priority)));
        ui->tableWidget_Ready->setItem(index,2,new QTableWidgetItem(QString::number((*it).time)));
        ui->tableWidget_Ready->setItem(index,3,new QTableWidgetItem(QString::number((*it).memoryNeeds)));
        index++;
        it++;
    }

}
//更新后备队列的显示
void MainWindow::updateReserve(){
    int index=0;
    QStringList header;
    header<<"PID"<<"Priority"<<"Time"<<"Memory";
    ui->tableWidget_Reserve->clear();
    ui->tableWidget_Reserve->setHorizontalHeaderLabels(header);
    vector<PCB>::iterator it=reserve.begin();
    while(it!=reserve.end()){
        ui->tableWidget_Reserve->setItem(index,0,new QTableWidgetItem(QString::number((*it).pid)));
        ui->tableWidget_Reserve->setItem(index,1,new QTableWidgetItem(QString::number((*it).priority)));
        ui->tableWidget_Reserve->setItem(index,2,new QTableWidgetItem(QString::number((*it).time)));
        ui->tableWidget_Reserve->setItem(index,3,new QTableWidgetItem(QString::number((*it).memoryNeeds)));
        index++;
        it++;
    }
}
//更新挂起队列的显示
void MainWindow::updateHang(){
    int index=0;
    QStringList header;
    header<<"PID"<<"Priority"<<"Time"<<"Memory";
    ui->tableWidget_Hang->clear();
    ui->tableWidget_Hang->setHorizontalHeaderLabels(header);
    vector<PCB>::iterator it=hang.begin();
    while(it!=hang.end()){
        ui->tableWidget_Hang->setItem(index,0,new QTableWidgetItem(QString::number((*it).pid)));
        ui->tableWidget_Hang->setItem(index,1,new QTableWidgetItem(QString::number((*it).priority)));
        ui->tableWidget_Hang->setItem(index,2,new QTableWidgetItem(QString::number((*it).time)));
        ui->tableWidget_Hang->setItem(index,3,new QTableWidgetItem(QString::number((*it).memoryNeeds)));
        index++;
        it++;
    }
}
//响应开始按钮
void MainWindow::on_pushButtonSche_clicked()
{
    t1=new QTimer;
    connect(t1,SIGNAL(timeout()),this,SLOT(pcbsche()));
    QString time=ui->lineEditShiJianPian->text();
    t1->start(time.toInt()*1000);//时间片为1s
    t2=new QTimer;
    connect(t2,SIGNAL(timeout()),this,SLOT(UpdateProcessBar()));
    t2->start(time.toInt()*100);
    pcbsche();
}
//单击获取选中的pid
int MainWindow::getPid(const QModelIndex &index){
    int row=index.row();
    int pid=index.sibling(row,0).data().toString().toInt();
    selectPid=pid;
    return pid;
}
//单击就绪控件
void MainWindow::on_tableWidget_Ready_clicked(const QModelIndex &index)
{

}
//单击挂起控件
void MainWindow::on_tableWidget_Hang_clicked(const QModelIndex &index)
{

}
//响应挂起操作
void MainWindow::on_pushButtonHang_clicked()
{
    for(vector<PCB>::iterator it=ready.begin();it!=ready.end();it++){
        if((*it).pid==selectPid){
            hang.push_back(*it);
            memo.remove((*it).memoryStart);
            ready.erase(it);
            ui->progressBarMemory->setValue(memo.sum);
            ui->textBrowserResult->append(QString::number(selectPid)+"  已挂起");
            updateReady();
            updateHang();
            break;
        }
    }
}
//响应解挂操作
void MainWindow::on_pushButtonWakeUp_clicked()
{
    for(vector<PCB>::iterator it=hang.begin();it!=hang.end();it++){
        if((*it).pid==selectPid){
            if(memo.add(*it)){
                ready.insert(ready.begin(),*it);
                hang.erase(it);
                ui->textBrowserResult->append(QString::number(selectPid)+"  已解挂");
                ui->progressBarMemory->setValue(memo.sum);
                updateReady();
                updateHang();
            }
            else
                ui->textBrowserResult->append(QString::number(selectPid)+"  内存不足以解挂");
            break;
        }
    }
}
//响应暂停按钮，停止时钟
void MainWindow::on_pushButtonSchePause_clicked()//暂停
{
    if(t1->isActive()||t2->isActive()){

        t1->stop();
        t2->stop();
        ui->tableWidgetRun->clear();
        updateReady();
     }
}
//生成随机进程
void MainWindow::on_pushButtonSrand_clicked()
{
    srand((unsigned)time(NULL));
    for(int i=0;i<20;i++){
        PCB a(i,random(1,40),random(1,30),random(1,30));
        ui->textBrowserResult->append(QString::number(i)+"  已进入就绪队列!");
        reserve.push_back(a);
    }
    updateReserve();
}

//画图
void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter;
    painter.begin(this);

    vector<mymemory>::iterator it1=memo.mymem.begin();
    while(it1!=memo.mymem.end()){
        cout<<(*it1).state<<endl;
        if((*it1).state==0){
            painter.setPen(Qt::cyan);
            painter.setBrush(Qt::yellow);
            cout<<"front: "<<(*it1).front<<"  length  "<<(*it1).length<<endl;
            painter.drawRect(20+(*it1).front,470,(*it1).length,40);

        }
        else{
            painter.setPen(Qt::cyan);
            painter.setBrush(Qt::green);
            cout<<"front: "<<(*it1).front<<"  length  "<<(*it1).length<<endl;
            painter.drawRect(20+(*it1).front,470,(*it1).length,40);
        }
        // 设置字体：微软雅黑、大小10
        painter.setPen(Qt::black);
        QFont font;
        font.setFamily("Microsoft YaHei");
        font.setPointSize(7);
        painter.setFont(font);
        //在矩形中间绘制文本
        painter.drawText(QRect(20+(*it1).front,470,(*it1).length,40),Qt::AlignCenter, QString::number((*it1).length));

        it1++;

    }



}
